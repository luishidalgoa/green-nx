# Branch & contribution map — luishidalgoa/green-nx

A **feature fork** of [rmrf404/green-nx](https://github.com/rmrf404/green-nx),
the Xbox Cloud / remote-play client for Nintendo Switch. This fork adds, on top
of upstream: remote play from your own console (**xHome**), an on-screen
**debug HUD**, audio reliability fixes, a UI redesign, and several UX / quality
improvements. Upstream is credited as the base; general fixes are sent back
upstream as focused PRs.

## Release lines

| Branch | Meaning |
|--------|---------|
| `main` | Last cut release — **v1.1.0** (xHome remote play + audio fixes). |
| `develop` | Integrated next line — **v1.2.0-dev**: everything in `main` plus the quality options, the debug HUD (resolution / FPS / bitrate / packet-loss / buffer), touch menus, UI sounds, friendlier error UX, and the xHome WNS-registration retry. The dev `.nro` builds from here. |

## Contributions sent upstream (rmrf404/green-nx)

| PR | Branch | Status |
|----|--------|--------|
| [#3](https://github.com/rmrf404/green-nx/pull/3) | `fix/signin-exit-hang` | ✅ merged |
| [#4](https://github.com/rmrf404/green-nx/pull/4) | `fix/clean-build` | ✅ merged |
| [#7](https://github.com/rmrf404/green-nx/pull/7) | `feat/ui-redesign` | open |
| [#8](https://github.com/rmrf404/green-nx/pull/8) | `feat/xhome` | open |
| [#11](https://github.com/rmrf404/green-nx/pull/11) | `fix/audio-jitter-reset` | open |

## Feature branches (the units for future PRs)

| Branch | What it is | Upstream status |
|--------|-----------|-----------------|
| `feat/xhome` | Remote play from your own Xbox: GSSV home path, android compatibility profile, SDP fixes, WNS-registration retry | PR #8 |
| `feat/ui-redesign` | "OLED premium" UI redesign | PR #7 |
| `fix/audio-jitter-reset` | Reset the jitter buffer between streams (intermittent no-audio) | PR #11 |
| `feat/audio-limiter` | Soft-knee output limiter on decoded audio | not submitted |
| `feat/settings-volume` | Adjustable output volume + `volume` setting | not submitted |
| `feat/perf-options` | "1080p low-latency" quality option; removes the broken high-bitrate tier | not submitted |
| `feat/ui-sounds` | Menu-navigation tick sounds (SDL audren) | not submitted |
| `feat/touch-menus` | Touchscreen menu navigation + tappable hint bar + swipe scroll | not submitted |
| `feat/ux-errors` | Word-wrapped, friendly error text | not submitted |
| `feat/hud` | Debug HUD **stage 0 only** (overlay-pass proof). The full HUD lives on `develop` — see notes. | needs extraction |
| `feat/multiplayer` | Abandoned (local multiplayer, dropped by design) | not planned |

## Fork-only (never upstreamed)

- `ci/build` — GitHub Actions workflow that builds the `.nro` (fork
  infrastructure; not relevant to upstream).

## Notes for preparing future PRs

- **One focused branch per PR, rebased on `upstream/main` at submit time.** Do
  not open a PR from `develop` — it's an integration line (octopus merges), not
  a clean unit.
- **The debug HUD** (Settings toggle + text overlay + live net-stats) was
  developed integrated on `develop` and *stacks on `feat/perf-options`* (it
  shares the Settings changes). `feat/hud` currently holds only the stage-0
  overlay proof. To upstream the HUD: branch off `feat/perf-options` (or off
  `upstream/main` once the Settings changes land), cherry-pick the HUD commits,
  and split out the unrelated xHome WNS-retry hunk (that one belongs with
  `feat/xhome`).
- **Cutting `main` to v1.2.0** = bump `APP_VERSION` (Makefile) + the build
  workflow version + tag `v1.2.0` + GitHub Release. Do that only with an
  explicit go-ahead.

## History note

Intermediate integration branches (`test/all-v2` … `test/all-v9`,
`test/all-features`) and throwaway build branches (`build/xhome-audio*`,
`build/release-1.1.0`) were pruned on 2026-07-24 after their content was
consolidated into `develop`. No feature work was lost — every feature lives on
its own `feat/*` branch, and the integrated result on `develop`.
