#include "qr.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

#include "qrcodegen.h"

namespace gnx {

namespace {

// Version 10 holds ~200 bytes at ECC medium -- several times the length of a
// sign-in URL -- and keeps each scratch buffer under 1 KB. The library's
// maximum version would want ~4 KB apiece for no gain here.
constexpr int kMaxVersion = 10;

// Modules of margin around the code. The spec asks for 4, and below that
// phone scanners start missing the finder patterns against a dark screen.
constexpr int kQuietZone = 4;

// The encoded matrix, kept between frames: re-encoding the same URL 60 times
// a second is pure waste. Caching the bytes rather than an SDL texture keeps
// this free of renderer-lifetime problems -- gfx.suspend() destroys every
// texture when the stream takes over the display.
std::string g_text;
std::vector<uint8_t> g_qr;

bool encode(const std::string& text) {
    if (text == g_text && !g_qr.empty()) return true;
    std::vector<uint8_t> qr(qrcodegen_BUFFER_LEN_FOR_VERSION(kMaxVersion));
    std::vector<uint8_t> scratch(qr.size());
    if (!qrcodegen_encodeText(text.c_str(), scratch.data(), qr.data(),
                              qrcodegen_Ecc_MEDIUM, qrcodegen_VERSION_MIN,
                              kMaxVersion, qrcodegen_Mask_AUTO, true)) {
        g_text.clear();
        g_qr.clear();
        return false;
    }
    g_text = text;
    g_qr = std::move(qr);
    return true;
}

}  // namespace

void draw_qr(gfx::Gfx& gfx, const std::string& text,
             const SDL_Rect& destination) {
    if (text.empty() || !encode(text)) return;

    const int size = qrcodegen_getSize(g_qr.data());
    const int modules = size + kQuietZone * 2;

    // Whole pixels per module: a fractional scale rounds some modules one
    // pixel wider than their neighbours, and that unevenness is exactly what
    // makes a phone camera give up on a code.
    const int scale = std::min(destination.w, destination.h) / modules;
    if (scale < 1) return;
    const int side = modules * scale;
    const int x0 = destination.x + (destination.w - side) / 2;
    const int y0 = destination.y + (destination.h - side) / 2;

    // Light plate first, quiet zone included: it is part of the code, and the
    // app's own background is far too dark to scan against.
    gfx.fill({x0, y0, side, side}, gfx::kText);

    const int inset = kQuietZone * scale;
    for (int y = 0; y < size; ++y) {
        // One fill per horizontal run instead of one per module: a 29-module
        // code is 841 modules but only a couple of hundred runs.
        int run = 0;
        for (int x = 0; x <= size; ++x) {
            if (x < size && qrcodegen_getModule(g_qr.data(), x, y)) {
                ++run;
                continue;
            }
            if (run > 0) {
                gfx.fill({x0 + inset + (x - run) * scale,
                          y0 + inset + y * scale, run * scale, scale},
                         gfx::kBg);
                run = 0;
            }
        }
    }
}

}  // namespace gnx
