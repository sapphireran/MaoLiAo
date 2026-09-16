# Known quirks in the 2020 source

Personal notes while writing the docs. Nothing here is a company
issue tracker. The examples **do not silently "fix"** these unless a
comment says so; the catalog program reports the ones that affect
data.

## Include guard typo — `define.h`

```cpp
#ifndef MYDEFINE
#define MYDIFINE
```

The guard never defines `MYDEFINE`, so a second include redefines
every macro. Today each translation unit includes `define.h` once
through the headers, so it does not explode. A future extra include
will spam "macro redefined" warnings and the `max` / `random` macros
can clash with `<windows.h>`.

## `LIFE` has a semicolon

```cpp
#define LIFE 5;
```

`life = LIFE;` becomes `life = 5;;`. Harmless. `life = LIFE - 1`
would become `life = 5; - 1` and fail to compile.

## Hero spawn uses `X0` for y

`Y0` is `3 * HEIGHT` (96) and is unused. `Role::Role` sets both
`myHero.x` and `myHero.y` to `X0` (64). The hero therefore drops from
tile row 2, not row 3.

## Map array overflow

`MAP_NUMBER` is 30. The copy loop stops at `i < MAP_NUMBER`.

| World | Authored records | Stored | Dropped (end of literal) |
| --- | --- | --- | --- |
| 1 | 32 | 30 | water at x=71 and x=75 |
| 2 | 36 | 30 | tuft, flag `{111,1,id 12}`, four trees |
| 3 | 30 + a `<=` extra write | 30, then one past-the-end write | see below |

World 2's flag sprite can go missing; `isEnding` is a distance check
(104 tiles), so the stage still completes.

`examples/level_catalog --warn` prints the truncation counts.

## Off-by-one copy loops

`createEnemy`, `createCoin`, `createFood`, and the world-3 map copy
use

```cpp
while (i <= sizeof(arr) / sizeof(arr[0]))
```

`<=` reads one past the initializer. On many compilers that extra
iteration writes a zeroed or garbage slot into the destination. Coins
and food tolerate a `{0,0}` extra because `show` skips `x == 0`.
Enemies with a leftover `turn != 0` can spawn a ghost crawler.

The catalog example uses `<` and prints a warning when you pass
`--legacy-leq` so you can see the difference.

## World 3 map loop vs `MAP_NUMBER`

The world-3 loop is `i <= sizeof(m)/sizeof(m[0])` with 30 records in
`m` (7 columns × 4 + runway + flag). That is 31 iterations into a
30-slot `map[]` — one write past the end of `Scene::map`.

## Sticky keyboard

See [controls-and-save.md](controls-and-save.md). `_kbhit` does not
clear bits on key-up.

## `F` macro

```cpp
#define F TIME*0.3;
```

Another trailing semicolon. `scenery_iframe += F` works
(`+= TIME*0.3;`). `x = F * 2` would not.

## `random` macro

```cpp
#define random(a,b) (rand()%(b-a)+a)
```

World 3 heights: `random(1, 7)` → `rand() % 6 + 1` → 1..6.
Coins: `random(3, 7)` → 3..6. No `srand` in `main`.

## Bullet interval after release

```cpp
shootButtonDown = false;
shootTimeInterval = 0.01;
```

The next press takes the "held" branch (`shootButtonDown` was false
so the first shot still fires). The `0.01` reset is leftover.

## `bullteFlying` typo

The method name is missing an `e`. Call sites match.

## Pause label vs behavior

"退出游戏" returns `VIR_HOME`, which is the home screen, not process
exit. Process exit is only the home-screen "退出" row (`exit(0)`).

## Unreachable shutdown

```cpp
while (true) { ... }
mciSendString("close all", NULL, 0, NULL);
closegraph();
```

The only clean process death is `exit(0)` from the menu.

## Enemy `hitMap` and `x0`

`hitEnemy` / enemy walk pass coordinates that already include
`hero.x0`, and `hitMap` subtracts `x0` again. AI therefore depends on
camera origin. Documented so a "cleanup" of the probe math is not
done blindly.

## `inertia.h` includes `graphics.h` and `scene.h`

`Inertia::move` needs neither. The portable header drops both.

## Release | Unicode

[build-windows.md](build-windows.md) — Chinese UI strings are not
wide. Stick to Debug MultiByte for the original exe.
