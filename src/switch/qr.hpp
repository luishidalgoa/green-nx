#pragma once

#include <SDL2/SDL.h>

#include <string>

#include "gfx.hpp"

namespace gnx {

// Draws `text` as a QR code centred in `destination`, quiet zone included.
// Draws nothing if the text is too long to encode -- the code printed on
// screen stays the way in, the QR is only a shortcut.
void draw_qr(gfx::Gfx& gfx, const std::string& text,
             const SDL_Rect& destination);

}  // namespace gnx
