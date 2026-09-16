# Known issues (as shipped)

These are observations from reading the 2020 sources. The examples document them; they do not silently “fix” the game.

## Include guard typo

`define.h` opens with `#ifndef MYDEFINE` and defines `MYDIFINE`. The guard never matches on a second include, so the header is not idempotent. Harmless today because nothing includes it twice in a way that redefines macros with different values.

## `MAP_NUMBER = 30` overflow

World 1 authors 32 stamps, world 2 authors 36. The last water tiles (world 1) and several trees / stairs / the far goal neighborhood (world 2) are not copied into `map[]`. World 3 fits exactly.

`examples/level_dump` prints `authored` vs `stored` for each world.

## Off-by-one copies

`createCoin`, `createFood`, `createEnemy`, and world 3 `createMap` use

```cpp
while (i <= sizeof(arr) / sizeof(arr[0]))
```

The `<=` reads one past the initializer. That is undefined behavior. It often copies a zeroed extra slot (or stack garbage) into the next table entry. Prefer `<`.

## World 3 food units

Worlds 1–2 store food in pixels. World 3 stores `{10, 10}`. `hitFood` and `show` treat those as pixels, so the sprite sits at the top-left rather than tile (10, 10).

## World 3 touch-death vs horizontal bump

`hitMap(..., world)` applies the world-3 death rule. The horizontal wall test calls `hitMap(..., 1)` on purpose so bumping a pipe does not use that rule — then the vertical / overlap tests still can. The result is sensitive to order: you can cancel an x-step against a pipe and still die on the same frame from the overlap test.

## `rand` without `srand`

World 3 pipe heights call `rand()` and never seed it. MSVC’s default seed is 1, so one compiler gives a stable layout; another CRT will not.

## Unreachable shutdown

```cpp
while (true) { ... }
mciSendString("close all", ...);
closegraph();
```

The process only leaves through `exit(0)` on the title screen. MCI aliases and the EasyX device are not closed on that path either.

## `LIFE` and `F` macros end with semicolons

```cpp
#define F TIME*0.3;
#define LIFE 5;
```

`int life = LIFE;` expands to `int life = 5;`, which is fine. `life = LIFE + 1` would become `life = 5; + 1` and compile as a no-op plus a discarded expression. `scenery_iframe += F` happens to be a single statement.

## Pause “退出游戏” is Home, not process exit

The pause row labeled 退出游戏 returns `VIR_HOME` and rebuilds the title loop. Only the title row 退出 calls `exit(0)`.

## Save dialog argument order

`MessageBox(..., "提醒", "存档缺失，请检查存档", 1)` passes the short word as text and the sentence as the caption.

## Bullet simulation in `show`

`bullteFlying` (original spelling) runs inside the draw path. A frame that dies before `show` will not advance bullets; a tool that calls `show` twice would double-step them.

## Enemy table vs `MAP_NUMBER`

Enemies that walk off authored ground because a supporting tile was dropped by the 30-stamp cap will immediately reverse (no floor under the lead foot) and jitter on the last solid pixel.

## Character set mismatch

Debug is MultiByte, Release is Unicode. Menu strings are narrow literals. If Release menus look empty or mojibake, switch the Release character set or stick to Debug.

## Recursive `gameStart`

The 返回 button calls `Control::gameStart()` nested inside the current `gameStart`. It works because the inner call is the last thing that happens before the outer loop’s state is abandoned, but it grows the stack if someone clicked 返回 in a tight loop.

## Examples that stay honest

Portable code under `examples/lib` copies the formulas and the authored arrays. It uses `<` on copy loops and reports overflow instead of writing past `map[30]`. That is documentation, not a stealth gameplay patch.
