# Branch & contribution map — luishidalgoa/green-nx

A fork of [rmrf404/green-nx](https://github.com/rmrf404/green-nx) — the Xbox
Cloud / remote-play client for Nintendo Switch — maintained by
[@luishidalgoa](https://github.com/luishidalgoa). Most of this fork's work has
been contributed to and **merged upstream**; `main` tracks upstream and
additionally carries the on-screen debug HUD.

## Branches

| Branch | Meaning |
|--------|---------|
| `main` | Upstream `green-nx` + the debug HUD ([PR #20](https://github.com/rmrf404/green-nx/pull/20)) + multiple accounts ([PR #23](https://github.com/rmrf404/green-nx/pull/23)) + this doc. Kept in sync with upstream. |
| `develop` | Working line, same as `main`. |
| `pr/debug-hud` | The debug HUD as the clean PR #20 branch (off `upstream/main`). |
| `pr/multi-account` | Multiple accounts as the clean PR #23 branch (off `upstream/main`). |
| `ci/build` | The fork's fast GitHub Actions workflow (fork infra; not upstreamed). |

Tag `v1.2.0` marks the last standalone fork release (before syncing to upstream);
the `.nro` is on the [Releases](https://github.com/luishidalgoa/green-nx/releases)
page.

## Contributions to upstream (rmrf404/green-nx)

| PR | What | Status |
|----|------|--------|
| [#3](https://github.com/rmrf404/green-nx/pull/3) | sign-in exit-hang fix | ✅ merged |
| [#4](https://github.com/rmrf404/green-nx/pull/4) | clean-clone build fix | ✅ merged |
| [#7](https://github.com/rmrf404/green-nx/pull/7) | "OLED premium" UI redesign | ✅ merged |
| [#8](https://github.com/rmrf404/green-nx/pull/8) | remote play from your own Xbox (xHome) | ✅ merged |
| [#11](https://github.com/rmrf404/green-nx/pull/11) | jitter-buffer reset (intermittent no-audio) | ✅ merged |
| [#14](https://github.com/rmrf404/green-nx/pull/14) | menu navigation sounds | ✅ merged |
| [#15](https://github.com/rmrf404/green-nx/pull/15) | word-wrapped, friendlier error messages | ✅ merged |
| [#16](https://github.com/rmrf404/green-nx/pull/16) | touchscreen navigation | ✅ merged |
| [#17](https://github.com/rmrf404/green-nx/pull/17) | "1080p low-latency" quality option | declined (maintainer keeps the present pacing) |
| [#19](https://github.com/rmrf404/green-nx/pull/19) | adjustable volume + soft-knee limiter | ✅ merged |
| [#20](https://github.com/rmrf404/green-nx/pull/20) | on-screen debug HUD | in review |
| [#23](https://github.com/rmrf404/green-nx/pull/23) | multiple accounts on one console | in review |

`main` merges #20 and #23 together; they both add a Settings row before *Sign
out*, so the merge orders them Debug HUD → Accounts → Sign out. Whichever PR
lands upstream second needs that same trivial rebase.

## Not upstreamed (yet)

- **xHome WNS-registration retry** — retry the home session on
  `WaitingForServerToRegister` (console still registering) and bump home attempts
  4 → 6. Improves reconnecting to your own Xbox; carved out of the HUD work but
  not yet its own PR.
