# 猫里奥 (MaoLiAo) V2.0

A personal C++ side-scrolling platformer written with [EasyX](https://easyx.cn/) for a 2020 research-and-development practice course. The player is 猫里奥 (Cat Mario). There are three worlds: two classic platform stages and a pipe-dodging finale inspired by Flappy Bird.

This repository is **personal study / hobby code**. It is not affiliated with Nintendo or any company project.

## What this repo contains

| Path | Role |
| --- | --- |
| `MaoLiAo/` | Visual Studio game sources, resources, and save file |
| `MaoLiAo.sln` | VS 2019 solution (toolset v142) |
| `docs/` | Architecture, physics, levels, controls, assets, known issues |
| `examples/` | Headless ports of the game formulas you can run without Windows / EasyX |

The Windows game itself still needs EasyX and a Visual Studio toolchain. The `examples/` tree is standalone: Python 3 and a C++ compiler are enough to replay jump arcs, friction tables, command bitmasks, save-file I/O, and ASCII maps of worlds 1–2.

## Play the Windows build

1. Install Visual Studio 2019+ with the C++ desktop workload.
2. Install EasyX and make sure `graphics.h` is on the include path.
3. Open `MaoLiAo.sln` and build **Debug | Win32** (the Debug configs use a multi-byte charset, which matches the Chinese asset filenames and UI strings).
4. Run with the working directory set so `res\` resolves to `MaoLiAo/res`.
5. Assets expected at launch: BMP sprites plus MP3 cues listed in [docs/assets.md](docs/assets.md).

## Controls

| Action | Keys |
| --- | --- |
| Move left / right | `A` / `D` |
| Jump | `W` or `K` |
| Shoot (after picking up the weapon mushroom) | `J` |
| Pause | `Esc` |

Pause menu: return, restart the current world, quit to the title screen, or write the current world number to `gameRecord.dat`. The title screen can read that file back. Full UI notes live in [docs/controls.md](docs/controls.md).

## Gameplay in one paragraph

You start with five lives (`LIFE`). Worlds 1 and 2 are tile-platform stages with coins (+10), stomped or shot enemies (+5), and one weapon mushroom that enables shooting. Falling below the screen or walking into an enemy while not falling kills you. World 3 turns most solid tiles lethal (clouds are safe) and allows infinite mid-air jumps. Reach the goal sign and walk off the right edge of the 512×384 window to clear a world; clearing world 3 returns you to the title screen.

## Headless examples

```bash
python3 examples/tests/test_all.py
python3 examples/levels/ascii_map.py --world 1
python3 examples/physics/jump_profile.py
g++ -O2 -o /tmp/inertia_demo examples/physics/inertia_demo.cpp && /tmp/inertia_demo
```

See [examples/README.md](examples/README.md) for the full list.

## Documentation map

- [Architecture and game loop](docs/architecture.md)
- [Physics (inertia, jump, friction, camera)](docs/physics.md)
- [Level data and tile IDs](docs/levels.md)
- [Input, menus, and save format](docs/controls.md)
- [Art and audio inventory](docs/assets.md)
- [Known issues copied from the 2020 sources](docs/known-issues.md)

## License / origin

Course project, 2020. Original author notes in the in-game intro credit “PWB”. This GitHub copy is maintained as a personal archive.
