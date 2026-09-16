# 猫里奥 / MaoLiAo

Personal 2020 coursework: a 512×384 EasyX platformer. Three worlds, five lives, coins, a weapon pickup, and a Flappy-style pipe stage.

This repository now also has **documentation** and **portable examples** that reconstruct the physics, tile tables, and save format without Visual Studio.

```
docs/          architecture, physics, levels, controls, assets, known issues
examples/      C++17 programs you can build on Linux
MaoLiAo/       original game sources + res/
```

## Play (Windows)

1. Install Visual Studio (v142 toolset) and [EasyX](https://easyx.cn/).
2. Open `MaoLiAo.sln`, build **Debug | Win32** or **Debug | x64**.
3. Run with the working directory set so `res\*.bmp` and `res\*.mp3` resolve.

| Key | Action |
| --- | --- |
| A / D | Run |
| W or K | Jump (world 3: extra hops while held) |
| J | Shoot after the weapon pickup |
| Esc | Pause: resume, restart, title, save |

Saves write `gameRecord.dat` (one integer, the world index) next to the working directory. Title-screen **读档** reads it back.

Full build notes: [docs/building.md](docs/building.md).

## Read the engine

| Doc | Topic |
| --- | --- |
| [docs/index.md](docs/index.md) | Table of contents |
| [docs/architecture.md](docs/architecture.md) | Game loop and coordinate spaces |
| [docs/physics.md](docs/physics.md) | `Inertia::move`, jump, friction |
| [docs/levels.md](docs/levels.md) | Tile ids and the three worlds |
| [docs/controls-and-save.md](docs/controls-and-save.md) | Menus and `gameRecord.dat` |
| [docs/assets.md](docs/assets.md) | Sprite sheets and MCI aliases |
| [docs/known-issues.md](docs/known-issues.md) | 30-stamp cap, UB copies, world-3 units |

## Portable examples (no EasyX)

```bash
make -C examples test
```

`inertia_sim` integrates a hop and checks the apex against `UNREAL_HEIGHT`. `level_dump` prints ASCII maps and reports how many authored stamps the `MAP_NUMBER = 30` array actually keeps. See [examples/README.md](examples/README.md).

## Layout

```
MaoLiAo.sln
MaoLiAo/
  main.cpp          loop, lives, music
  control.cpp       title / pause / HUD
  role.cpp          hero, enemies, bullets
  scene.cpp         tiles, coins, parallax
  inertia.cpp       x = vt + ½at²
  define.h          constants
  res/              bmp + mp3
```

Credit on the in-game intro screen: **PWB**. Window title: `猫里奥 V2.0`.
