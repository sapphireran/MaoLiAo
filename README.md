# 猫里奥 MaoLiAo

Personal C++ side-scroller from a 2020 研究与开发实践 course. The playable
Windows build still lives under `MaoLiAo/` and talks to [EasyX](https://easyx.cn/)
plus WinMM. This checkout now also carries a **docs + examples lab**: the game
constants, inertia step, AABB hits, camera, save file, and level tables are
re-implemented in Python so they can be studied on a machine that cannot run
EasyX.

This repository is a personal study archive. It is not a product, and it does
not contain any company code.

## What the game is

Cat Mario (猫里奥) walks a 512×384 window through three worlds:

| World | Feel | Pass distance | Jump rule |
| --- | --- | --- | --- |
| 1 | Grass / mixed ground, pipes, coins | `94 * WIDTH` (3008 px) | Grounded jump only |
| 2 | Snow platforms and stairs | `104 * WIDTH` (3328 px) | Grounded jump only |
| 3 | Flappy-style random pipe gaps | `94 * WIDTH` (3008 px) | Jump every frame if `W`/`K` is held |

Five lives. Coins are +10, stomping or shooting an enemy is +5. Eating the
weapon pickup sets `Hero.isShoot`, which unlocks `J` fire **and** makes World 3
pipes non-lethal.

## Controls

| Key | Bit in `define.h` | Action |
| --- | --- | --- |
| `A` | `CMD_LEFT` (1) | Walk left |
| `D` | `CMD_RIGHT` (2) | Walk right |
| `W` or `K` | `CMD_UP` (4) | Jump |
| `S` | `CMD_DOWN` (8) | Reserved |
| `J` | `CMD_SHOOT` (16) | Fire after the weapon pickup |
| `Esc` | `CMD_ESC` (32) | Pause overlay |

Pause overlay (mouse): return / restart / home menu / write `gameRecord.dat`.
Home menu: start / intro / controls / exit / read save.

## Layout

```
MaoLiAo.sln                 Visual Studio 2019 solution
MaoLiAo/
  main.cpp                  Window, music, life/world loop
  define.h                  Screen, physics, command bits
  control.cpp / .h          Keys, menus, HUD, save/load
  role.cpp / .h             Hero, enemies, bullets, AABB
  scene.cpp / .h            Tiles, coins, food, parallax
  inertia.cpp / .h          x = v t + ½ a t²
  timer.h                   QueryPerformanceCounter sleep
  res/                      BMP sheets + MP3 cues
docs/                       How the 2020 code actually behaves
examples/                   Python lab + unit tests (no EasyX)
```

## Build the Windows game

Needs Visual Studio 2019 (toolset v142), the EasyX graphics library, and a
Windows desktop. Open `MaoLiAo.sln`, set **Debug | Win32** (the project is a
multi-byte console subsystem app), and run. Assets load from `MaoLiAo/res`
relative to the working directory, so run from `MaoLiAo/` or copy `res/` next
to the exe.

A longer build note lives in [docs/build.md](docs/build.md).

## Study the systems without Windows

Python 3.10+ is enough. No third-party packages.

```bash
python3 -m unittest discover -s examples/tests -v
python3 -m examples.run_lab --help
python3 -m examples.run_lab jump-table
python3 -m examples.run_lab ascii-map --world 1
python3 -m examples.run_lab tick --frames 120
```

Start with:

1. [docs/architecture.md](docs/architecture.md) — objects and the frame loop
2. [docs/constants.md](docs/constants.md) — every `#define` decoded
3. [docs/physics.md](docs/physics.md) — inertia, jump, friction, camera
4. [docs/levels.md](docs/levels.md) — tile ids and the three worlds
5. [docs/collision.md](docs/collision.md) — four-corner AABB
6. [docs/input-and-ui.md](docs/input-and-ui.md) — bits, pause, save file
7. [docs/sprites-and-audio.md](docs/sprites-and-audio.md) — sheets and MCI aliases
8. [docs/quirks.md](docs/quirks.md) — header-guard typo, `LIFE 5;`, off-by-ones
9. [examples/README.md](examples/README.md) — what each lab module mirrors

## License / origin

Course project, 2020. History includes work merged from
`pang990801/MaoLiAo`. Treat this tree as a personal archive.
