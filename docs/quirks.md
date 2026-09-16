# Quirks that still ship in V2.0

These are observations from the current sources, not a patch list.
The examples reproduce several of them so a future rewrite can see the
behaviour without booting Visual Studio.

## Include guard

```c
#ifndef MYDEFINE
#define MYDIFINE
```

The guard never matches the define. `define.h` still works because of
`#pragma once`.

## `LIFE` semicolon

`#define LIFE 5;` makes `int life = LIFE;` compile as `int life = 5;`.
Do not write `LIFE + 1`.

## `Y0` is unused

`define.h` sets `Y0` to `3 * HEIGHT` (96). `Role::Role` sets
`myHero.y = X0` (64). The hero therefore **spawns 32 px higher** than the
comment in `define.h` describes, already airborne (`isFly = true`).

## `MAP_NUMBER = 30` truncates authored maps

| World | Authored tiles | Loaded | Dropped |
| --- | --- | --- | --- |
| 1 | 32 | 30 | water at x=71 and x=75 |
| 2 | 36 | 30 | tuft (42,8), **goal (111,1)**, four trees |
| 3 | 30 | 30 + one OOB write | loop is `i <= count` |

World 2 still “clears” because `isEnding` is a distance check, not a
collision with id 12.

## Off-by-one copies

`createCoin`, `createFood`, `createEnemy`, and world 3 `createMap` all use

```
while (i <= sizeof(arr) / sizeof(arr[0]))
```

That reads one element past the local array. World 3 also writes
`map[30]` when `map` only has indices `0..29`.

## Idle `k = 0/0`

See [physics.md](physics.md). Grounded idle computes `Xabs / vX` with `vX == 0`.

## Pause label vs action

The third pause row is drawn as “退出游戏” but returns `VIR_HOME`
(title + reset). Process exit is only the title-screen 退出 button.

## Swapped MessageBox strings

`MessageBox(..., "提醒", "存档缺失，请检查存档", 1)` puts the short word in
the body and the sentence in the caption. `1` is `MB_OKCANCEL`.

## World 3 flower is in pixel space

Worlds 1–2 store the flower in pixels on purpose (`14 * WIDTH`, …).
World 3 stores `{10, 10}`, which is 10 px from the origin, not tile (10, 10).

## World 3 kill vs clouds

Pipes kill on touch. Clouds (id 2) do not. The 25-wide cloud at x=80 is the
safe runway. `hitMap` is called with `world` for the hero and hard-coded
`world=1` for bullets and enemy probes, so enemies never take the
touch-death rule.

## Bullet range is screen-x

`p->x > MAX_DISTANCE` (480) uses the sprite’s **window** x. A shot fired
while the hero is pinned at 192 dies after 288 px of flight, not 480 world px.

## `getKey` latch + `_kbhit`

Movement bits update only when the console reports a key event. EasyX’s
window can eat those. In practice the game still feels all right because
`GetAsyncKeyState` is sampled whenever `_kbhit` is true, but it is not a
pure async poll.

## Timer statics in a header

`timer.h` defines `Timer::m_clk` and `Sleep` in the header. The project
compiles because `timer.h` is included only from `main.cpp`. Include it
from a second TU and you get duplicate symbols.

## Unreachable shutdown

`close all` / `closegraph()` sit after an infinite loop.

## World 3 `rand` without `srand`

Pipe heights and coin rows depend on the CRT’s default seed. The catalog
uses seed **2020** for a stable personal example; it will not match an
arbitrary MSVC run.

## `inertia.cpp` starts with `#pragma once`

Harmless on a `.cpp`. The real include guard is `inertia.h`.
