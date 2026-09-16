# 猫里奥 (MaoLiAo)

A personal C++ side-scrolling platformer from a 2020 research-and-development course.

The playable game is a Visual Studio / EasyX Windows program. This repository now also keeps a portable
documentation set and console examples that reconstruct the original physics, input, collision, camera,
save format, and level tables **without** requiring EasyX or a Windows desktop.

| | |
| --- | --- |
| Window | 512 × 384 pixels |
| Tile size | 32 × 32 |
| Version banner | 猫里奥 V2.0 |
| Lives | 5 |
| Worlds | 3 (grass / snow platforms / Flappy-Bird pipes) |
| Original stack | C++, EasyX, WinMM, Visual Studio 2019 (v142) |

## Play (Windows + EasyX)

1. Install [EasyX](https://easyx.cn/) for Visual Studio.
2. Open `MaoLiAo.sln`.
3. Build **Debug | Win32** or **Debug | x64** (those configurations use the MultiByte charset the
   original EasyX string calls expect).
4. Run with the working directory set so `MaoLiAo/res/` is visible, or copy `res\` next to the exe.

### Controls

| Action | Key |
| --- | --- |
| Move left / right | `A` / `D` |
| Jump | `W` or `K` |
| Shoot (after picking up the weapon) | `J` |
| Crouch (reserved) | `S` |
| Pause menu | `Esc` |

Home menu buttons: **开始** start, **介绍** intro, **指导** controls, **退出** quit, **读档** load.
Pause menu buttons: **返回游戏**, **重新开始**, **退出游戏** (returns to the home screen), **进行存档**.

Scoring: coin +10, stomped or shot enemy +5. World 3 lets the cat jump in mid-air and treats most
solid tiles as lethal unless the weapon / star pickup is active.

## Portable docs and examples

Nothing under `docs/` or `examples/` is company code. It is a personal reconstruction of this 2020
course project so the constants and formulas can be studied on Linux or macOS.

```text
docs/          architecture, physics, levels, input, save, build, quirks
examples/      standalone C++17 programs + Makefile
```

```bash
cd examples
make test
```

Each example asserts against the same numbers that live in `MaoLiAo/define.h`, `role.cpp`, and
`scene.cpp`. See [docs/README.md](docs/README.md) for the reading order.

## Source map

| File | Responsibility |
| --- | --- |
| `MaoLiAo/main.cpp` | Window, music aliases, life / world loop |
| `MaoLiAo/control.cpp` | Home / pause UI, async key bits, save / load |
| `MaoLiAo/role.cpp` | Hero, enemies, bullets, stomps, pickups |
| `MaoLiAo/scene.cpp` | Tile maps, coins, food, parallax sky |
| `MaoLiAo/inertia.cpp` | `x = vt + ½at²` step used by walk and jump |
| `MaoLiAo/timer.h` | QueryPerformanceCounter sleep |
| `MaoLiAo/define.h` | Screen, gravity, command bits, friction times |

## License / origin

Personal coursework (开发者署名 in-game: PWB). Not a commercial Mario clone and not affiliated with
Nintendo. Assets and music live under `MaoLiAo/res/`.
