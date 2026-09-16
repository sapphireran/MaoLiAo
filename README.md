# 猫里奥 MaoLiAo

Personal EasyX / C++ side-scrolling platformer (猫里奥 V2.0).
Originally a 2020 research-and-development practice course project.

This repository is a **personal** game project. The Windows game still builds
with Visual Studio + EasyX. The `docs/` and `examples/` trees document the
engine and reimplement its gameplay math in portable C++ so the systems can
be studied, tested, and reused without a Windows graphics stack.

## Play (Windows)

| Key | Action |
| --- | --- |
| `A` / `D` | Move left / right |
| `W` or `K` | Jump (world 3 allows air jumps) |
| `J` | Shoot after collecting the weapon mushroom |
| `Esc` | Pause: resume, restart, quit to menu, or save |

Five lives. Three worlds. Coins are +10, stomps and bullet hits are +5.
World 3 is a Flappy-Bird-style pipe run: contact with most solids kills
unless the star/weapon pickup is active.

Build steps: [docs/windows-build.md](docs/windows-build.md).

## Repository layout

```
MaoLiAo.sln                 Visual Studio solution
MaoLiAo/                    EasyX game (the original Windows build)
  main.cpp                  Window, audio aliases, life/world loop
  control.*                 Keyboard bits, menus, HUD, save/load
  role.*                    Hero, enemies, bullets, AABB hits
  scene.*                   Tile map, coins, food, parallax
  inertia.*                 s = vt + ½at² integrator
  timer.h                   QueryPerformanceCounter sleeper
  define.h                  Screen, tile, physics, command macros
docs/                       Architecture and design notes
examples/                   Portable C++ ports of the game math
```

## Documentation

| Document | Contents |
| --- | --- |
| [docs/architecture.md](docs/architecture.md) | Modules, ownership, data flow |
| [docs/game-loop.md](docs/game-loop.md) | `main` tick, death, clear, restart |
| [docs/physics.md](docs/physics.md) | Jump, friction, camera, scale |
| [docs/map-format.md](docs/map-format.md) | Tile IDs, world layouts, pickups |
| [docs/controls.md](docs/controls.md) | Command bits, pause, save file |
| [docs/resources.md](docs/resources.md) | Expected `res/` art and audio |
| [docs/known-issues.md](docs/known-issues.md) | Guard macros, off-by-one loops |
| [docs/windows-build.md](docs/windows-build.md) | VS 2019 + EasyX setup |

## Portable examples

The original game links EasyX, WinMM, and a Windows window. The examples
extract the **same formulas and constants** into a small C++11 library that
builds with `g++` on Linux or MSVC on Windows.

```bash
cd examples
make test
```

See [examples/README.md](examples/README.md) for the program list
(inertia, AABB, command bits, save file, map tables, jump sim, friction,
bullets).

## License / origin

Course-era personal project. Art, audio, and game code stay with the original
authors. The docs and examples in this tree are notes on that personal code.
