# 猫里奥 MaoLiAo

Personal EasyX / C++ side-scroller. The playable hero is **猫里奥** (Cat Mario). The tree is a 2020 research-and-practice course project that grew into a three-world game with inertia-based movement, tile maps, enemies, shooting, save/load, and a Flappy-Bird-style third stage.

This repository is personal only. It is not company code.

```
Window: 512 x 384
Tile:   32 x 32
Tick:   10 ms  (TIME = 0.01 s)
Lives:  5
Worlds: 3
```

## Play

| Key | Action |
| --- | --- |
| `A` | Move left |
| `D` | Move right |
| `W` or `K` | Jump (world 3 allows mid-air jumps) |
| `J` | Shoot after picking up the weapon pickup |
| `Esc` | Pause menu (resume / restart / home / save) |
| Mouse | Title menu and pause menu |

Score: **+10** per coin, **+5** per stomped or shot enemy. The weapon pickup unlocks shooting and, in world 3, also acts as a star that ignores lethal tiles.

Pause **进行存档** writes the current world index to `MaoLiAo/gameRecord.dat`. Title **读档** reads that file back.

## Build (Windows)

The game itself is a Visual Studio Win32 project and needs EasyX plus Windows MCI audio.

1. Install Visual Studio 2019 or later with the C++ desktop workload (toolset `v142` is what `MaoLiAo.vcxproj` asks for).
2. Install [EasyX](https://easyx.cn/) so `graphics.h` is on the include path.
3. Open `MaoLiAo.sln`.
4. Set the working directory to the `MaoLiAo/` folder so `res\...` bitmap and MP3 paths resolve.
5. Build **Debug | Win32** or **Debug | x64** (those configurations use the MultiByte charset the source was written against). Release configurations are set to Unicode and can mis-handle the Chinese UI strings.
6. Run. The window title is `猫里奥 V2.0`.

Linux / this Cloud Agent environment cannot link EasyX or play the Win32 window. The portable **docs** and **examples** below exist so the game rules can still be read, simulated, and tested without Windows.

## Repository map

```
MaoLiAo.sln                 Visual Studio solution
MaoLiAo/
  main.cpp                  Window, audio aliases, life/world loop
  define.h                  Screen, physics, and command macros
  control.cpp / .h          Title, pause, HUD, save/load
  role.cpp / .h             Hero, enemies, bullets, bombs, scoring
  scene.cpp / .h            Tiles, coins, food, parallax sky
  inertia.cpp / .h          Kinematic step: x = vt + ½at²
  timer.h                   QueryPerformanceCounter sleep
  gameRecord.dat            Last saved world (plain integer)
  res/                      Bitmaps + Chinese-named MP3s
docs/                       Design notes extracted from the sources
examples/                   Runnable ports of the game math (no EasyX)
```

## Documentation

| Note | What it covers |
| --- | --- |
| [docs/README.md](docs/README.md) | Index of every design note |
| [docs/architecture.md](docs/architecture.md) | Classes, loop, globals, draw order |
| [docs/physics.md](docs/physics.md) | Inertia, jump, friction, camera |
| [docs/collision.md](docs/collision.md) | Inset AABB tests used by hero / tiles / pickups |
| [docs/levels.md](docs/levels.md) | Tile IDs, world layouts, ending distances |
| [docs/controls-and-save.md](docs/controls-and-save.md) | Key bits, menus, `gameRecord.dat` |
| [docs/resources.md](docs/resources.md) | Sprite sheets, sky slices, MCI aliases |
| [docs/known-quirks.md](docs/known-quirks.md) | Include-guard typo, `MAP_NUMBER` overflow, off-by-ones |

## Portable examples

The `examples/` tree reimplements the numbers from `define.h` / `inertia.cpp` / `role.cpp` / `scene.cpp` in plain Python (and one C++ harness) so they run on Linux.

```bash
python3 examples/run_all.py
```

See [examples/README.md](examples/README.md) for the physics jump demo, AABB tests, world dumps, and save-file checks.

## History (high level)

- v1.x — grassland scroller, EasyX sprites, first maps and enemies
- v1.4 — art pass (`role.bmp`, `map.bmp`, `ani.bmp`, `scenery.bmp`)
- v2.0 — three worlds, inertia movement, shooting, pause/save, Flappy-style world 3
