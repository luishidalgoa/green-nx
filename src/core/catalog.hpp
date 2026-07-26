#pragma once

#include <string>
#include <vector>

#include "auth.hpp"
#include "http.hpp"

namespace gnx {

struct Game {
    std::string title_id;    // xCloud launch id, e.g. "ASSASSINSCREEDSHADOWS"
    std::string product_id;  // Microsoft Store bigId, for metadata lookup
    std::string name;        // filled by fetch_names()
    std::string box_art_url;
};

// Titles the signed-in account can actually play (entitlement or subscription),
// from the xCloud backend's /v2/titles.
std::vector<Game> fetch_playable_titles(Http& http,
                                        const EndpointCredentials& cloud);

// Fills name/box_art_url from the public Microsoft Store display catalog.
// Batched; no authentication required.
void fetch_names(Http& http, std::vector<Game>& games,
                 const std::string& market = "US",
                 const std::string& language = "en-US");

// An Xbox console linked to the account, streamable over xHome (remote play).
struct HomeConsole {
    std::string server_id;     // target for GssvSession::start_home
    std::string name;          // user-given console name
    std::string console_type;  // e.g. "XboxSeriesX"
    std::string power_state;   // e.g. "On", "ConnectedStandby"
};

// Consoles the account can remote-play, from the xhome offering's /v6/servers/
// home. Use the xhome credentials (StreamingCredentials.home), not the cloud
// ones. Empty result = feature stays hidden in the UI.
std::vector<HomeConsole> fetch_home_consoles(
    Http& http, const EndpointCredentials& home);

// Turn a linked console on or off through the Xbox remote-management service
// (the same one the Xbox app's power button uses). Waking is what a stream
// start already does implicitly; turning off has no other route from here.
// Needs the plain Xbox Live credentials, not the streaming ones. Throws on
// failure.
void send_console_power(Http& http, const XblCredentials& xbl,
                        const std::string& console_id, bool turn_on);

}  // namespace gnx
