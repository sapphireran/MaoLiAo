# Portable examples

These programs copy the **gameplay math** out of the EasyX tree so it
can build with any C++11 compiler. They are not a second game: there
is no window, no BMP blit, and no MCI audio.

```
examples/
  include/maoliao_core.hpp   constants + API
  src/maoliao_core.cpp       integrator, AABB, maps, save I/O
  01_inertia.cpp             Inertia::move vs ½at²
  02_aabb.cpp                inset-vertex hits + 2×2 pipes
  03_commands.cpp            command / virtual-key bits
  04_save_load.cpp           gameRecord.dat integer
  05_map_layout.cpp          world 1/2 tables, MAP_NUMBER cap
  06_jump_sim.cpp            101 px design jump, flappy tap
  07_friction.cpp            T1/T2/T3 coast times, camera pin
  08_bullet.cpp              4 px/tick, max range, hold-fire
  Makefile
```

## Build and test

Linux / macOS / MinGW:

```bash
cd examples
make test
```

That compiles `bin/maoliao_core.o` and eight executables, then runs
them. A non-zero exit means a check failed.

Without make:

```bash
mkdir -p bin
g++ -std=c++11 -Iinclude -c src/maoliao_core.cpp -o bin/maoliao_core.o
g++ -std=c++11 -Iinclude 01_inertia.cpp bin/maoliao_core.o -o bin/01_inertia
./bin/01_inertia
```

## What is guaranteed to match the game

| Topic | Source | Example |
| --- | --- | --- |
| `s = vt + ½at²`, `v += at` | `inertia.cpp` | 01 |
| Pixel scale `101 / 3.5` | `define.h` | 01, 06, 07 |
| Jump `vY = -√(2 G 3.5)` | `role.cpp` | 01, 06 |
| Four inset vertices | `Role::isHit` | 02 |
| Pipe ids 8/10 are 2×2 | `Role::hitMap` | 02 |
| Command bit values | `define.h` | 03 |
| Save `"%d"` world 1–3 | `control.cpp` | 04 |
| World 1/2 `Map` literals | `scene.cpp` | 05 |
| `MAP_NUMBER` 30 cap | `scene.h` | 05 |
| Landing snap to 32 px | `Role::action` | 06 |
| Friction `u` and `k=±2` | `scene.cpp`, `role.cpp` | 07 |
| Camera pin at x=192 | `Role::action` | 07 |
| Bullet +4 px, expire 480 | `role.h` / `bullteFlying` | 08 |

## What is deliberately different

- `LIFE` / `F` are integers and doubles, not macros that hide a `;`.
- Save load **fails** on a missing file instead of `fscanf`ing a null
  `FILE*`.
- Map copies use `< count`, not `<= sizeof`.
- World 3 pipe RNG is not simulated (`rand` is unseeded in the game
  anyway).
- No drawing, sound, or MCI alias lifetime.

Read [docs/physics.md](../docs/physics.md) and
[docs/known-issues.md](../docs/known-issues.md) before changing a
constant. If a number here drifts from `define.h`, the example is
wrong — fix the example, not the memory of the game.

## Adding a ninth check

1. Put `09_whatever.cpp` next to the others.
2. `#include "maoliao_core.hpp"` and return 0/1 like the rest.
3. Append `09_whatever` to `PROGRAMS` in the Makefile.
4. One sentence in this README.

Keep new helpers in `maoliao_core` only when two programs need them.
