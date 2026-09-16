# 猫里奥 (MaoLiAo) V2.0

Personal archive of a 2020 *研究与开发实践* course project: a 512×384 EasyX / Win32 side-scroller. The playable binary is Windows-only. This checkout keeps the original Visual Studio sources as they were, and adds a portable **docs + examples lab** so the rules can be studied without EasyX.

| | |
|---|---|
| Window | `512 × 384`, title `猫里奥 V2.0` |
| Loop | `TIME = 0.01` s (`Timer::Sleep(10)` after each frame) |
| Lives | `LIFE = 5` |
| Worlds | 1 grassland, 2 snow / cloud platforms, 3 Flappy-style pipes |
| Input | `A` `D` move · `W` / `K` jump · `J` shoot · `Esc` pause |
| Score | coin **+10** · stomp enemy **+5** · bullet hit **+5** |
| Save | `MaoLiAo/gameRecord.dat` — one integer, world `1`–`3` |

The hero is **猫里奥**. World 3 is the “jump forever / touch a pipe and die” stage (unless the weapon pickup is active). Original in-game credit line: `游戏开发者：PWB`.

## Play (Windows)

1. Install [EasyX](https://easyx.cn/) and Visual Studio 2019+ with the C++ desktop workload (toolset **v142**, Windows SDK 10).
2. Open `MaoLiAo.sln`.
3. Set the working directory to `MaoLiAo/` so `res\*.bmp` and `res\*.mp3` resolve.
4. Build **Debug | x86** (or x64) and run. `Winmm.lib` is pulled in by `#pragma comment` for `mciSendString` music.

Full notes: [docs/build-windows.md](docs/build-windows.md).

## Repository layout

```
MaoLiAo.sln                 Visual Studio 2019 solution
MaoLiAo/
  main.cpp                  window, music aliases, life / world machine
  control.*                 home / pause UI, async keys, save / load
  role.*                    hero, enemies, bullets, bombs, scoring
  scene.*                   tiles, coins, food, parallax sky
  inertia.*                 s = vt + ½at²  (updates v in place)
  timer.h                   QueryPerformanceCounter sleep
  define.h                  tunables and CMD_* / VIR_* bits
  gameRecord.dat            last pause-menu save (currently world 3)
  res/                      bitmaps + mp3 cues
docs/                       personal reference (this kit)
examples/                   portable g++ demos of the same numbers
```

## Docs

- [docs/README.md](docs/README.md) — index
- [Architecture](docs/architecture.md) — `Control` / `Role` / `Scene` / `Inertia` / `Timer` and the frame order
- [Physics](docs/physics.md) — jump apex, friction `u`, camera rail, parallax
- [Collision and combat](docs/collision-and-combat.md) — four-corner AABB, 2× pipes, world-3 instadeath
- [Input, UI, save](docs/input-ui-save.md) — command bits, pause menu, `gameRecord.dat`
- [Levels](docs/levels.md) — authored tables vs what `MAP_NUMBER = 30` actually stores
- [Assets](docs/assets.md) — measured BMP sizes and sprite-sheet rows
- [Quirks](docs/quirks.md) — header-guard typo, `<= sizeof` overruns, clipped world-2 flag
- [Build](docs/build-windows.md) — EasyX / MSVC

## Portable examples

The EasyX game does not build on this Linux agent. The lab under `examples/` replays the same constants and formulas with g++:

```bash
cd examples && make test
```

That builds twelve small programs from `examples/include/maoliao/` and exits 0 only when every self-check prints `ok`. See [examples/README.md](examples/README.md).

## What this PR does not do

- No edits under `MaoLiAo/*.cpp` / `*.h` (the homework binary stays as committed).
- No company code. This tree is the personal GitHub repo `sapphireran/MaoLiAo`.
