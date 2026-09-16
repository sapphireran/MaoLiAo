# Study guide

Read the EasyX sources next to the portable programs. Each row is one
afternoon's worth of notes, not a rewrite.

| Start here | Then run | You should be able to explain |
| --- | --- | --- |
| `define.h` + [physics.md](physics.md) | `01_inertia_jump`, `02_friction_run` | Why `vY = -√(2gH)` still jumps *up* on an EasyX screen, and why grass μ is 0.222 |
| `role.cpp` `hitMap` / `isHit` | `03_collision_aabb` | Why a sprite flush on a tile needs the `y+1` ground probe |
| `scene.cpp` `createMap` | `04_level_preview` | Why world 1 lists 32 tiles but only 30 draw |
| `control.cpp` `GetCommand` | `05_command_mask` | Why A+D is mask `3` and both accelerations apply |
| `pauseClick` / `gameStart` read-save | `06_save_roundtrip` | Why `9` in `gameRecord.dat` is "存档缺失" |
| `Role::action` camera clamp + `Scene::action` | `07_camera_scroll` | Why the cat freezes at x=192 while `x0` runs negative |
| enemy loop in `action` | `08_enemy_patrol` | Why a walker turns on a ledge (`hitMap` under the leading foot is null) |
| `bullteFlying` | `09_bullet_flight` | Why `MAX_DISTANCE` is window X, so a shot at x=500 dies instantly |
| score += in `action` | `10_score_events` | 10 / 5 / 5 and why a coin cannot be collected twice |

## Suggested order through the original files

1. `main.cpp` — the infinite loop, music, death / clear branches.
2. `define.h` — every magic number the examples re-export.
3. `inertia.cpp` — four lines; everything else is callers.
4. `role.cpp` `action` — jump, run, camera, pickups, death.
5. `scene.cpp` `createMap` / `show` — data then blit.
6. `control.cpp` — title and pause are the entire UI.

[known-issues.md](known-issues.md) is the punch list if you come back
to change the 2020 code itself. The examples do not patch those bugs;
they describe them (`04` prints `!` on dropped tiles, `09` keeps the
window-X range check).
