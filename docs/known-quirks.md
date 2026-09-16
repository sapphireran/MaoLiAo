# Known quirks

These are observations from the 2020 source, not a bug-bash. Fixtures and the validator encode them so a later edit does not “fix” documentation by accident.

## Include guard does not match

`define.h` has both `#pragma once` and:

```c
#ifndef MYDEFINE
#define MYDIFINE
```

`MYDEFINE` is never defined. The `#pragma once` is what actually prevents a double include. Harmless today; fragile if the pragma is stripped.

Several macros also trail a semicolon (`LIFE`, `F`). `int life = LIFE;` expands to `int life = 5;;`, which compiles.

## `MAP_NUMBER = 30` truncates authored maps

World 1 declares 32 tiles; world 2 declares 36. The copy loop stops at 30. Visible consequences:

- World 1 loses two water decorations (x=71 and x=75). Gameplay is unchanged (ID 13 does not collide).
- World 2 loses the victory sign, four trees, and one grass tuft. The “flag” at the end of the snow level is never drawn. The level still completes because `isEnding` is a distance gate at 104 tiles.

`fixtures/levels/world-1.json` and `world-2.json` list `authored_tiles` and `runtime_tiles` separately. The ASCII goldens render **runtime** tiles only.

## Off-by-one copy loops

`createCoin`, `createFood`, `createEnemy`, and world 3 `createMap` use:

```c
while (i <= sizeof(arr) / sizeof(arr[0]))
```

`<=` walks one past the initializer.

- Coins / food / enemies: one extra write into a still-in-capacity slot (`coins[20]` on world 1, etc.) from out-of-bounds source memory. Usually a `{0,0}` that `show` already treats as empty, but it is undefined behavior.
- World 3 maps: `m` has 30 elements, `map` has 30 elements, `i <= 30` writes `map[30]` — **out of bounds**. This is the most serious of the lot. The fixture rebuilds only the 30 defined tiles.

Worlds 1–2 `createMap` correctly uses `i < MAP_NUMBER`.

## World 3 jump and death

`CMD_UP` is accepted every tick in world 3, even while `isFly` is already true. Holding W/K applies `vY = -sqrt(210)` every 10 ms, so the hero can hover. Solids other than clouds kill unless the flower was picked up. The world 3 flower is at pixel (10,10), which is not on any pipe gap — it is effectively a debug leftover.

`hitMap(..., world)` uses the real world only on the vertical probe. Horizontal bumps and enemy AI call `hitMap(..., 1)`, so they never trigger the world-3 death branch.

## Pause label vs. behavior

The third pause button is drawn as **退出游戏** but returns `VIR_HOME`, which `main` treats as “title menu,” not process exit. Process exit is only the title-screen **退出** (`exit(0)`).

## Save path and working directory

`gameRecord.dat` is opened with a bare filename. The copy in `MaoLiAo/gameRecord.dat` is only used if the exe’s cwd is `MaoLiAo/`. Visual Studio’s default cwd is the project directory for this `.vcxproj`, so a Debug launch from the IDE usually hits the right file; a launch from `MaoLiAo.sln`’s folder does not.

## Sticky keys

`getKey` updates only when `_kbhit()` is true, but movement is read from `GetAsyncKeyState`. After you release a key, the last combo can remain latched until another console key event arrives. In practice EasyX’s window plus `_kbhit` makes this feel like mild input lag, not a held-run bug, but it is why a jump can “repeat” after you let go if a previous combo still has `CMD_UP`.

## Character set split

Debug configurations are `MultiByte`. Release is `Unicode`. The source is not `TCHAR`-clean (`outtextxy` with narrow literals, MCI strings). Release builds are the risky ones for Chinese SFX paths.

## Unreachable music close

`mciSendString("close all", ...)` and `closegraph()` sit after `while (true)`. Title-menu **退出** calls `exit(0)` and skips them. Task Manager / Alt+F4 is the other way out.

## No `srand`

World 3 uses `rand()` (`random(a,b)` in `define.h`) without `srand`. Pipe layouts repeat for a given CRT until something else calls `srand`, or they follow the implementation’s default seed (often `1` on MSVC). The fixtures therefore keep an explicit height table instead of pretending to know the CRT seed.

## Quantumult X

This tree has no `[server_remote]`, resource parser, or rewrite script. If you landed here looking for QuanX usage notes, that material is the other personal repo (`quanx-resource-parsers`), not a missing folder in MaoLiAo.
