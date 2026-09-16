# Known issues (as shipped)

These are notes on the personal V2.0 tree. The portable examples avoid
the broken macros and the off-by-one copies; they do not patch the
EasyX sources unless a later change says so.

## Guard typo in `define.h`

```cpp
#ifndef MYDEFINE
#define MYDIFINE
```

The include guard never defines `MYDEFINE`, so a second include would
redefine every macro. Today each translation unit includes the header
once, so the game still builds.

## Trailing semicolons in macros

```cpp
#define F TIME*0.3;
#define LIFE 5;
```

`int life = LIFE;` expands to `int life = 5;;` and compiles.
`scenery_iframe += F;` expands to `scenery_iframe += TIME*0.3;;` and
also compiles. Any use like `x = LIFE * 2` would not.

Portable headers spell `kLife = 5` and `kSceneryRate = TIME * 0.3`
without semicolons.

## Off-by-one copy loops

Several creators use `<= sizeof(arr)/sizeof(arr[0])`:

- `Scene::createCoin` (all worlds)
- `Scene::createFood` (all worlds)
- `Scene::createMap` world 3
- `Role::createEnemy` (all worlds)

The last iteration reads one past the stack array. Depending on the
compiler and what sits next on the stack, that can write a garbage
tile/enemy/coin into the last slot (or crash). World 3's map loop
also does not stop on `id == 0`.

`examples/05_map_layout` copies with a strict `< count` loop.

## `MAP_NUMBER` (30) drops authored tiles

World 1's `Map m[]` has **32** records. The copy stops at 30, so the
last two water decorations `(71,10)` and `(75,10)` never enter
`Scene::map`.

World 2's list has **36** records. Slots 31+ include the grass tuft
`(42,8)`, the **goal sign `(111,1)`**, and four trees. Those never
copy. The level can still clear because `isEnding` is a distance
check, not a sign-touch check.

`examples/05_map_layout` prints both the authored count and the first
30 copied ids so the cap stays visible.

## World 3 death vs camera `hitMap`

Death-on-pipe uses `hitMap(..., world)` with the real world index.
The horizontal wall test later calls `hitMap(..., 1)` on purpose so
a shove into a pipe does not instantly kill *and* so landing logic
can share code. Air-body overlap in world 3 still kills (unless
`isShoot`) because the jump/land probe passes `world` through.

## Bullet / camera mismatch

Bullets live in screen space; enemies and tiles live in world space
with `± x0` applied inside the hit helpers. After a long scroll,
`hitEnemy(bullet.x, bullet.y, ...)` subtracts `x0` from the *bullet*
as if it were a screen-space hero, which can desync late-game shots.
`MAX_DISTANCE = 480` also expires bullets near the right of a 512 px
window, so the desync is easy to miss.

## Save load without `fopen` check

Title **读档** `fscanf`s even if `fopen_s` failed. Expect a dialog or
an uninitialized `flag` on a missing file.

## Unreachable shutdown

```cpp
while (true) { ... }
mciSendString("close all", ...);
closegraph();
```

Only `exit(0)` from the title menu or killing the process ends the
program. MCI aliases leak for the process lifetime (acceptable for a
game exe).

## `Timer` statics in a header

`timer.h` defines `Timer::m_clk` and `Sleep` in the header. The
project compiles `timer.h` from `main.cpp` only, so there is no
multiple-definition link error. Adding `#include "timer.h"` to a
second `.cpp` would break the link.

## `random` without seed

`#define random(a,b) (rand()%(b-a)+a)` and no `srand`. World 3 pipe
heights are deterministic per CRT.

## `max` macro vs Windows

`define.h` defines `max(a,b)`. Windows headers sometimes define the
same name. World 3 is the only caller.

## Pause "存档" does not record lives or score

Only `world` is written. Loading a late world still starts with five
lives and zero score, hero at the spawn of that world.

## `showDied` icon count

After `life--`, icons are drawn for `life` in `{1,2,3,4}`. There is
no branch for 5 (never happens post-decrement) or 0 (game over path).
Starting `LIFE` 5 → first death shows 4 cats.

## Recursing `gameStart`

Returning from Introduction / Help calls `gameStart()` again instead
of flipping flags. Stack depth equals "how many times the player
opened help." Fine for a person, not for a fuzzer.

## Spelling in identifiers

`bullteFlying`, `MYDIFINE`, `LEHGTH_INTERVAL_BULLET`, `ENEMY_TOTE`.
Docs and examples keep the original names when wrapping those
behaviors so grep still works.
