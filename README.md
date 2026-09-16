# 猫里奥 (MaoLiAo) V2.0

Personal C++ side-scrolling game written with [EasyX](https://easyx.cn/) for a 2020 research-and-practice course. The hero is **猫里奥** (Cat Mario). There are three worlds: a grassland run, a snow / ice parkour, and a Flappy-Bird-style pipe gauntlet.

This repository is a personal / course project. It is not company code.

## What is in this repo

| Path | Role |
| --- | --- |
| `MaoLiAo.sln` | Visual Studio solution (Win32 / x64, Debug / Release) |
| `MaoLiAo/` | Game sources, headers, icon, and save file |
| `docs/` | Architecture, physics, collision, levels, assets, build notes |
| `examples/` | Portable C++ demos of the same formulas used in the game |

The Windows binary expects art and music under `MaoLiAo/res/` at runtime (`res\\map.bmp`, `res\\背景音乐.mp3`, …). Those assets are not always present in a fresh clone; see [docs/assets.md](docs/assets.md) and [docs/build.md](docs/build.md).

## Controls

| Action | Keys |
| --- | --- |
| Move left / right | `A` / `D` |
| Jump | `W` or `K` |
| Shoot (after picking up food) | `J` |
| Pause | `Esc` |

Pause menu: return, restart world, quit to title, write `gameRecord.dat`. Title menu: start, introduction, controls, quit, read save.

## Scoring and lives

- Start with **5** lives (`LIFE` in `define.h`).
- Coin: **+10**. Stomp or shoot an enemy: **+5**.
- Falling below the screen, walking into an enemy while not falling, or (world 3) touching a non-cloud pipe without the food power-up: death.
- Reaching the right edge after the world’s “ending” distance: clear. Clearing world 3 returns to the title screen.

## Architecture in one paragraph

`main.cpp` owns the window, music aliases, and the frame loop. `Control` reads overlapping key bits and draws menus / HUD. `Role` simulates the hero, enemies, bullets, and pickups. `Scene` owns tiled platforms, coins, food, and the scrolling sky. `Inertia::move` is the shared `x = vt + ½at²` integrator. `Timer` is a QueryPerformanceCounter sleep used as the 10 ms tick (`TIME = 0.01`).

Longer write-ups live under [`docs/`](docs/README.md). Standalone experiments that compile without EasyX live under [`examples/`](examples/README.md).

## Build (Windows)

1. Install Visual Studio 2019+ with the C++ desktop workload and [EasyX](https://easyx.cn/).
2. Open `MaoLiAo.sln`.
3. Prefer **Debug | Win32** (the Debug configs use a multi-byte character set, which matches the Chinese string literals).
4. Run with the working directory set to `MaoLiAo/` so `res\\…` paths resolve.

Details, character-set traps, and resource notes: [docs/build.md](docs/build.md).

## Portable examples (any OS with a C++ compiler)

```bash
cd examples
make
make test
```

These programs re-implement the kinematics, AABB test, command bitflags, save-file format, and world-1 layout dump. They do not open a window.

## License

No license file is checked in. Treat the sources as the original author’s personal course work unless a license is added later.
