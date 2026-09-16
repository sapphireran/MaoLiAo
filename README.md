# 猫里奥 MaoLiAo

Personal C++ side-scroller (EasyX / Visual Studio) from a 2020 研究与开发实践 course project.
The playable game stays Windows-only. This repository now also carries **personal documentation** and **portable examples** that unpack the systems I wrote: inertia, AABB hits, command bits, tile friction, and `gameRecord.dat` save/load.

> Not affiliated with Nintendo. This is a student clone for study, not a commercial product.

## What you can do here

| Path | What it is |
| --- | --- |
| [`MaoLiAo/`](MaoLiAo/) | Original EasyX game (needs Visual Studio + EasyX on Windows) |
| [`docs/`](docs/) | Architecture, loop, physics, collision, levels, controls, assets, known quirks |
| [`examples/`](examples/) | Standalone C++17 programs that rebuild those algorithms without EasyX |

If you only want to **read** how the game works, start at [`docs/README.md`](docs/README.md).
If you want to **run** the extracted math on Linux or macOS, start at [`examples/README.md`](examples/README.md).

## Play (Windows)

1. Install [Visual Studio](https://visualstudio.microsoft.com/) with the C++ desktop workload (toolset v142 or later).
2. Install [EasyX](https://easyx.cn/) and make sure `graphics.h` is on the include path.
3. Open `MaoLiAo.sln`.
4. Build **Debug | x86** or **Debug | x64**.
5. Run with the working directory set so `MaoLiAo/res/` (bitmaps + MP3s) and `MaoLiAo/gameRecord.dat` resolve as `res\...` and `gameRecord.dat`.

Window size is **512×384**. Title string: `猫里奥 V2.0`.

### Controls

| Action | Key |
| --- | --- |
| Move left / right / duck | `A` / `D` / `S` |
| Jump | `W` or `K` |
| Shoot (after picking up the weapon) | `J` |
| Pause menu | `Esc` |

Pause menu (mouse): 返回游戏, 重新开始, 退出游戏 (returns `VIR_HOME` and reopens the title), 进行存档 (writes the current world index to `gameRecord.dat`).

Title menu: 开始, 介绍, 指导, 退出, 读档.

### Scoring and lives

- Start with **5** lives (`LIFE` in `define.h`).
- Coin: **+10**. Stomp or shoot an enemy: **+5**.
- Fall below the screen or walk into an enemy (without stomping) → death.
- World 3 is the Flappy-style pipe gauntlet: most solid tiles kill unless you have the star/weapon flag.

Three worlds. Clearing world 3 plays the all-clear sequence and returns to the title.

## Source map

```
MaoLiAo.sln                 Visual Studio 2019 solution
MaoLiAo/
  main.cpp                  Window, music aliases, main loop
  define.h                  Screen, timing, physics, command bits
  control.h/.cpp            Keyboard, menus, HUD, save/load
  role.h/.cpp               Hero, enemies, bullets, bombs, hits, score
  scene.h/.cpp              Tiles, coins, food, parallax sky
  inertia.h/.cpp            x = v t + ½ a t² and v += a t
  timer.h                   QueryPerformanceCounter sleep
  gameRecord.dat            Plain-text world index (example: "3")
  res\                      Bitmaps + MP3s (not always in git)
```

Loop, in order, every ~10 ms (`TIME = 0.01`):

1. `Control::getKey()` — async WASD/JK + Esc pause
2. `Role::action` — jump, inertia, camera `x0`, enemies, coins, food, bullets
3. `Scene::action` — parallax sky follows `Hero::x0`
4. Death / pass-level branches (music + interstitial from `home.bmp` strips)
5. `BeginBatchDraw` → scene → role → score → level → `EndBatchDraw`
6. `Timer::Sleep(TIME * 1000)`

Details: [`docs/game-loop.md`](docs/game-loop.md).

## Portable examples (no EasyX)

```bash
cd examples
make test
```

These programs do **not** open a window. They assert the same numbers the game uses (`G = 30`, `REAL_HEIGHT = 3.5`, `UNREAL_HEIGHT = 101`, tile AABB, bit flags, save file).

## Docs index

- [Architecture](docs/architecture.md) — classes, globals, who owns what
- [Game loop](docs/game-loop.md) — `main` states and music aliases
- [Physics](docs/physics.md) — jump, run, friction, camera
- [Collision](docs/collision.md) — four-vertex AABB, stomp, bullets
- [Levels](docs/levels.md) — tile IDs, world layouts, ending distances
- [Controls and save](docs/controls-and-save.md) — command bits, pause, `gameRecord.dat`
- [Assets](docs/assets.md) — sprite sheets and `home.bmp` strips
- [Known quirks](docs/known-quirks.md) — include-guard typo, `LIFE` semicolon, off-by-one loops

## License / origin

Course project, 2020. Personal archive of Sapphire Ran (`sapphireran/MaoLiAo`). Treat art and audio as study assets; do not redistribute as an official Mario product.
