# Portable examples

The Windows game needs EasyX. These programs do not. They copy the 2020 formulas and authored level tables into headers under `lib/` and assert that the numbers still mean what the docs say.

```bash
make -C examples
make -C examples test
```

Binaries land in `examples/bin/`. Each process prints a short report and exits `0` on success.

## Programs

| Target | What it checks |
| --- | --- |
| `inertia_sim` | Jump apex near 101 px, grass vs ice brake, air has no friction, `V_MAX` clamp |
| `collision_probe` | Inset corners, 2×2 pipe AABBs, scenery ids skipped, world-3 touch-death, camera rail |
| `level_dump` | ASCII maps for worlds 1–2, overflow vs the 30-stamp cap, a seeded world-3 sketch |
| `save_roundtrip` | `gameRecord.dat` is one integer; rejects empty/garbage; writes a temp file |
| `camera_parallax` | Right-rail scroll, `xBg` ≈ `|x0| / 5`, ending releases the rail |
| `input_decode` | WASD/JK bit masks from `define.h` |

`make test` also diffs `level_dump` against [expected/](expected/README.md).

`level_dump` flags:

```text
level_dump --stored     # default: only the 30 stamps the game actually copies
level_dump --authored   # every stamp in the source initializer
level_dump 1            # one world (also 2 or 3)
```

`save_roundtrip --read <file>` prints `world=` for any dat file.

## Headers

| Header | Game twin |
| --- | --- |
| `lib/constants.hpp` | `MaoLiAo/define.h` |
| `lib/kinematics.hpp` | `Inertia::move` plus jump / friction helpers |
| `lib/aabb.hpp` | `Role::isHit` / `hitMap` / camera clamp |
| `lib/worlds.hpp` | `Scene::createMap` / coins / enemies |
| `lib/input_bits.hpp` | `Control::GetCommand` |
| `lib/save_format.hpp` | pause-menu `fprintf("%d")` |

World 3 pipes are generated, not authored. The dumper uses a tiny LCG so Linux and Windows agree on a **sample** layout. That sample is not MSVC `rand()`.

## What is intentionally not here

No `graphics.h`, no MCI, no `GetAsyncKeyState`. No attempt to play the game in a terminal. For the real loop see [../docs/architecture.md](../docs/architecture.md).
