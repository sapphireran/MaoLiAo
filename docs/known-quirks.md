# Known quirks

These are observations of the 2020 source, left as-is. The portable examples **do not** copy the broken include guard or the `LIFE` semicolon; they do copy the off-by-one loop pattern where the game’s numbers depend on it, and then show the safer rewrite.

## `define.h` include guard

```
#ifndef MYDEFINE
#define MYDIFINE
```

The defined token is not the token being tested, so the guard never trips. `#pragma once` still protects a single TU.

## `LIFE` and `F` trailing semicolons

```
#define F TIME*0.3;
#define LIFE 5;
```

`int life = LIFE;` expands to `int life = 5;;` and happens to compile.
`life = LIFE;` in assignment form also works. Using `LIFE` inside a larger expression (`life + LIFE`) would break.

`scenery_iframe += F` expands to `scenery_iframe += TIME*0.3;;` — fine as a statement, unusable in an expression.

## `createEnemy` / `createCoin` / `createFood` / world-3 map copy

All use

```
while (i <= sizeof(arr) / sizeof(arr[0]))
```

`<=` runs **one extra** iteration and reads `arr[count]`. That is undefined behavior. In practice the extra `Enemy`/`POINT`/`Map` is leftover stack (often zeros), which looks like “a dummy slot”. Examples print both the unsafe count and the `i < n` count.

World 3’s `createMap` loop has the same `<=` on a `Map m[]` that already fills most of `MAP_NUMBER`.

## World 2 copy condition

```
while (m[i].id > 0 && m[i].id < 15 && i < MAP_NUMBER)
```

If the initializer is shorter than `MAP_NUMBER`, the loop depends on a **zeroed** sentinel after the last real entry. The local `m[]` has no guaranteed trailing `id == 0`. World 1 happens to stop because the last written id is 13 and the next stack value is often 0. This is another reason the portable map builder takes an explicit length.

## `bullteFlying` / `LEHGTH_INTERVAL_BULLET`

Original spellings, kept in notes so grep still works. Portable code uses `bulletFlying` and `LENGTH_INTERVAL_BULLET` and mentions the aliases.

## `showDied` life icons

Draws 1, 2, 3, or 4 sprites. After a death from 5 lives you have 4, so the first continue screen matches. There is no `life == 5` or `life == 0` drawing path.

## Pause “退出游戏” vs title “退出”

Pause 退出游戏 sets `VIR_HOME` (back to title, reset life/world). Title 退出 calls `exit(0)`. Hover English on pause 退出 is “The menu”.

## Save load `MessageBox` argument order

```
MessageBox(..., "提醒", "存档缺失，请检查存档", 1)
```

Win32 order is `(hwnd, text, caption, type)`, so the **caption** becomes 存档缺失… and the body is 提醒.

## `fopen` on 读档

No null check. A missing file can crash instead of showing the MessageBox. The error MessageBox path also skips `fclose`.

## `Role` MCI opens every construct

Death / next world / restart all `Role world` again and `open` the same aliases.

## World 3 `rand` never seeded

Pipe heights repeat each process. Call `srand` yourself if you restore the stage and want variety.

## Narrow pits are optional

`isHit` only tests four inset corners of a 32×32 sprite. A 32 px gap almost always still contains a corner. A 96 px (3-tile) gap — the water holes in world 1 — is a real hole if you stop, but at `V_MAX` you can often stride it before `y` leaves the far ledge’s snap range. The portable sim checks both outcomes.

## Horizontal wall test uses `world = 1`

Walking into a world-3 pipe does **not** go through the “tile kills you” branch of `hitMap` (that branch is on the vertical probe with the real world id). You still die if the vertical `y+1` or overlap test runs with `world == 3` on the same frame — usually it does, because you are overlapping the pipe.

## `CMD_DOWN` and `CMD_ESC` in `action`

Empty `if` blocks. Duck was never implemented. Esc is handled in `Control`, not `Role`.

## `max` / `random` macros

```
#define max(a,b) ((a) > (b) ? (a) : (b))
#define random(a,b) (rand()%(b-a)+a)
```

They collide with `<windows.h>` `max` if that header is included first, and `random` evaluates `a`/`b` twice. World 3 uses both.

## Unreachable `closegraph`

The `while (true)` never breaks. `mciSendString("close all")` and `closegraph()` after the loop are dead. Title 退出 uses `exit(0)` and skips them too.

## Double include of `inertia.h` in `role.cpp`

Harmless because of `#pragma once`.

---

When editing the **game** later, prefer fixing these in `MaoLiAo/` with a playtest on Windows. The examples stay defined-behavior C++ so they can run in CI or on this agent.
