# Portable examples

Small C++ programs that replay MaoLiAo formulas **without EasyX or Windows**. They exist so you can tweak gravity, collision, input bits, the save integer, or the world-1 table on any machine (including this Linux workspace) and get a pass/fail line.

They are personal study aids, not a second game engine.

## Build / test

```bash
cd examples
make
make test
make clean
```

Requires a C++17 compiler (`g++` or `clang++`). No extra libraries.

| Target | What it proves |
| --- | --- |
| `physics_kinematics` | `Inertia::move`, jump `v0`, pixel scale, hang time |
| `jump_arc` | Prints a 50-tick jump table and checks apex ≈ 101 px |
| `aabb_collision` | Four-corner inset test vs tiles, coins, stomps |
| `level_dump` | Renders world 1 as ASCII and checks known cells |
| `command_bits` | WASD/JK mask chords and `VIR_*` values |
| `save_record` | Round-trip `gameRecord.dat` integer 1–3 |
| `camera_rail` | `XRIGHT` clamp, `x0` camera, ending / passed |
| `friction_slide` | Stopping distance by tile `u`, air has no brake |
| `life_cycle` | Deaths, world clears, pause-home, save-does-not-keep-lives |

Shared headers live in `portable/`:

- `maoliao_math.hpp` — constants + integrator from `define.h` / `inertia.cpp`
- `aabb.hpp` — `isHit` / `hitMap` style tests from `role.cpp`
- `level_data.hpp` — world 1 tile / coin / enemy tables from `scene.cpp` / `role.cpp`
- `commands.hpp` — command macros
- `save_io.hpp` — portable fopen read/write

## How this relates to the `.exe`

Numbers are copied from the 2020 sources. Intentional differences:

- Examples use `i < n` loops, not the game’s `i <= sizeof`.
- Examples do not implement sticky `GetAsyncKeyState`.
- Examples do not open bitmaps or MCI.

If you change `G`, `TIME`, `V_MAX`, or the world-1 map in the game, change the matching header here and rerun `make test`.
