# 猫里奥 (MaoLiAo) V2.0

A personal EasyX / Win32 C++ side-scroller from a 2020 research-and-practice course. The hero is 猫里奥 (Cat Mario). There are three shipped worlds: a grassland stroll, a snow platforming stretch, and a Flappy-Bird-style pipe gauntlet.

This repository is **personal course work**, not a product. The original game still builds as a Visual Studio Win32 project. The `docs/` and `examples/` trees document the engine as it actually exists in `MaoLiAo/` and add portable tools you can run without EasyX.

## Play

| Key | Action |
| --- | --- |
| `A` | Move left |
| `D` | Move right |
| `W` or `K` | Jump (world 3 allows mid-air jumps) |
| `J` | Shoot (after picking up the weapon pickup) |
| `Esc` | Pause: return, restart, quit to menu, or write `gameRecord.dat` |

Start menu (mouse): **开始 / 介绍 / 指导 / 退出 / 读档**.

You have 5 lives. Coins are +10. Stomping or shooting an enemy is +5. Falling below the screen, walking into an enemy, or (in world 3, without the pickup) touching a solid pipe kills you.

## Build the game (Windows)

The playable binary is a Visual Studio 2019+ Win32 / x64 project that depends on **EasyX** (`graphics.h`) and `Winmm.lib`.

1. Install [EasyX](https://easyx.cn/) into Visual Studio.
2. Open `MaoLiAo.sln`.
3. Put the art and music folder `res\` next to the executable (the code loads paths such as `res\\role.bmp` and `res\\背景音乐.mp3`).
4. Build **Debug|Win32** or **Debug|x64** (those configurations use the MultiByte charset the string literals expect).
5. Run. The window title is `猫里奥 V2.0` at 512×384.

The Linux cloud checkout does not have EasyX or the `res\` bitmaps, so the game itself is not compiled here. The example toolkit and physics sandbox **are** portable and are tested on Linux.

## Repository layout

```
MaoLiAo.sln              Visual Studio solution
MaoLiAo/                 Game sources (EasyX + Win32)
  main.cpp               Window, music, life/world loop
  control.*              Keyboard, pause, menus, HUD
  scene.*                Background, tiles, coins, food
  role.*                 Hero, enemies, bullets, collision
  inertia.*              s = vt + ½at² helper
  define.h               Screen, physics, and command macros
  timer.h                QueryPerformanceCounter sleeper
docs/                    Engine notes written from the sources
examples/                Level JSON, a Python toolkit, a C++ sandbox
```

## Docs

- [Architecture](docs/architecture.md) — objects, ownership, and the per-frame loop
- [Game loop and input](docs/game-loop.md) — `main`, `Control`, save file
- [Physics](docs/physics.md) — jump, friction, camera, bullets
- [Map format](docs/map-format.md) — tile ids, collision, authoring units
- [Adding a level](docs/adding-a-level.md) — checklist that matches the current C++ API
- [Resources](docs/resources.md) — `res\` files the binaries expect
- [Known quirks](docs/known-quirks.md) — real limits and off-by-ones in this tree

## Examples

See [examples/README.md](examples/README.md).

```bash
# validate and render the shipped worlds + two extra design files
python3 examples/toolkit/maoliao_levels.py validate examples/levels/*.json
python3 examples/toolkit/maoliao_levels.py render examples/levels/world-1-grassland.json

# same inertia / jump / friction numbers as MaoLiAo/inertia.cpp + define.h
make -C examples/physics test
```

## License / credit

Original course project. In-game credits string: `游戏开发者：PWB`. Fork history includes `pang990801/MaoLiAo`.
