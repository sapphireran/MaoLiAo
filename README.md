# 猫里奥 MaoLiAo

Personal EasyX side-scroller from a 2020 research-and-development practice course.
Window title: **猫里奥 V2.0**. The playable hero is 猫里奥 (Cat Mario).

This repository is a **personal student project**. It is not company code.

```
512 x 384 window   32 x 32 tiles   3 worlds   5 lives
EasyX + WinMM      Visual Studio   MultiByte debug builds
```

The original game still builds only on Windows with [EasyX](https://easyx.cn/).
The `docs/` and `examples/` trees added later extract the same numbers and
algorithms so they can be read, compiled, and checked on any machine that has
a C++ compiler.

## Play

| Action | Keys |
| --- | --- |
| Move left / right | `A` / `D` |
| Jump | `W` or `K` |
| Shoot (after picking up the weapon) | `J` |
| Pause | `Esc` |

World 3 is a Flappy-style pipe gauntlet. Jump is allowed in the air there.
Touching anything except cloud tiles (`id == 2`) kills the hero unless the
weapon / star pickup has already been collected.

Scoring in `Role::action` / `Role::bullteFlying`:

| Event | Points |
| --- | --- |
| Coin | +10 |
| Stomp an enemy | +5 |
| Bullet hits an enemy | +5 |

Pause menu (Esc): return, restart the current world, quit to the home screen,
or write the current world index into `MaoLiAo/gameRecord.dat`. The home
screen can read that file back.

## Layout

```
MaoLiAo.sln                 Visual Studio 2019 solution
MaoLiAo/
  main.cpp                  window, music, game loop
  define.h                  screen, physics, command bits
  control.*                 home / pause UI, HUD, save
  role.*                    hero, enemies, bullets, bombs
  scene.*                   tiles, coins, food, parallax
  inertia.*                 kinematic step: x = vt + ½at²
  timer.h                   QueryPerformanceCounter sleep
  gameRecord.dat            last saved world (plain integer)
  res/                      bitmaps + MCI mp3 aliases
docs/                       architecture, physics, levels, assets
examples/                   portable C++ demos of the same math
```

## Build the game (Windows)

1. Install Visual Studio 2019+ with the C++ desktop workload.
2. Install EasyX and link it the way the EasyX installer documents.
3. Open `MaoLiAo.sln`.
4. Use **Debug | x86** or **Debug | x64**. Those configurations are MultiByte,
   which matches the Chinese `outtextxy` / `drawtext` strings.
5. Run with the working directory set so `res\` resolves next to the exe
   (the project already loads `"res\\role.bmp"` and friends).

Release | x86 / x64 are Unicode in the checked-in `.vcxproj`. That mismatch
is a known quirk; prefer Debug when you just want the original game to run.

## Portable examples

On a machine with `g++`:

```bash
cd examples
make test
```

Those programs do not open a window. They replay inertia, AABB hits, command
bits, save-file I/O, camera scroll, and the three hand-authored level tables.

## Docs

| File | What it covers |
| --- | --- |
| [docs/architecture.md](docs/architecture.md) | Loop, classes, ownership |
| [docs/physics.md](docs/physics.md) | Jump, friction, camera |
| [docs/levels.md](docs/levels.md) | Worlds 1–3 tile / enemy / coin lists |
| [docs/controls-and-save.md](docs/controls-and-save.md) | Input bits, menus, `gameRecord.dat` |
| [docs/assets.md](docs/assets.md) | Sprite sheets and MCI aliases |
| [docs/build-windows.md](docs/build-windows.md) | VS / EasyX notes |
| [docs/known-quirks.md](docs/known-quirks.md) | Include-guard typo, off-by-ones |
| [examples/README.md](examples/README.md) | How to build the demos |

## History

Course project, 2020. Mid-year versions added maps, enemies, a third
Flappy-style world, then a V2.0 refactor (UI, music, save / load). The
personal docs and examples in this tree were added later so the old source
can be studied without opening Visual Studio.
