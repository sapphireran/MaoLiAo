# Design notes and known quirks

Personal later-self list. None of these are company bugs. They are course-code fossils worth not “fixing” blindly if you still want the 2020 feel.

## Macros that include semicolons

```cpp
#define F TIME*0.3;
#define LIFE 5;
```

`int life = LIFE;` becomes `int life = 5;;` which happens to compile. `something = F` does not. `LIFE` cannot be used in expressions like `LIFE + 1` without a syntax error.

## Header guard typo

`#ifndef MYDEFINE` / `#define MYDIFINE`. `#pragma once` is the real guard.

## `Y0` is unused

Hero spawn y is `X0` (64), not `Y0` (96). The cat always drops in.

## Off-by-one copies

`createCoin`, `createFood`, `createEnemy`, and world-3 `createMap` use

```cpp
while (i <= sizeof(arr) / sizeof(arr[0]))
```

The `<=` reads one past the local array. It has “worked” because the extra slot is leftover stack / the destination is larger (`COINS_NUMBER` etc.). Do not copy this pattern. The portable pipe generator uses `<`.

World 2’s map loop is `while (id > 0 && id < 15 && i < MAP_NUMBER)` without a hard length on the local `m[]`. That is safer only because a zeroed `Map` would stop it — the last written entries are never zero.

## `hitMap` world argument is overloaded

Passing the real `world` enables “pipe contact kills” in world 3. Passing `1` disables it. Call sites disagree on purpose. If you add world 4, audit every `hitMap(..., 1)`.

## Sticky keys

`getKey` only refreshes on `_kbhit()`. Combined with `GetAsyncKeyState`, you can get a held-key bitset that never clears until another console key event. Esc therefore tends to reopen pause unless you click 返回游戏 and then tap a movement key.

## Pause label vs behavior

第三行写着「退出游戏」但 `VIR_HOME` 只回主菜单，并不 `exit`.

## Save MessageBox argument order

Win32 `MessageBox(hwnd, lpText, lpCaption, type)`. The call passes `"提醒"` as text and `"存档缺失，请检查存档"` as caption.

## Score sparkle vs food

`hitFood` calls `setScorePos(q->x, q->y)` using **pixel** coordinates, while coin sparkles use **tile** coordinates. The mushroom sparkle draws at `xMap + pixels*WIDTH`, which is off-screen. Harmless, just invisible.

## Bullet max distance is screen-space

`p->x > MAX_DISTANCE` uses the bullet’s screen x. Fine at the start of a level; after scrolling, bullets still expire near the right of the window.

## World 3 mushroom

`(10, 10)` px is not a tile on the path. Immunity is theoretically in the code and practically unused unless you jump into the top-left corner.

## `inertia.cpp` starts with `#pragma once`

The `.cpp` is not a header. Harmless.

## `timer.h` defines static members

Including `timer.h` from two translation units would duplicate `Timer::m_clk` / `m_oldclk` / `m_freq`. Only `main.cpp` includes it.

## `random` macro

```cpp
#define random(a,b) (rand()%(b-a)+a)
```

World 3 never seeds `srand`. Heights repeat per process. Also `rand()%(b-a)` is biased and `random(1,7)` yields `1..6`.

## Release Unicode vs Debug MultiByte

String literals in `control.cpp` are narrow `"黑体"` / `"返回游戏"`. A Unicode Release build wants `L"..."` or `_T`. Prefer Debug Win32 for the authentic build.

## Resource folder

`res\` is not in git. Keep a personal backup of the 2020 bitmaps and MP3s if you still want to launch the EasyX binary.

## Why examples exist

The interesting bits — integrator, friction `u`, four-corner hit, camera pin, save range, pipe pair math — are small and do not need EasyX. They are copied into `examples/common` so they can be regression-tested on a Linux box without the course toolchain.
