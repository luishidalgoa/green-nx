# green-nx

A standalone, open-source **Xbox streaming client for the Nintendo Switch**
(homebrew): stream your **Xbox Game Pass Ultimate** library from the cloud
(xCloud) **and remote-play from your own Xbox** over your network — WebRTC,
hardware H.264 decoding and GPU rendering all done on the console, no companion
apps or PCs involved.

> **About this fork.** Maintained by
> **[@luishidalgoa](https://github.com/luishidalgoa)**, built on the original
> [**green-nx** by rmrf404](https://github.com/rmrf404/green-nx). Most of this
> fork's work — remote play from your own Xbox (xHome), audio-reliability fixes,
> the UI redesign, touch controls, menu sounds, adjustable volume and other UX —
> has been **contributed to and merged upstream** (PRs #3–#19). This build tracks
> upstream and additionally ships the on-screen **debug HUD** (PR #20) and
> **multiple accounts** (PR #23), both in review. All credit for the original
> client goes to rmrf404 — see [Acknowledgements](#acknowledgements).

## What's in this build

Upstream `green-nx` plus two features still in review:

- **On-screen debug HUD** — live **resolution, FPS, bitrate, packet loss and
  audio-buffer depth** over the video while you play, toggleable in
  **Settings → Debug HUD**
  ([PR #20](https://github.com/rmrf404/green-nx/pull/20)).
- **Multiple accounts** — share the console: each account keeps its own sign-in,
  library, favorites and linked Xboxes. Switch or add one from
  **Settings → Accounts**
  ([PR #23](https://github.com/rmrf404/green-nx/pull/23)).

Everything else this fork contributed now ships in upstream itself: remote play
from your own Xbox (xHome), the audio-reliability fixes (RTP demux, jitter reset,
soft-knee limiter, adjustable volume), the "OLED premium" UI redesign,
touchscreen navigation, menu sounds and friendlier error messages.

## Features

- Microsoft **device-code sign-in** (no password typed on console); tokens
  cached on SD
- **Game library** with box-art covers, search, and per-stream quality settings
- **Cloud (xCloud) streaming** *and* **remote play from your own Xbox (xHome)**
- **Native WebRTC streaming**: ICE (Teredo), DTLS-SRTP, SCTP data channels,
  RTP video/audio, NACK/PLI recovery — implemented on-console
- **Hardware H.264 decoding** (NVDEC via FFmpeg's NVTEGRA hwaccel) with
  **zero-copy GPU rendering** (deko3d) — the decoded surface goes straight from
  the decoder to the screen
- Opus audio, 60 fps video, quality tiers **720p / 1080p / 1080p low-latency**
- **Debug HUD** overlay with live streaming stats

## Requirements

- A Nintendo Switch running the [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere) custom firmware
- An active **Xbox Game Pass Ultimate** subscription (for xCloud), and/or your
  own Xbox with remote features enabled (for xHome)
- A 5 GHz Wi-Fi connection is recommended

## Install

Grab `green-nx.nro` from the [Releases](https://github.com/luishidalgoa/green-nx/releases)
page (or build it yourself, below), then:

1. Copy `green-nx.nro` to `sdmc:/switch/` on your SD card.
2. Launch **in title mode** — hold **R** while starting an installed game.
   (Applet mode does not have enough memory for hardware video decoding.)
3. Sign in once with the device code shown on screen (`microsoft.com/link`).
   Tokens and the game list are cached in `sdmc:/switch/green-nx/`.

> **Branches:** `main` tracks upstream plus the two features above; each is also
> kept as its own clean PR branch. See [`docs/BRANCHES.md`](docs/BRANCHES.md)
> for the full map.

### Controls

| Context | Buttons |
| --- | --- |
| Library | Left stick / d-pad or **touch** to move · **A** play · **Y** search · **X** refresh · **ZL** settings · **-** sign out · **+** exit |
| Settings | Stream quality · button layout · vibration · volume · video pacing · sharpness · preferred source (xCloud / your Xbox) · **Debug HUD** on/off · **Accounts** · sign out |
| In stream | Xbox controls mapped from the Switch pad · hold **-** + **+** to quit |

## Build

No local toolchain needed — everything builds inside the `devkitpro/devkita64`
Docker image (this is also what the fork's GitHub Actions workflow does):

```sh
# 1. Build the WebRTC dependencies once (libpeer, libsrtp, usrsctp, mbedTLS)
bash deps/build-switch.sh

# 2. Build the app
docker run --rm -v "$PWD":/src -w /src devkitpro/devkita64 make
```

The result is `green-nx.nro`. A small desktop harness (`make -f Makefile.pc`)
builds the core (auth/catalog/signaling) for development on a PC.

### Layout

```
src/core/            auth, catalog, HTTP, xCloud session + protocol
src/switch/          UI (SDL2), covers, input
src/switch/stream/   streaming engine: WebRTC glue, RTP jitter buffer,
                     NVDEC decoder, deko3d zero-copy renderer + debug HUD, Opus audio
shaders/             deko3d video + HUD shaders (compiled by uam during make)
deps/                build script + patches for the WebRTC stack
docs/BRANCHES.md     branch & contribution map
```

## Acknowledgements

- **[rmrf404](https://github.com/rmrf404)** — author of the original
  [green-nx](https://github.com/rmrf404/green-nx), the foundation this fork is
  built on. This distribution would not exist without it.

green-nx builds on these open-source projects:

| Project | License | Use |
| --- | --- | --- |
| [libpeer](https://github.com/sepfy/libpeer) (patched) | MIT | WebRTC: ICE / DTLS-SRTP / SCTP |
| [libsrtp](https://github.com/cisco/libsrtp) | BSD-3 | SRTP encryption |
| [usrsctp](https://github.com/sctplab/usrsctp) | BSD-3 | SCTP data channels |
| [mbedTLS](https://github.com/Mbed-TLS/mbedtls) | Apache-2.0 | TLS / DTLS crypto |
| [FFmpeg](https://ffmpeg.org) (devkitPro build with [averne's](https://github.com/averne/FFmpeg) NVTEGRA hwaccel) | LGPL-2.1+ | H.264 hardware decoding |
| [deko3d](https://github.com/devkitPro/deko3d) | zlib | GPU rendering |
| [SDL2](https://libsdl.org), SDL2_ttf, SDL2_image | zlib | UI rendering, input |
| [Opus](https://opus-codec.org) | BSD-3 | Audio decoding |
| [libcurl](https://curl.se) | curl | HTTPS |
| [nlohmann/json](https://github.com/nlohmann/json) | MIT | JSON |
| [devkitPro / libnx](https://devkitpro.org) | various | Switch toolchain and OS interface |

The patches applied to libpeer (keyframe requests, NACK, receiver reports,
REMB, data-channel fixes, Switch port, SSRC/payload-type demux) are in
`deps/patches/`.

## Disclaimer

green-nx is a **fun, experimental, non-commercial** hobby project, provided
as-is for personal use. It is **not affiliated with, endorsed by, or supported
by Microsoft or Nintendo** in any way. Xbox, Xbox Cloud Gaming and Game Pass
are trademarks of Microsoft. Nintendo Switch is a trademark of Nintendo. You
need your own valid Game Pass Ultimate subscription and/or your own Xbox; this
project only implements a client for services you already pay for.

## License

[GPL-3.0](LICENSE) — same as upstream. As a GPL-3.0 derivative, this fork keeps
the original license and copyright notices intact.
