# 猫里奥 (MaoLiAo) V2.0

Personal C++ side-scrolling game written with [EasyX](https://easyx.cn/) for a 2020 research-and-practice course. The playable character is **猫里奥** (Cat Mario). There are three worlds: two authored platform stages and a Flappy-Bird-style pipe gauntlet.

This repository is a personal hobby/course archive. It is **not** company software.

| Item | Value |
| --- | --- |
| Window | `512 × 384` pixels (`XSIZE` × `YSIZE`) |
| Tile size | `32 × 32` (`WIDTH` × `HEIGHT`) |
| Engine | EasyX + Win32 (`GetAsyncKeyState`, MCI sound) |
| Toolchain | Visual Studio 2019 (`v142`), Win32 / x64 |
| Lives | 5 (`LIFE`) |
| Worlds | 1 grassland, 2 snow / ice, 3 random pipes |

## Play

| Key | Action |
| --- | --- |
| `A` / `D` | Move left / right |
| `W` or `K` | Jump (world 3 allows air jumps) |
| `J` | Shoot (after picking up the weapon mushroom) |
| `Esc` | Pause menu |

Pause menu: return to the run, restart the current world, go home, or write `gameRecord.dat`. The home screen can start a new game, show the in-game intro, show the control sheet, exit, or load that save file.

Scoring in the current code:

- Coin: **+10**
- Stomp or shoot an enemy: **+5**

Reach the right-hand end of a world (`Scene::isEnding`) and the hero auto-walks off screen to clear the stage. World 3 is the last stage; clearing it plays the victory sequence and returns to the title screen.

## Repository layout

```
MaoLiAo.sln                 Visual Studio solution
MaoLiAo/
  main.cpp                  Window, music, life/world loop
  define.h                  Tunables and command bit flags
  control.cpp / .h         Title, pause, HUD, save/load
  scene.cpp / .h            Maps, coins, food, parallax
  role.cpp / .h             Hero, enemies, bullets, collision
  inertia.cpp / .h         Kinematic step: x = vt + ½at²
  timer.h                   QueryPerformanceCounter sleep
  gameRecord.dat           Last saved world index (ASCII integer)
docs/                       Design notes extracted from the source
examples/                   Headless tools that do not need EasyX
```

Graphics and audio (`res\*.bmp`, `res\*.mp3`) live next to the executable when you build on Windows. They are not required to run the headless examples.

## Build (Windows)

1. Install Visual Studio 2019 or later with the C++ desktop workload.
2. Install EasyX and confirm `graphics.h` is on the include path.
3. Open `MaoLiAo.sln`.
4. Use **Debug \| Win32** (the project’s Debug configurations are `MultiByte`, which matches the existing string literals).
5. Run. The working directory must contain `res\` (bitmaps and mp3 files) and may contain `gameRecord.dat`.

Details, character-set notes, and a resource checklist: [docs/build.md](docs/build.md).

## Documentation

Start at [docs/README.md](docs/README.md). The notes walk through:

- [Architecture](docs/architecture.md) — classes, ownership, and the main loop
- [Physics](docs/physics.md) — jump, friction, and `Inertia::move`
- [Collision](docs/collision.md) — four-corner tests against tiles, coins, food, enemies
- [Level design](docs/level-design.md) — how `Map` records are authored
- [Tile catalog](docs/tile-catalog.md) — tile ids 1–14
- [Controls](docs/controls.md) — bit flags and pause routing
- [Save format](docs/save-format.md) — `gameRecord.dat`
- [Resources](docs/resources.md) — sprite sheets and MCI aliases
- [Known quirks](docs/known-quirks.md) — include-guard typo, off-by-one loops, and related traps

## Examples (no EasyX, Linux-friendly)

The `examples/` tree reimplements the **numbers and data** from this repo so you can inspect maps, physics, and saves without Visual Studio:

```bash
python3 examples/python/cli.py --help
python3 -m unittest discover -s examples/python/tests -v
make -C examples/cpp
```

See [examples/README.md](examples/README.md).

## License / origin

Course project, 2020. Original in-game credit line: “游戏开发者：PWB”. Personal GitHub copy: [sapphireran/MaoLiAo](https://github.com/sapphireran/MaoLiAo).
