# Known quirks

These are facts about **this** tree, written so the examples do not “fix” the game by pretending the API is cleaner than it is.

## `define.h` include guard

```cpp
#ifndef MYDEFINE
#define MYDIFINE
```

The guard is never defined, so the header is not actually include-guarded. `#pragma once` on line 1 saves you in MSVC. The portable sandbox copies the numeric values, not the header, to stay clear of the `max` macro and the trailing-semicolon macros.

## Trailing semicolons in macros

```cpp
#define F TIME*0.3;
#define LIFE 5;
```

`life = LIFE;` in `main.cpp` expands to `life = 5;;` and compiles. `if (life == LIFE)` would not. `F` is only used as a statement-like increment (`scenery_iframe += F`), so the extra semicolon is tolerated.

## Sticky keys

`Control::getKey` only refreshes the mask when `_kbhit()` is true. Releasing `D` does not clear `CMD_RIGHT`. The hero keeps the last command until another key event arrives. See [game-loop.md](game-loop.md).

## `MAP_NUMBER == 30` overflow

| World | Authored `Map` rows | Installed |
| --- | --- | --- |
| 1 | 32 | 30 (last two water tiles dropped) |
| 2 | 36 | 30 (grass, sign, four trees dropped) |
| 3 | 30 + an off-by-one read | 30 written, then one past `m[]` |

World 2’s goal sign is record 32, so the **sign is not in `map[]`**. Clearing still works because `isEnding` is a distance check, not a sprite overlap.

## Off-by-one copy loops

Coins, food, and enemies use:

```cpp
while (i <= sizeof(arr) / sizeof(arr[0]))
```

That iterates `N+1` times on an `N`-element literal. The extra step reads the next stack bytes. World 3’s map loop has the same `<=`. The toolkit exporter prints `for` loops with `i < n && i < CAP`.

## `Role` constructor spawn

```cpp
myHero.x = X0;
myHero.y = X0;   // X0 is 2*WIDTH == 64, not Y0 (3*HEIGHT == 96)
```

The hero starts at (64, 64), one tile higher than the `Y0` comment in `define.h`. `isFly` starts `true`, so the first frames are a drop onto the world 1 floor at y=9.

## World 3 `rand` without `srand`

`createMap` / `createCoin` call `random(a,b)` → `rand()%(b-a)+a` with no `srand`. Pipe layouts repeat for a given CRT. They are not stable across MSVC vs MinGW.

## Bullet range vs camera

`MAX_DISTANCE` is 480 **screen** pixels. After the camera rails, shots explode near the right edge and cannot snipe walkers far ahead. `bullteFlying` also hard-codes `hitMap(..., 1)` so the world-3 death rule does not apply to projectiles.

## `role.show` steps bullets

Simulation of in-flight shots lives in the draw function. Skipping `show()` (for example a headless test) freezes projectiles. The physics sandbox does not model bullets for that reason.

## Save read

读档 `fopen_s` + `fscanf_s` without a NULL check. A missing `gameRecord.dat` is not a handled error. Accepted worlds are only 1–3 (`flag >= 4` is rejected).

## Pause vs HUD labels

The pause sheet’s third button is drawn as **退出游戏** but returns `VIR_HOME` (start menu), not `exit`. Hover text is English (`The menu`). The fourth button saves and returns `VIR_RETURN`, not home.

## Friction sign hack

`k = Xabs/vX ± 3` is not `sign(vX)`. It produces `k = -2` when running right and `k = +4` when running left, so stopping is faster facing left. Documented in [physics.md](physics.md); reproduced in `examples/physics`.

## Header coupling

`inertia.h` includes `graphics.h` and `scene.h` even though `Inertia::move` is pure math. `inertia.cpp` starts with `#pragma once` (harmless, unusual). The portable header is `examples/physics/inertia_portable.h`.

## Character set

Debug builds are MultiByte; Release builds in the vcxproj are Unicode. EasyX text calls in `control.cpp` follow the Debug convention.

## What the examples will not do

The toolkit will **warn**, not rewrite `MaoLiAo/*.cpp`. This folder is documentation for a personal 2020 project. Behaviour-changing patches belong in a different change.
