#include "http.hpp"

#include <curl/curl.h>

#include <mutex>
#include <stdexcept>

namespace gnx {

namespace {

size_t write_cb(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* out = static_cast<std::string*>(userdata);
    out->append(ptr, size * nmemb);
    return size * nmemb;
}

std::string g_ca_bundle;

std::mutex g_forwarded_mutex;
std::string g_forwarded_for;  // guarded by g_forwarded_mutex

int abort_cb(void* userdata, curl_off_t, curl_off_t, curl_off_t, curl_off_t) {
    auto* flag = static_cast<std::atomic<bool>*>(userdata);
    return (flag && flag->load()) ? 1 : 0;  // non-zero aborts the transfer
}

}  // namespace

void Http::set_ca_bundle(std::string path) { g_ca_bundle = std::move(path); }

void Http::set_forwarded_for(std::string ip) {
    std::lock_guard<std::mutex> lock(g_forwarded_mutex);
    g_forwarded_for = std::move(ip);
}

Http::Http() {
    curl_ = curl_easy_init();
    if (!curl_) throw std::runtime_error("curl_easy_init failed");
}

Http::~Http() {
    if (curl_) curl_easy_cleanup(static_cast<CURL*>(curl_));
}

void Http::global_cleanup() { curl_global_cleanup(); }

std::string Http::urlencode(const std::string& value) {
    char* escaped = curl_escape(value.c_str(), static_cast<int>(value.size()));
    std::string result = escaped ? escaped : "";
    curl_free(escaped);
    return result;
}

HttpResponse Http::get(const std::string& url,
                       const std::vector<std::string>& headers) {
    return request("GET", url, nullptr, headers);
}

HttpResponse Http::post(const std::string& url, const std::string& body,
                        const std::vector<std::string>& headers) {
    return request("POST", url, &body, headers);
}

HttpResponse Http::del(const std::string& url,
                       const std::vector<std::string>& headers) {
    return request("DELETE", url, nullptr, headers);
}

HttpResponse Http::request(const char* method, const std::string& url,
                           const std::string* body,
                           const std::vector<std::string>& headers) {
    CURL* curl = static_cast<CURL*>(curl_);
    curl_easy_reset(curl);

    // curl_easy_strerror() only names the error class: every CA-bundle
    // failure reads as "Problem with the SSL CA cert (path? access rights?)"
    // whether the file could not be read, the console refused to import it,
    // or it did not parse. The backend's own reason goes to
    // CURLOPT_ERRORBUFFER, and it is the difference between a bug report
    // that can be acted on and one that cannot.
    char error_buf[CURL_ERROR_SIZE] = {0};
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

    HttpResponse response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    // The console's ssl service is initialised with three sessions for the
    // whole process (switch-curl calls sslInitialize(0x3)) and every TLS
    // connection takes one. libcurl otherwise keeps up to five alive per
    // handle -- one per host it has visited -- so walking from
    // login.microsoftonline.com through the two xboxlive auth hosts to the
    // streaming endpoints runs the console out of sessions, and the next
    // request fails with "SSL connect error" after the TCP connect
    // succeeded. Keep at most one, and hand it back when the transfer ends.
    curl_easy_setopt(curl, CURLOPT_MAXCONNECTS, 1L);
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);
    if (!g_ca_bundle.empty())
        curl_easy_setopt(curl, CURLOPT_CAINFO, g_ca_bundle.c_str());
    if (abort_flag_) {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, abort_cb);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, abort_flag_);
    }

    if (body) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body->c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,
                         static_cast<long>(body->size()));
    }

    curl_slist* list = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_forwarded_mutex);
        if (!g_forwarded_for.empty())
            list = curl_slist_append(
                list, ("X-Forwarded-For: " + g_forwarded_for).c_str());
    }
    for (const auto& header : headers)
        list = curl_slist_append(list, header.c_str());
    if (list) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

    CURLcode code = curl_easy_perform(curl);
    if (list) curl_slist_free_all(list);
    if (code != CURLE_OK) {
        // For TLS failures, name the address the console actually reached: a
        // DNS/hosts block presenting its own certificate then reads as "cert
        // not OK via 127.0.0.1" instead of a guessing game (#58). Only for
        // TLS-class errors -- there a connection certainly happened on THIS
        // attempt, so the handle's last-connection info cannot be stale.
        std::string peer;
        if (code == CURLE_PEER_FAILED_VERIFICATION ||
            code == CURLE_SSL_CONNECT_ERROR) {
            char* peer_ip = nullptr;
            curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &peer_ip);
            if (peer_ip && *peer_ip) peer = std::string(" via ") + peer_ip;
        }
        std::string detail;
        if (error_buf[0]) detail = std::string(" [") + error_buf + "]";
        long os_errno = 0;
        curl_off_t downloaded = 0;
        curl_easy_getinfo(curl, CURLINFO_OS_ERRNO, &os_errno);
        curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &downloaded);
        detail += " <errno=" + std::to_string(os_errno) + " got=" +
                  std::to_string(static_cast<long long>(downloaded)) + "B>";
        throw std::runtime_error(std::string("HTTP request failed: ") +
                                 curl_easy_strerror(code) + detail + " (" +
                                 url + peer + ")");
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status);
    return response;
}

}  // namespace gnx
