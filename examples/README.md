# Portable examples

These programs rebuild pieces of 猫里奥 **without** EasyX, Win32, or MP3s. They exist so the personal notes in `docs/` stay executable on Linux / macOS / any C++17 toolchain.

They are **not** the game. They share numbers and predicates with `MaoLiAo/` (see the “Source twin” line in each file).

## Build and test

```bash
cd examples
make test
```

`make test` builds six binaries under `build/` and runs them. Every program returns 0 only if its checks pass. `make clean` removes `build/`.

Individual targets:

```bash
make 01_inertia_motion
make 02_aabb_hit
make 03_command_bits
make 04_save_record
make 05_level_friction
make 06_side_scroller_sim
```

## Layout

```
examples/
  Makefile
  common/          shared headers + .cpp (the portable library)
  01_inertia_motion/
  02_aabb_hit/
  03_command_bits/
  04_save_record/
  05_level_friction/
  06_side_scroller_sim/
```

`04_save_record` writes `build/gameRecord.example.dat` only. It never touches `MaoLiAo/gameRecord.dat`.

## What each binary proves

| Program | Game twin | Checks |
| --- | --- | --- |
| 01 | `inertia.cpp`, jump in `role.cpp` | `√(2 G h)`, 101 px apex, 8 m/s cap |
| 02 | `Role::isHit` / `hitMap` / stomp | inset corners, pipe 2× box, scenery ignored |
| 03 | `define.h` bits + pause replace | OR masks, Esc → virtual key |
| 04 | `gameRecord.dat` | accept 1–3, reject 0 and 4 |
| 05 | `Scene::createMap` `u` switch | T1/T2/T3 tables, coast distance |
| 06 | `main` order in miniature | run, camera `x0`, coin, stomp, death, ending |

## Why not link the real `.cpp` files

`role.cpp` and `scene.cpp` include `graphics.h` and call `mciSendString`. Pulling them into this Makefile would require a full EasyX install. The portable library is a deliberate, documented subset.
