# Quirks (as the source is)

These are observations from the current tree, useful when you edit levels or port the loop. They are not a rewrite plan.

## Include guard never closes

`define.h`:

```c
#ifndef MYDEFINE
#define MYDIFINE
```

The guard name and the define do not match, so a second include still parses the macros. Harmless with `#pragma once` at the top.

## `LIFE` macro includes a semicolon

```c
#define LIFE 5;
int life = LIFE;   // int life = 5;
```

Works for that one assignment. `life == LIFE` would not compile. Do not use `LIFE` in expressions.

## Hero spawn uses `X0` for y

`Role` constructor:

```c
myHero.x = X0;   // 64
myHero.y = X0;   // 64, not Y0 (96)
```

The first frames are airborne until a tile catch at `y+1`.

## Off-by-one copies

`createEnemy`, `createCoin`, `createFood` use:

```c
while (i <= sizeof(arr) / sizeof(arr[0]))
```

That reads one element past the local array. World 3 `createMap` uses the same `<=` on `m[]`.

## `MAP_NUMBER` is 30

World 1 authors 32 map records; world 2 authors 36. Only the first 30 are stored. World 1 loses two water sprites; world 2 loses the goal-adjacent trees (and may lose the goal sign itself — check `examples/map_layout` output: the sign is record 32, so it is **not** copied). Finish still works because `isEnding` is distance-based, not “touch id 12”.

## Pause “退出游戏” is home

The third pause row sets `VIR_HOME`. `main` returns to `gameStart()` and resets lives and world. Process exit is only the title 退出 button (`exit(0)`).

## Save `MessageBox` argument order

```c
MessageBox(GetForegroundWindow(), "提醒", "存档缺失，请检查存档", 1);
```

Win32 is `(hwnd, text, caption, type)`. The reminder string is the **caption**; 存档缺失 is not the body. `fopen_s` is also unchecked.

## `inertia.cpp` starts with `#pragma once`

No effect on a translation unit that is not included. The function itself is fine.

## World 3 food is in pixels

Worlds 1–2 store food in pixel coordinates already (world 1: `14*WIDTH`). World 3 uses `{10, 10}`, which is a 10×10 pixel corner, not tile (10, 10).

## Release is Unicode

See [build.md](build.md). Debug MultiByte is the configuration that matches the string literals.

## Timer first-call skip

`Timer::Sleep` on the first call sets `m_oldclk` and still waits. Later calls schedule from the previous deadline (good for a stable 10 ms). Overlay `Sleep(6500)` in `Control` is the CRT sleep, not this timer.

## Bullet explode at `x > 480`

That is screen space, so shots cannot travel a full world. Combined with `hitMap(..., 1)` they also explode on any solid, including clouds.

## No `srand`

World 3 `random(a,b)` is `rand()%(b-a)+a`. Without a seed, pipe layouts are whatever the CRT default stream is on that machine.

## Unreachable shutdown

`close all` / `closegraph` sit after `while (true)`. Title 退出 uses `exit(0)` and skips them.
