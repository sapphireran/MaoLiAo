# 猫里奥 / MaoLiAo

C++ 横版过关游戏。2020 年《研究与开发实践》课程项目，Windows + [EasyX](https://easyx.cn/) 图形库。

This repository is the personal 2020 course build of **猫里奥** (Cat Mario): a 512×384 EasyX window, three authored worlds, inertia-based movement, and a pause-menu save slot. It is **not** a Quantumult X project. Personal QuanX resource-parser notes live in [`sapphireran/quanx-resource-parsers`](https://github.com/sapphireran/quanx-resource-parsers).

The original README was a one-line stub. The files under [`docs/`](docs/) and [`fixtures/`](fixtures/) reconstruct how the binary actually behaves: tile IDs, friction, camera math, hardcoded entity tables, and the `MAP_NUMBER = 30` cap that silently drops later scenery.

## What you get

| World | Feel | Length to auto-run | Notes |
| --- | --- | --- | --- |
| 1 | Grass / snow platformer | 94 tiles (3008 px) | Coins on clouds and a late staircase; one flower unlocks shooting |
| 2 | High snow / cloud parkour | 104 tiles (3328 px) | Stepped cloud stairs near x=67; victory sign and trees are authored but **not loaded** (see [known quirks](docs/known-quirks.md)) |
| 3 | Flappy-style pipe run | 94 tiles (3008 px) | Infinite jump; any solid except clouds kills unless you already have the flower; pipe gaps are `rand()` each death |

Lives start at 5. Score: coin +10, stomp or bullet-kill +5. Clearing world 3 plays the victory + all-clear jingles and returns to the title menu.

## Build (Windows)

The `.sln` is Visual Studio 2019 (`v142`, Windows 10 SDK, Win32/x64). EasyX must be installed into that toolchain. This Linux checkout cannot link `graphics.h`; treat the commands below as the Windows recipe.

1. Install Visual Studio 2019 or later with the C++ desktop workload.
2. Install EasyX for the same toolset (the project includes `<graphics.h>` and `<conio.h>`).
3. Open `MaoLiAo.sln`. Prefer **Debug | Win32** or **Debug | x64** (`CharacterSet` is `MultiByte`, which matches the Chinese `res\*.mp3` paths and `outtextxy` literals). Release | Win32/x64 is `Unicode` and will mis-handle those strings unless you change it.
4. Set the working directory to `MaoLiAo/` so `res\map.bmp` and `gameRecord.dat` resolve. From a Developer Command Prompt:

```bat
msbuild MaoLiAo.sln /p:Configuration=Debug /p:Platform=x64
cd MaoLiAo
.\x64\Debug\MaoLiAo.exe
```

Expected window title: `猫里奥 V2.0`. Assets are loaded with EasyX `loadimage` / MCI (`Winmm.lib`):

| Path | Role |
| --- | --- |
| `res\mapsky.bmp` | 512×1536 sky strip; world *n* uses slice `y = -(n-1)*384` |
| `res\map.bmp` | 32×32 terrain + pipe sheet |
| `res\scenery.bmp` | Animated trees / water (IDs 11–14) |
| `res\ani.bmp` | Coins, flower, enemies, bullets, explosions |
| `res\role.bmp` | Hero walk / death frames |
| `res\home.bmp` | 512×1920 stacked UI (title, game over, level clear, all-clear) |
| `res\背景音乐.mp3` … | BGM and SFX opened as MCI aliases in `main.cpp` / `role.cpp` |

A current save stub is checked in as `MaoLiAo/gameRecord.dat` (the single digit `3`). Pause → **进行存档** overwrites it with the current `world`.

## Play

Mouse on the title screen: **开始** / **介绍** / **指导** / **退出** / **读档**.

| Key | Action |
| --- | --- |
| A / D | Walk. Camera holds the hero between x=0 and x=192 (`WIDTH*6`). Past that, `Hero.x0` scrolls the map. |
| W or K | Jump. Worlds 1–2 need ground contact. World 3 allows mid-air jumps every tick you hold the key. |
| J | Shoot (only after picking up the flower). Hold-to-repeat uses a 0.2 s gate. |
| S | Bound but unused. |
| Esc | Pause overlay: return, restart world, title (`life` reset to 5, `world` reset to 1), or write `gameRecord.dat`. |

Stomp enemies while falling (`vY > 0`). Side contact while not stomping is death. Falling below the 384 px window is death. World 3 treats most solid tiles as instant death until `Hero.isShoot` is true (the flower). Bullets travel 4 px/tick, explode on enemy, terrain, or x > 480, and never leave the 512 px window.

## Repository map

```text
MaoLiAo.sln                 Visual Studio solution
MaoLiAo/
  main.cpp                  Window, MCI, life/world loop
  define.h                  Screen, physics, and key bitmasks
  control.*                 Title, HUD, pause, save/load
  scene.*                   Camera, tiles, coins, flower
  role.*                    Hero, enemies, bullets, collisions
  inertia.*                 s = vt + ½at² helper
  timer.h                   QueryPerformanceCounter sleeper
  gameRecord.dat            Last pause-menu save (world index)
  res/                      Bitmaps + MP3s
docs/                       Architecture, physics, levels, quirks
docs/examples/              Save-file walkthrough and validator recipes
fixtures/                   Machine-readable levels, tiles, physics, saves
tools/                      Source-sync checks, ASCII render, fixture tests
```

Read in this order:

1. [`docs/architecture.md`](docs/architecture.md) — frame loop and class boundaries
2. [`docs/physics-and-combat.md`](docs/physics-and-combat.md) — inertia, friction, jump, bullets
3. [`docs/level-design.md`](docs/level-design.md) — tile IDs, per-world tables, `MAP_NUMBER` cap
4. [`docs/controls-and-saves.md`](docs/controls-and-saves.md) — input bits and `gameRecord.dat`
5. [`docs/known-quirks.md`](docs/known-quirks.md) — include-guard typo, off-by-one copies, dropped scenery
6. [`docs/assets.md`](docs/assets.md) — bitmap sheets and MCI aliases
7. [`fixtures/README.md`](fixtures/README.md) — JSON schema and how to re-check it
8. [`docs/examples/README.md`](docs/examples/README.md) — runnable fixture commands

## Verify the fixtures (Linux / macOS / Windows)

The game itself needs EasyX. The **data** does not. Python 3.9+ with no third-party packages:

```bash
python3 tools/validate_fixtures.py
python3 tools/render_level.py --world 1
```

`validate_fixtures.py` will:

- parse `MaoLiAo/define.h` and compare it to `fixtures/physics.json`
- extract the live `Map` / `POINT` / `Enemy` literals from `scene.cpp` and `role.cpp`
- compare them to `fixtures/levels/world-1.json` and `world-2.json`
- rebuild world 3 from the documented `rand()%(b-a)+a` rules plus the example height table
- reject tile IDs outside 1–14 and flag rows past `MAP_NUMBER`
- render ASCII maps and diff them against `fixtures/expected/`
- classify every file in `fixtures/saves/` the same way `Control::gameStart` does (`1..3` ok, else “missing archive”)

That is the portable stand-in for “open the editor and stare at the initializer lists.”

## Constants (authoritative copy)

Copied from `define.h`. `fixtures/physics.json` must match.

| Macro | Value | Meaning |
| --- | --- | --- |
| `XSIZE` × `YSIZE` | 512 × 384 | Window |
| `WIDTH` × `HEIGHT` | 32 × 32 | Tile / sprite cell |
| `TIME` | 0.01 s | Fixed tick |
| `LIFE` | 5 | Hits before game over |
| `V_MAX` | 8.0 | Horizontal speed cap (pre-scale) |
| `A_ROLE` | 20.0 | Walk acceleration |
| `G` | 30.0 | Gravity |
| `REAL_HEIGHT` | 3.5 | “Meters” used in the jump energy formula |
| `UNREAL_HEIGHT` | 101 | Peak jump in pixels (`3*HEIGHT+5`) |
| `XRIGHT` | 192 | Camera lock (`WIDTH*6`) |
| `K_MAP_BG` | 5 | Map pixels per 1 background pixel |
| `T1` / `T2` / `T3` | 0.5 / 1.2 / 1.5 | Time-to-`V_MAX` for high / medium / low friction |

Friction stored on each `Map` is `(V_MAX / Tn) / G`. Worlds 1–2 treat IDs 1, 2, 3, 4, 5 as medium (`T2`) and ID 6 (pipe body) as high (`T1`). World 3 moves IDs 1 and 3–6 onto `T1`.

## License / provenance

Personal course work from 2020 (original remote `pang990801/MaoLiAo`). No company code. Do not commit subscription URLs, account IDs, or other private material here — this tree has none, and should stay that way.
