# Known issues (as of the v2.0 tree)

These are observations from reading the current sources. They are recorded
so the portable examples can avoid copying the footguns, and so a later
personal pass has a punch list. Nothing here is a company bug tracker.

## 1. Include guard never closes

`define.h`:

```
#ifndef MYDEFINE
#define MYDIFINE
```

The guard macro is misspelled, so the `#ifndef` is never satisfied by the
`#define`. The file relies on `#pragma once` at the top. If a compiler
ignores that pragma, every include of `define.h` redefines the same macros.

## 2. `LIFE` swallows a semicolon

```
#define LIFE 5;
```

Harmless for `int life = LIFE;` (becomes `int life = 5;;`). Harmful inside
expressions (`LIFE * 2` becomes `5; * 2`). The examples header uses a real
constant.

## 3. Off-by-one `sizeof` loops

Several builders walk `i <= sizeof(arr) / sizeof(arr[0])`:

- `Scene::createCoin` (all three worlds)
- `Scene::createFood` (all three worlds)
- `Role::createEnemy` (all three worlds)
- `Scene::createMap` world 3 (`i <= sizeof(m)/sizeof(m[0])`)

The last iteration reads one past the local array. World 3 then also writes
`map[i]` for that extra `i`, which is one past `m` and can overwrite the
next stack object. World 1/2 map loops stop on `id > 0 && id < 15` instead,
which is safer if the last authored record is a real tile.

The portable pipe builder uses `i < n`.

## 4. `MAP_NUMBER` is tight for world 2

World 2 writes 29 map records into a 30-slot array. World 3 writes
7 columns × 4 pipe pieces + 2 extras = 30, then the extra loop step
mentioned above.

## 5. Sticky keyboard mask

`getKey` only calls `GetCommand` when `_kbhit()` is true. The previous bit
mask is otherwise returned unchanged, so walk / jump / shoot can repeat
after the physical key is up. EasyX / conio makes this easy to miss in
playtesting because almost any extra keypress refreshes the mask.

## 6. World 3 flower is in the corner

`createFood` world 3 stores `{10, 10}`. Food coordinates are pixels, so the
pickup is not on tile (10, 10). Combined with death-on-touch, the
“invincible after flower” comment is hard to reach unless you flap into
the top-left.

## 7. Hero spawn uses `X0` for y

`define.h` publishes `Y0 = 3*HEIGHT` (96). `Role::Role` sets
`myHero.y = X0` (64). The comment says “初始化主角数据” and copies x into y.

## 8. Save load is unchecked

`fopen_s` + `fscanf_s` without a NULL check. Missing `gameRecord.dat` is a
crash, not the MessageBox. The MessageBox argument order is also reversed
relative to the usual `(hwnd, text, caption, type)` reading of the Chinese
strings.

## 9. Recursing `gameStart`

The title “返回” button calls `gameStart()` again instead of looping. A
player who opens 介绍 / 指导 many times deepens the stack. Each call also
re-enters the mouse loop with a fresh local `MOUSEMSG`.

## 10. Duplicate `struct Map`

`role.h` and `scene.h` both define `struct Map` under `#ifndef _MAP`. That
works, but the two copies can drift. `inertia.h` includes `scene.h` only
for the type, and `inertia.cpp` does not use `Map` at all.

## 11. `bullteFlying` typo and `MAX_DISTANCE`

The 480 px cap is in **screen** space (`p->x`), so a shot fired while the
camera has scrolled still dies at window x = 480, not at world x = 480.
Shots also cannot travel into the last 32 px of the 512 px window.

## 12. World 3 map loop uses `<=` and no `id` stop

Unlike worlds 1 and 2, world 3 does not stop on `id == 0`. Combined with
the `<=` bound, a garbage record can land in `map[]` and either draw junk
or become a phantom solid if its `id` happens to be 1–10.

## 13. `random` without `srand`

Pipe heights and world 3 coins are deterministic per process unless
something else seeds `rand`. That is convenient for the portable tests
(they seed explicitly) and surprising in play if you expected a new
gauntlet every launch.

## 14. Unreachable shutdown

```
while (true) { ... }
mciSendString("close all", ...);
closegraph();
```

Title **退出** uses `exit(0)`. There is no gameplay path that breaks the
loop, so the cleanup lines are dead in the current binary.

## 15. `F` macro includes a semicolon

```
#define F TIME*0.3;
```

`scenery_iframe += F;` expands to `scenery_iframe += TIME*0.3;;` and
happens to compile. `iframe += F * 2` would not.

## What the examples do instead

| Issue | Portable stand-in |
| --- | --- |
| `LIFE` / `F` macros | `constexpr` values, no trailing `;` |
| `sizeof` `<=` loops | `size_t i = 0; i < n; ++i` |
| Unguarded save | `read_world` returns `optional<int>` |
| Unseeded `random` | caller passes an `Engine` |
| Sticky keys | examples treat a mask as “held this tick only” |
| Duplicate `Map` | one `struct Tile` in `tiles.hpp` |
