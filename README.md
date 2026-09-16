# 猫里奥 MaoLiAo

Personal EasyX / C++ side-scroller from a 2020 research-and-development practice course.

The playable window is **512×384**. The cat (猫里奥) runs through three worlds: a grassland stage, a snow / platform stage, and a Flappy-Bird-style pipe gauntlet. Lives, coins, mushrooms, stomps, and bullets all live in a handful of source files under `MaoLiAo/`.

This repository is **personal course work**. It is not company code.

---

## Play (Windows + EasyX)

| Action | Keys |
| --- | --- |
| Move left / right | `A` / `D` |
| Jump | `W` or `K` |
| Shoot (after eating a mushroom) | `J` |
| Crouch (bound, unused in gameplay) | `S` |
| Pause | `Esc` |

Pause menu (mouse):

- 返回游戏 — resume
- 重新开始 — rebuild the current world
- 退出游戏 — return to the home screen (`VIR_HOME`)
- 进行存档 — write the current world index to `gameRecord.dat`

Home screen: 开始, 介绍, 指导, 退出, 读档.

You start with **5 lives**. Coins are **+10**. Stomping or shooting an enemy is **+5**. World 3 kills on contact with any solid tile that is not a cloud (`id != 2`) unless the cat already picked up the weapon mushroom.

---

## Build

The original target is **Visual Studio 2019+** with **EasyX** on Windows.

1. Install [EasyX](https://easyx.cn/) for your MSVC toolset.
2. Open `MaoLiAo.sln`.
3. Build **Debug | Win32** (the checked-in project uses MultiByte / `v142`).
4. Run with a `res\` folder next to the executable. The code loads bitmaps and MP3s from that folder (`mapsky.bmp`, `map.bmp`, `role.bmp`, `ani.bmp`, `home.bmp`, `scenery.bmp`, and several `mciSendString` aliases).

Linux / macOS cannot link EasyX. The portable physics, collision, camera, save, and scoring kernels are extracted under [`examples/`](examples/README.md) and compile with `g++`.

```bash
make -C examples
make -C examples test
```

---

## Source map

| File | Responsibility |
| --- | --- |
| `MaoLiAo/main.cpp` | Window, music aliases, main loop, death / clear / restart |
| `MaoLiAo/define.h` | Screen, physics, and command-bit constants |
| `MaoLiAo/control.cpp` | Home UI, pause UI, HUD, save / load |
| `MaoLiAo/scene.cpp` | Tile maps, coins, food, parallax background |
| `MaoLiAo/role.cpp` | Hero, enemies, bullets, bombs, hit tests |
| `MaoLiAo/inertia.cpp` | `x = vt + ½at²` integrator used by walk and jump |
| `MaoLiAo/timer.h` | High-resolution `QueryPerformanceCounter` sleep |

Long-form notes live in [`docs/`](docs/README.md).

---

## Worlds

| World | Feel | End distance | Notes |
| --- | --- | --- | --- |
| 1 | Grass, pipes, water, clouds | `94 * WIDTH` (3008 px) | Classic walk + stomp |
| 2 | Snow platforms and stairs | `104 * WIDTH` (3328 px) | Tighter jumps |
| 3 | Random pipe gaps | `94 * WIDTH` (3008 px) | Multi-jump; most tiles are lethal |

After `Scene::isEnding` the cat auto-walks right. When `hero.x > XSIZE` the world is cleared. Clearing world 3 plays the victory + all-clear sequence and returns to the home screen.

---

## License / credit

Course project, 2020. Original in-game credit line: **PWB**. Personal archive maintained by Sapphire Ran (`sapphireran`).
