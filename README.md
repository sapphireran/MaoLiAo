# 猫里奥 / MaoLiAo

A personal C++ EasyX side-scroller from a 2020 研究与开发实践 course project. The playable game is Windows-only (Visual Studio + EasyX). This repository now also has **portable documentation and examples** that reconstruct the gameplay systems without the EasyX window.

| | |
| --- | --- |
| Engine | EasyX + Win32 (`GetAsyncKeyState`, `mciSendString`) |
| Window | 512 × 384 |
| Tile size | 32 × 32 |
| Lives | 5 |
| Worlds | 3 (grassland, sky platforms, Flappy-style pipes) |
| Version | V2.0 (window title in `main.cpp`) |

## Play (Windows)

1. Install [Visual Studio](https://visualstudio.microsoft.com/) with the C++ desktop workload and [EasyX](https://easyx.cn/).
2. Open `MaoLiAo.sln`.
3. Build **Debug | Win32** or **Debug | x64**.
4. Run with the working directory set so `MaoLiAo/res/` is reachable as `res\` (the exe loads `res\\*.bmp` and `res\\*.mp3` with relative paths).

See [docs/build.md](docs/build.md) for toolset, character-set, and asset notes.

## Controls

| Action | Key |
| --- | --- |
| Move left / right | `A` / `D` |
| Jump | `W` or `K` |
| Shoot (after picking up food) | `J` |
| Pause menu | `Esc` |

The title screen is mouse-driven: 开始 / 介绍 / 指导 / 退出 / 读档.

## Worlds

1. **Grassland** — solid ground, clouds, pipes, coins, one food pickup, ten walkers. Goal around tile column 94.
2. **Sky** — floating cloud platforms over snow ground, stairs of clouds, trees. Goal around tile column 104.
3. **Pipes** — Flappy-style random-height pipe pairs, infinite air jumps, most tiles kill on contact unless you have the food power. Goal around tile column 94.

Level tables, friction, and known overflow of `MAP_NUMBER` are in [docs/levels.md](docs/levels.md).

## Repository layout

```
MaoLiAo.sln                 Visual Studio solution
MaoLiAo/                    game sources, icon, save file, res/
docs/                       architecture, physics, collision, levels, UI
examples/                   portable C++ kernels + demos (Linux/macOS/Windows)
```

The game classes are `Control` (input + menus), `Role` (hero, enemies, bullets), `Scene` (map, coins, food, parallax), `Inertia` (suvat step), and `Timer` (QPC sleep).

## Portable examples

The EasyX game does not build on this Linux environment. The `examples/` tree extracts the formulas and data so they can be compiled with `g++`:

```bash
cd examples
make
make test
```

Demos cover inertia, jump arcs, AABB hits, command bits, save I/O, camera/parallax, and ASCII level previews.

## Documentation

Start at [docs/README.md](docs/README.md).

## Personal project

This is a personal archive of a student game (original in-game credit: PWB). It is not company code. The docs and examples are written so the systems can be studied without opening Visual Studio.
