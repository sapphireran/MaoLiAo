# Controls, menus, and save files

Input is a bit mask, not a queue of key-down events. `Control::GetCommand`
ORs every key that `GetAsyncKeyState` reports as down (`0x8000`):

| Bit | Macro | Physical key |
| --- | --- | --- |
| 1 | `CMD_LEFT` | `A` |
| 2 | `CMD_RIGHT` | `D` |
| 4 | `CMD_UP` | `W` or `K` |
| 8 | `CMD_DOWN` | `S` (read, unused in `Role::action`) |
| 16 | `CMD_SHOOT` | `J` |
| 32 | `CMD_ESC` | `Esc` |

Pause-menu results overwrite `key` with a disjoint set:

| Value | Macro | Meaning |
| --- | --- | --- |
| 64 | `VIR_RETURN` | close pause, keep playing |
| 128 | `VIR_RESTART` | reconstruct Role + Scene for `world` |
| 256 | `VIR_HOME` | reset lives, `world = 1`, home screen |

`main.cpp` only special-cases `VIR_RESTART` and `VIR_HOME`. `VIR_RETURN`
falls through into `Role::action` as a mask with no movement bits.

## Sticky last key

`getKey` updates `key` only when `_kbhit()` is true. `_kbhit` is a
console "a new press is waiting" test. It does **not** fire on key-up.
Combined with `GetAsyncKeyState`, the observed behavior is:

1. Press `D`. `_kbhit` is true, `key` becomes `CMD_RIGHT`, hero runs.
2. Keep holding `D`. Some frames `_kbhit` is false; `key` stays
   `CMD_RIGHT`. Still fine.
3. Release `D`. `_kbhit` usually does **not** run. `key` stays
   `CMD_RIGHT` until another press (or Esc) refreshes the mask.

So the hero can slide after you let go, then the friction path in
`Role::action` is what actually stops `vX` — except friction only runs
when `vX * a <= 0`. If `key` still has `CMD_RIGHT`, `a` stays `A_ROLE`
and friction is skipped. That is a real 2020 control bug: a released
`D` can leave the hero at `V_MAX` until another key is hit.

`examples/command_bits_demo` shows the mask algebra only. It cannot
reproduce Win32 `_kbhit`; the sticky behavior is documented here so
you do not "fix" a demo that was never broken.

## Home screen (`Control::gameStart`)

`home.bmp` is 512 × 1920, five stacked 384-px pages. The home screen
blits the top page (`putimage(0, 0, &img_bg)`).

Five hit boxes, 90 × 30 px, centered at x = 256, starting at y = 128
(`YSIZE/3`):

| Row | Chinese label | Hover English | Action |
| --- | --- | --- | --- |
| 0 | 开始 | Start | leave the menu, start `world` |
| 1 | 介绍 | Introduction | overlay text, author credit "PWB" |
| 2 | 指导 | Directions | key legend |
| 3 | 退出 | Exit | `exit(0)` |
| 4 | 读档 | Read Load | read `gameRecord.dat` |

Introduction copy (as drawn):

```
这是一款横版过关
游戏。游戏主角叫
猫里奥，共有三关
第三关为跳跃关卡
游戏开发者：PWB
```

Operation copy:

```
向左移动：A键
向右移动：D键
开火射击：J键
跳跃：W键/K键
暂停键：Esc键
```

Hover recolors a row green and swaps in the English label. The
introduction / directions pages have a 44 × 24 "返回" button at the
bottom-right.

`gameStart` is recursive: hitting 返回 calls `gameStart()` again after
clearing flags. Deep menu toggling can grow the stack.

## Pause screen (`pauseClick`)

A green 90 × 120 panel at the same x center, y = `YSIZE/3`. Rows:

| Row | Idle label | Hover label | Result |
| --- | --- | --- | --- |
| 0 | 返回游戏 | Return | `VIR_RETURN` |
| 1 | 重新开始 | start again | `VIR_RESTART` |
| 2 | 退出游戏 | The menu | `VIR_HOME` (despite the Chinese "quit game") |
| 3 | 进行存档 | Write Data | write `world`, then `VIR_RETURN` |

Row 2's Chinese says "exit the game"; the code returns to the home
screen and does not call `exit`. Row 3 writes the save and unpauses.

Hover uses `getpixel` to decide whether a row must be redrawn idle.
That works only if nothing else paints those pixels.

## HUD

`showScore` / `showLevel` print `得分:  N` at (10, 10) and `关卡:  W`
at (422, 10) with the "Cooper" face. `showDied` blits the third slice
of `home.bmp` (`putimage(0, -2*YSIZE)`) and 1–4 copies of the death
frame from `role.bmp` to show remaining lives. Five lives is the
start; the first death therefore shows four icons.

`showGameOver` uses slice 1 (`y = -YSIZE`), sleeps 6.5 s.
`showPassed` uses slice 2 and prints `LEVEL:  W` for the **next**
world (because `main` increments `world` first).
`showPassedAll` uses slice 3, sleeps 7.8 s.

## Save format

Path: `gameRecord.dat` next to the working directory (the project
writes `"gameRecord.dat"`, not `"MaoLiAo\\gameRecord.dat"`).

Contents: a single `fprintf` of `%d` = `world`. No newline is
required; the checked-in file is the character `3`.

Load (home → 读档):

```cpp
fopen_s(&fp, "gameRecord.dat", "r");
fscanf_s(fp, "%d", &flag);
if (flag >= 4 || flag <= 0)
    MessageBox(..., "存档缺失，请检查存档", ...);
else
    world = flag;
```

There is no `fopen` failure check. A missing file will crash or
mis-read. `flag` is accepted for 1, 2, 3 only.

`examples/save_record_demo` reads and writes the same one-integer
format, including the out-of-range rejection rule.

## Music transport

`main` uses MCI string commands:

```
play music_bg repeat
play music_bg from 0          // after a reconstruct
stop music_bg                 // death, clear, home
play music_win from 0
play music_passedAll from 0
play music_end from 0
close all                     // unreachable; the loop never breaks
```

SFX aliases are fire-and-forget `play ... from 0` inside `Role`.
