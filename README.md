# 猫里奥 MaoLiAo

A personal EasyX / C++ side-scroller from a 2020 research-and-development practice course. The playable character is **猫里奥** (Cat Mario). The current window title is `猫里奥 V2.0`.

This repository is a Windows desktop game: Visual Studio + [EasyX](https://easyx.cn/). The `examples/` folder is portable C++ that does **not** need EasyX, so the movement math, collision rules, save format, and level tables can be inspected on Linux or macOS.

## What you get in a run

- Three worlds: grassland, snow / cloud platforms, then a Flappy-Bird-style pipe run
- Five lives, score, restart / home / save from a pause menu
- Coins, a weapon pickup that unlocks shooting, stomps, and bullets
- Parallax sky (`map` moves with the camera; background scrolls at `1/5` of that)
- MCI music and SFX under `MaoLiAo/res/`

## Controls

| Action | Keys |
| --- | --- |
| Move left / right | `A` / `D` |
| Jump | `W` or `K` |
| Shoot (after the weapon pickup) | `J` |
| Pause | `Esc` |

World 3 allows jump while airborne. Worlds 1–2 only jump when the hero is on a tile.

Pause menu (mouse): return, restart the current world, leave to the title screen, write `gameRecord.dat`. Title screen: start, introduction, controls, exit, read save.

More UI detail: [docs/controls-and-ui.md](docs/controls-and-ui.md).

## Build the game (Windows)

1. Visual Studio 2019 or 2022 with **Desktop development with C++**.
2. Install EasyX for that VS version from [easyx.cn/setup](https://easyx.cn/setup).
3. Open `MaoLiAo.sln`.
4. Prefer **Debug | x86** or **Debug | x64** (those configs are MultiByte, matching the Chinese UI strings).
5. Put the asset pack in `MaoLiAo/res/` (see [docs/resources.md](docs/resources.md)).
6. Set the working directory to `MaoLiAo/` so `res\\*.bmp` and `gameRecord.dat` resolve.

Full notes: [docs/build.md](docs/build.md).

## Build the portable examples

```bash
cd examples
make test
```

That compiles four small programs and runs their self-checks:

- `inertia_demo` — same `x = vt + ½at²` step the game uses, plus a jump table
- `collision_demo` — four-corner AABB used by `Role::isHit`
- `map_layout` — dumps the hardcoded world / coin / enemy tables
- `save_file` — reads and validates `gameRecord.dat`
- `input_bits` — command / pause masks from `define.h`

See [examples/README.md](examples/README.md).

## Repository layout

```
MaoLiAo.sln                 Visual Studio 16 solution
MaoLiAo/
  main.cpp                  window, music aliases, outer game loop
  control.*                 title / pause UI, HUD, key bits
  role.*                    hero, enemies, bullets, hits, score
  scene.*                   tiles, coins, food, camera / sky
  inertia.*                 one-step kinematics
  define.h                  screen, tile, physics, command macros
  timer.h                   QueryPerformanceCounter sleep
  gameRecord.dat            last saved world (1–3)
docs/                       design notes for this personal project
examples/                   portable demos of the same rules
```

## Design notes

| Topic | File |
| --- | --- |
| Loop and class map | [docs/architecture.md](docs/architecture.md) |
| Jump, friction, camera | [docs/physics.md](docs/physics.md) |
| Tile IDs and world data | [docs/levels.md](docs/levels.md) |
| Menus and save file | [docs/controls-and-ui.md](docs/controls-and-ui.md) |
| Expected `res/` files | [docs/resources.md](docs/resources.md) |
| Quirks worth knowing | [docs/quirks.md](docs/quirks.md) |
| One walk frame | [docs/frame-walkthrough.md](docs/frame-walkthrough.md) |

## Credits

Course project, 2020. In-game introduction credits **PWB**. Later personal work lives on this GitHub copy.
