# Portable examples

The EasyX game needs Visual Studio and a `res\` folder. This directory is a **personal reconstruction** of the same constants and rules so they can be compiled and checked on any machine with a C++17 compiler and Python 3.

Nothing here links EasyX, opens a window, or plays audio. The header is the spec; the programs are the tests.

## Layout

```
include/maoliao_model.hpp   constants + integrator + maps + camera + save
src/*.cpp                   one concern per program, each exits 0 on success
python/                     independent jump sim, ASCII maps, CSV check
testdata/                   world 1 tiles and coins as CSV
Makefile                    make / make test / make clean
```

## Build and test

```bash
cd examples
make          # g++ -std=c++17, binaries in bin/
make test     # every C++ program + the three Python scripts
make clean
```

`make test` is the whole suite (nine C++ programs plus three Python scripts). Each binary prints a short trace and `name: ok`.

## Programs

| Program | What it reconstructs | Assertions |
| --- | --- | --- |
| `jump_trajectory` | `vY = -√(2 G h)` and `Inertia::move` under gravity | Apex within 0.02 m / 0.6 px of −3.5 m / −101 px |
| `inertia_kinematics` | `u = (V_MAX / T) / G`, the ±2 friction helper, first run tick | Tile `u` tables for worlds 1–3, ~2.3 px/tick at cap |
| `aabb_collision` | Inset-corner vs tile box, 64×64 pipe mouths | Fixture hits, scenery skipped, world-3 kill table |
| `command_bits` | `CMD_*` / `VIR_*` masks and `gameRecord.dat` | Encode D+W, reject world 0/4, accept `"2\n"` |
| `map_layout` | `createMap` arrays and the 30-slot cap | 32→30 and 36→30 drops, finish lines, coin/enemy counts |
| `scoring_rules` | +10 coin, +5 stomp/shot, 5 lives, food arms only | World 1 sweep = 250 |
| `camera_scroll` | Right rail at x=192, `x0` slide, sky `/5` | Hold-D pins the rail; ending ignores it |
| `world3_pipes` | Seven pipe clusters + flap | Mouth rows, gap at `h+1`, flap replaces `vY` |
| `enemy_patrol` | 1 px walk, wall/ledge reverse, stomp vs rise | Reversals on a 5-tile ledge; rising jump is death |

## Header notes

`maoliao_model.hpp` is header-only on purpose: the docs can point at one file for a formula. Names follow the game (`TIME`, `XRIGHT`, `UNREAL_HEIGHT`) so a diff against `define.h` is mechanical.

Copy loops in the **game** use `<= sizeof...` (one past the end). The model uses a strict `<` / `loaded_map` cap. That difference is documented in `docs/known-quirks.md`; the examples assert the *intended* tables.

## Python

`simulate_jump.py` is a second integrator. If it ever disagrees with `jump_trajectory`, the C++ header and the Python file have drifted.

`dump_world_maps.py` paints worlds 1–2 with a 32-pixel-tile alphabet (`=` ground, `~` cloud, `F` sign, `w` water). Dropped rows are omitted from the paint, same as `Scene::createMap`.

`check_testdata.py` refuses a CSV edit that does not match `scene.cpp`.

## Not in scope

- Drawing `role.bmp` / `ani.bmp`
- MCI aliases
- The `_kbhit` sticky-key behaviour (the model treats a mask as data)
- A playable port

For how these numbers sit in the Windows program, start at `docs/architecture.md` and `docs/physics.md`.
