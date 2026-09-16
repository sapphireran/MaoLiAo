# Known quirks

These are behaviours visible in the current sources. The examples copy the **intentional numbers** (tile tables, `TIME`, `G`, …) but refuse to crash the way a few of the C loops can.

## Include guard never fires

`define.h`:

```cpp
#ifndef MYDEFINE
#define MYDIFINE
```

The guard macro is misspelled (`MYDIFINE`). Every include re-parses the file. Harmless in practice because of `#pragma once` at the top.

## `LIFE` macro includes a semicolon

```cpp
#define LIFE 5;
int life = LIFE;   // → int life = 5;
```

`life = LIFE` in assignment statements also works (`life = 5;`). Do not write `int remaining[LIFE]` — that would not compile.

## `MAP_NUMBER = 30` truncates authored tiles

World 1 lists 32 `Map` records; indices 30–31 (water at columns 71 and 75) are dropped.

World 2 lists 36; the goal sign and all four trees sit at index ≥ 30 and never copy.

`examples/python/cli.py lint` reports `truncated_records`.

## Off-by-one `<= sizeof` loops

`createCoin`, `createFood`, `createEnemy` use:

```cpp
while (i <= sizeof(arr) / sizeof(arr[0]))
```

The valid indices are `0 .. n-1`. The extra iteration reads one struct past the array and writes it into `coins[]` / `food[]` / `myEnemy[]`. Depending on stack contents this can spawn a garbage coin or a `{0,0}` no-op.

World 3 `createMap` uses the same `<=` bound **and** does not stop on `id == 0`, so it can write `map[n]` as well.

The JSON mirrors contain only the **authored** records. Lint compares `len(records)` to `MAP_NUMBER`.

## World 3 food is in pixels `{10, 10}`

Worlds 1–2 store food in pixels too, but at tile-aligned locations. World 3’s `{10, 10}` sits near the top-left of the world, almost certainly off the intended pipe course.

## `random` without `srand`

World 3 heights call `rand()` with the CRT default seed. Layouts repeat per-process until `rand` advances. Death reconstructs `Scene` and therefore rolls **new** pipe heights.

`random(a,b)` is `rand() % (b-a) + a`, so `random(1,7)` → `{1,2,3,4,5,6}`.

## Bullet function name

`bullteFlying` (missing ‘e’) in `role.h` / `role.cpp`.

## Pause “退出游戏” is home, not exit

`VIR_HOME` resets to the title screen via `main`. Process exit is only the title **退出** button (`exit(0)`).

## `MessageBox` argument order

Load-failure box: `MessageBox(hwnd, "提醒", "存档缺失，请检查存档", 1)` — in the Win32 API the second argument is **text** and the third is **caption**, so the user sees a window titled “存档缺失，请检查存档” with body “提醒”.

## `hitMap(..., world=1)` in several paths

Horizontal wall resolve, enemy patrol, and bullets pass `world = 1` even on stage 3. That avoids instant death on those tests; landing / grounded tests pass the real `world`.

## `Timer` in a header

`timer.h` defines static members. Second include from another `.cpp` would duplicate symbols. Only `main.cpp` includes it.

## Unreachable shutdown

`closegraph()` and `mciSendString("close all")` sit after `while (true)`.

## Release | Unicode vs Debug | MultiByte

See [build.md](build.md). Release Unicode builds are likely to mis-handle the narrow Chinese UI strings.

## Score sparkle vs food

On mushroom pickup the code calls `setScorePos(q->x, q->y)` with **pixel** coordinates, but `Scene::show` draws score pops at `score.x * WIDTH`. World 1 food `(448, 160)` therefore sparkles at tile (448, 160), far off-camera.

## Hero spawn `y = X0`

`Role::Role` sets `myHero.y = X0` and `X0` is `2*WIDTH` (64), not `Y0` (`3*HEIGHT`). `Y0` is unused. The hero starts at screen (64, 64) with `isFly = true` and falls onto the floor.
