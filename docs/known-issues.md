# Known issues (original 2020 code)

These are observations from reading `MaoLiAo/`, recorded so a later
personal pass can fix them without rediscovering each one. The
portable examples **document** the bugs; they do not silently "fix"
behavior that the game still has.

## Off-by-one copies

Several builders use `<= sizeof(arr)/sizeof(arr[0])`:

| Function | Effect |
| --- | --- |
| `Scene::createCoin` (all worlds) | Writes one extra `coins[i]` from past the local `POINT` array |
| `Scene::createFood` (all worlds) | Same for `food[]` |
| `Role::createEnemy` (all worlds) | Same for `myEnemy[]` |
| `Scene::createMap` world 3 | `while (i <= count)` also assigns `map[i] = m[i]` one past `m` |

World 1/2 map copies use `i < MAP_NUMBER` (30) **and** stop on `id`,
so they do not over-read, but they **drop** authored tiles past index 29.

World 1 authored length is 32. World 2 is 36. Tails (mostly scenery
and extra trees) never appear in `map[]`.

`examples/04_level_preview` prints `authored_count` vs `loaded_count`.

## Guard / macro typos

```c
#ifndef MYDEFINE
#define MYDIFINE          // define.h — include guard never matches
```

`LIFE` and `F` have trailing semicolons:

```c
#define F TIME*0.3;
#define LIFE 5;
```

`int life = LIFE;` still compiles (`int life = 5;;`). Using `LIFE` in
an expression like `foo(LIFE)` would break.

`LEHGTH_INTERVAL_BULLET` and `bullteFlying` are misspelled throughout.

## Hero spawn Y

`Role::Role` sets `myHero.y = X0` (64), not `Y0` (96). The hero starts
in the air (`isFly = true`) and falls onto world 1's y=9 grass.

## World 3 random + cap

`random(a,b)` is `rand()%(b-a)+a` and `rand` is never seeded in
`main`, so pipe heights are the same every launch until something else
calls `srand`.

The world 3 copy can write past `map[29]` because the loop is not
capped by `MAP_NUMBER`. That is a real buffer risk if the initializer
grows.

## Collision / combat quirks

- `hitMap(..., world)` for bullets always passes `world = 1`, so the
  "touch tile = death" rule never applies to bullets.
- Horizontal wall tests also pass `world = 1`.
- `MAX_DISTANCE` is compared to the **window** X of the bullet, not
  distance travelled, so shots spawned near the right clamp explode
  immediately.
- `showDied` draws 1–4 cats but the first death (life goes 5 → 4) still
  calls it with `life == 4`. After four deaths `life == 1` shows one
  sprite; the fifth death is Game Over instead.
- Food in world 3 is at pixel `(10, 10)`, easy to miss.
- Pause label "退出游戏" is wired to `VIR_HOME` (title), not process
  exit. Title "退出" *does* `exit(0)`.

## Input

`getKey` only samples `GetAsyncKeyState` when `_kbhit()` is true.
A key held without a new console event can stick on the previous mask,
or drop if the last event was a key-up that still left `_kbhit` false.

`CMD_DOWN` and the `CMD_ESC` branch inside `action` are empty.

## Unreachable shutdown

`mciSendString("close all")` and `closegraph()` sit after `while (true)`.

## Character set

The project uses MultiByte in Debug and Unicode in Release. EasyX
text (`"黑体"`, `"得分:  "`) historically broke on one of those
configs (`a33467f` — "修复easyX字符集识别失败的bug"). Build Debug
unless you re-test Release fonts.

## Include style

`inertia.cpp` starts with `#pragma once` (harmless) and includes
`inertia.h` twice from `role.cpp`. `inertia.h` pulls `<graphics.h>`
even though the integrator does not use it — that is why the portable
example header has no EasyX dependency.
