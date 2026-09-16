# Quirks (as committed)

These are observations about **this** tree, not a patch list. Game sources were left alone so the homework binary stays bit-identical. The portable lab uses `< count` loops on purpose.

## Include guard never closes

`define.h` starts with `#pragma once` **and**

```cpp
#ifndef MYDEFINE
#define MYDIFINE
```

The defined token is misspelled, so the `#ifndef` is useless. `#pragma once` still saves MSVC from a double include.

## `LIFE` and `F` macros end with `;`

```cpp
#define F TIME*0.3;
#define LIFE 5;
```

`int life = LIFE;` expands to `int life = 5;` and works. `scenery_iframe += F;` becomes `scenery_iframe += TIME*0.3;;`. Do not write `if (life == LIFE)` — that would inject a stray semicolon into the condition.

## Hero spawn uses `X0` for **both** axes

`define.h` documents `Y0 = 3*HEIGHT` (96). `Role::Role` does

```cpp
myHero.x = X0;   // 64
myHero.y = X0;   // 64, not Y0
```

The cat starts on tile row 2, not row 3.

## `MAP_NUMBER = 30` clips authored maps

| World | Authored tiles | Stored | What disappears |
|---|---|---|---|
| 1 | 32 | 30 | water at x=71 and x=75 |
| 2 | 36 | 30 | grass tuft, **goal sign at (111,1)**, four trees |
| 3 | 30 | 30 | none (but see the overflow below) |

World 2’s flag never blits. The level still ends on `distance > 104*32`. `createMap` even comments `//???存在问题` on the copy loop.

`examples/src/world_atlas.cpp` asserts `authored_w1 == 32`, `stored_w1 == 30`, `authored_w2 == 36`, `stored_w2 == 30`.

## `<= sizeof` off-by-one

`createEnemy`, `createCoin`, `createFood`, and world 3 `createMap` all walk

```cpp
while (i <= sizeof(arr) / sizeof(arr[0]))
```

The last iteration reads one past the local array (UB) and writes one past the destination when the destination is exactly full.

World 3 is the loud case: 30 local `Map`s, 30 `map[]` slots, 31 assignments → `map[30] = m[30]`.

The lab’s `pipe_field` demo shows a **safe** 30-tile fill with `< count`.

## World-3 `random` and `<stdlib.h>`

`#define random(a,b) (rand()%(b-a)+a)` lives in `define.h`. `scene.cpp` includes `math.h` but not `stdlib.h`. MSVC’s `<math.h>` often pulls `rand`; a stricter toolchain would fail to compile world 3.

`rand` is never `srand`’d, so the pipe field is whatever the CRT default seed is (usually 1).

## Sticky keys

`getKey` only samples `GetAsyncKeyState` when `_kbhit()` is true. A physical key can stay “down” in `Control::key` until another console character arrives. Combined with `CMD_ESC` latching, Esc can reopen pause immediately after resume.

## Pause “退出游戏” is not process exit

The button label is 退出游戏; `main` handles `VIR_HOME` as “reset to world 1 and show the home menu”. Process exit is only home → 退出 (`exit(0)`).

## `MessageBox` argument swap

On a bad save file:

```cpp
MessageBox(GetForegroundWindow(), "提醒", "存档缺失，请检查存档", 1);
```

Win32 order is `(hwnd, text, caption, type)`. The caption becomes 提醒 and the body is the short word. `fopen` failure is not checked; `fclose` is skipped on the error path.

## Integer division on world-2 food

```cpp
{ 39 * WIDTH - WIDTH / 3, 3 * HEIGHT + HEIGHT / 5 }
```

`32/3 = 10`, `32/5 = 6` → `(1238, 102)`, not a tile center.

## Pipe draw vs hit

Id 7 is blitted 64 px wide and 32 px tall, but `hitMap` gives it a 32×32 box (unless it is id 8/10). The visible shaft overhangs the collider.

## `hitMap(..., world=1)` from bullets and wall snap

Horizontal bump resolution and `bullteFlying` pass `world = 1`, so world-3 instadeath does not run on those probes. A bullet can strike a pipe without killing the hero.

## `inertia.cpp` starts with `#pragma once`

A `.cpp` does not need it. Harmless.

## Unreachable shutdown

`mciSendString("close all", ...)` and `closegraph()` sit after `while (true)`. The process only leaves through `exit(0)` on the home menu.

## `max` / `random` macros vs Windows headers

`#define max(a,b) ...` will collide with `<windows.h>` `max` if that header is included without `NOMINMAX`. EasyX’s `graphics.h` is what the homework actually includes.

## `gameRecord.dat` is committed

The file holds `3`. A player who never opens 读档 still has a leftover clear-state sitting in git.
