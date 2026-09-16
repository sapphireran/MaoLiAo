# Portable 猫里奥 examples

These programs rebuild the numbers in `MaoLiAo/define.h`, `inertia.cpp`,
`role.cpp`, and `scene.cpp` without EasyX or Windows. They are personal
study tools: compile them on Linux, read the headers next to the docs in
`../docs`, and change a constant only if you also change the note that
cites it.

## Layout

```
include/maoliao.hpp          umbrella header
include/maoliao/
  constants.hpp              define.h without the trailing-semicolon macros
  inertia.hpp                Inertia::move + jump samples
  tiles.hpp                  Map record + id names
  friction.hpp               u = (V_MAX / T) / G
  collision.hpp              1 px inset vertices vs tile AABBs
  commands.hpp               GetCommand bit masks
  save.hpp                   gameRecord.dat as an optional<int>
  random.hpp                 LCG for the world 3 generator
  pipes.hpp                  seven-column pipe gauntlet
  camera.hpp                 x0 pin + 1/5 sky parallax
  scoring.hpp                coins / stomps / lives / clears
  levels.hpp                 ending distances + authored spawn tables
  hero.hpp                   one Role::action tick
src/demo_*.cpp               printable walkthroughs of each system
tests/test_*.cpp             assertion suite (no extra framework)
```

## Build

Needs a C++17 compiler (`g++` or `clang++`):

```bash
cd examples
make          # demos + tests
make test     # run the assertion suite
make run-demos
make clean
```

Binaries land in `examples/build/`, which is gitignored.

## What each demo shows

| Program | Source it mirrors |
| --- | --- |
| `demo_jump` | Jump launch `-sqrt(2 G H)` and the 101 px apex |
| `demo_friction` | Tile id → T1/T2/T3 → μ |
| `demo_collision` | Inset vertices, 64×64 pipe mouths, world 3 death |
| `demo_commands` | `A\|D\|W\|J` mask and virtual pause bits |
| `demo_save` | One-integer save, reject 0 and 4 |
| `demo_pipes` | Seeded gauntlet, four-tile gaps, cloud + flag |
| `demo_camera` | Pin at x=192, negative `x0`, creeping `xBg` |
| `demo_scoring` | 85-point script + five-death Game Over |
| `demo_walkthrough` | 200 ticks of walk, then one hop on a flat floor |

## Deliberate differences from the game

The headers refuse to copy a few footguns documented in
`docs/known-issues.md`:

- `LIFE` and `F` are `constexpr`, not macros that swallow a `;`
- Loops are `i < n`, not `i <= sizeof(arr)/sizeof(arr[0])`
- `read_world` returns `nullopt` on a missing file instead of crashing
- World 3 heights take an `Engine` so tests can seed them
- A command mask is “held this tick”, not the sticky `_kbhit` latch

Collision, friction `k`, jump scaling, ending distances, and the pipe
formula stay bit-for-bit with the comments in the Windows sources.

## Running a single test

```bash
make build/test_hero && ./build/test_hero
```
