# Quirks (kept as-is)

These are behaviors of the 2020 source, recorded so the Python lab can match
them instead of “fixing” them silently.

## `define.h` include guard does not guard

```c
#ifndef MYDEFINE
#define MYDIFINE
```

`MYDEFINE` is never defined. Double include would re-`#define` every macro
(legal) and re-define `max` / `random` (also legal as macros). The real
protection for `struct Map` is the separate `_MAP` fence duplicated in
`scene.h` and `role.h`.

## Trailing semicolons in macros

`#define LIFE 5;` and `#define F TIME*0.3;` expand to extra empty statements.
`int life = LIFE;` compiles as `int life = 5;;`.

## Spawn y uses `X0`

`Role::Role` sets `myHero.y = X0` (64), not `Y0` (96). The documented spawn
height is unused.

## Off-by-one copies

`createCoin`, `createFood`, `createEnemy`, and world 3 `createMap` loop with
`i <= sizeof(arr)/sizeof(arr[0])`. The last iteration reads one-past the
local array. In practice it often copies a zeroed `map[i]` / stack garbage
into the next slot. World 3's loop can also write `map[30]` (`MAP_NUMBER` is
30, valid indices 0–29).

## `MAP_NUMBER = 30` truncates authored maps

World 1 authors 32 tiles (drops two water sprites). World 2 authors 36
(drops the goal sign, a tuft, and four trees). World 3 comments out pipe
columns 8–10 to stay at 30.

## Sticky keys

`getKey` only calls `GetCommand` when `_kbhit()` is true, then returns the
cached `key` forever. There is no “release A” event. Easy to end up walking
after the key is up until another keystroke arrives.

## Pause “退出游戏” is home, home “退出” is process exit

The pause caption 退出游戏 sets `VIR_HOME` (back to `gameStart`). The home
caption 退出 calls `exit(0)`.

## Save file has no error path

Read assumes `fopen_s` succeeded. Write does not check the `FILE*`. Only a
single integer is stored. Values outside 1–3 pop a `MessageBox` whose title
and body arguments are swapped (`"提醒"` as the *message*, `"存档缺失…"` as
the *caption* in the `MessageBox(hwnd, lpText, lpCaption, type)` order).

## World 3 death uses `world` except when it does not

Pipe-touch death is inside `hitMap` when `world == 3`. Horizontal collision
in `action` passes literal `1`. Bullet and enemy AI probes also pass `1`.
Only the gravity / stand-on-tile probes pass the real world index.

## Empty entity slots cannot sit on the origin

Coins, food, bombs, and bullets treat `(0, 0)` as unused. World 3's weapon at
`(10, 10)` is a few pixels from that trap.

## `random` has no seed

`rand()%(b-a)+a` with default CRT seed. World 3 “random” pipes are
deterministic per runtime.

## Unreachable cleanup

`mciSendString("close all")` and `closegraph()` sit after `while (true)`.

## Header comment vs behavior

`control.h` says `showPassedAll` is “通关所有的动画”; `pauseClick`'s last
button comment still says “主菜单” in one branch and the code writes a save
file instead. Comments in `scene.h` (`SCORE_NUMBER // 不懂`) were left in
from the course.

The Python lab copies the numeric behavior (caps, friction formula, AABB,
save format) and calls out these quirks in docstrings rather than changing
the C++.
