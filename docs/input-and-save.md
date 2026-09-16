# Input and save

Keyboard state is a bitfield. Mouse is only used on the title screen and the pause overlay. There is no gamepad path.

## Command bits

Defined in `define.h`, filled in `Control::GetCommand` with `GetAsyncKeyState(...) & 0x8000`.

| Bit | Value | Key | Meaning |
| --- | --- | --- | --- |
| `CMD_LEFT` | 1 | `A` | Run left |
| `CMD_RIGHT` | 2 | `D` | Run right |
| `CMD_UP` | 4 | `W` or `K` | Jump / flap |
| `CMD_DOWN` | 8 | `S` | Read, unused in `Role::action` |
| `CMD_SHOOT` | 16 | `J` | Fire if `isShoot` |
| `CMD_ESC` | 32 | `Esc` | Open pause |

Several keys can be down at once; the bits OR together. `Role::action` therefore writes `if (KEY & CMD_LEFT)` rather than `switch`.

`getKey()` only refreshes that bitfield when `_kbhit()` is true. That is a **console** buffer check (`conio.h`). If the EasyX window has focus but the CRT console has not seen a key, `key` stays at the last stored value — including a held direction — until another console-visible key arrives. `GetAsyncKeyState` inside `GetCommand` is then sampled once. This is why a tap can feel sticky: the last bitmask is replayed on frames where `_kbhit()` is false. Releasing keys does not clear `key` unless a new `_kbhit` sample runs `GetCommand` and sees those bits off. In practice, any key event (including key-up that still trips `_kbhit` on some CRT builds, or a different key down) refreshes the mask.

Virtual bits are **not** produced by the keyboard:

| Bit | Value | Source |
| --- | --- | --- |
| `VIR_RETURN` | 64 | Pause → “返回游戏” |
| `VIR_RESTART` | 128 | Pause → “重新开始” |
| `VIR_HOME` | 256 | Pause → “退出游戏” (label says 主菜单 in comments) |

`main` compares with `==`, so a mixed mask would miss. The pause path assigns exactly one virtual value and returns.

`examples/src/command_bits.cpp` encodes and decodes these masks, including the virtual-only values.

## Title screen (`gameStart`)

`home.bmp` page 0 is the backdrop. Five stacked 90×30 buttons sit at `x ∈ [211, 301]`, `y` starting at `YSIZE/3 = 128`:

| Button | Chinese | Hover English | Action |
| --- | --- | --- | --- |
| 0 | 开始 | Start | Leave the loop; play begins |
| 1 | 介绍 | Introduction | Page of story text, 返回 in the corner |
| 2 | 指导 | Directions | Control list, 返回 |
| 3 | 退出 | Exit | `exit(0)` |
| 4 | 读档 | Read Load | Read `gameRecord.dat` into `world` |

Hover recolors a button (green on the menu, red on 返回) by sampling `getpixel` of a known corner. The intro page attributes the game to **PWB** and mentions the third world as a jump stage.

`world` is an `extern int` in `control.cpp` so the load path can write the global that `main` already allocated. Invalid values (`flag <= 0 || flag >= 4`) pop a `MessageBox` and stay on the title screen.

## Pause (`pauseClick`)

`Esc` sets `CMD_ESC`. `getKey` then calls `pauseClick`, which blocks on `GetMouseMsg` until a click:

| Row | Chinese | Hover English | Result |
| --- | --- | --- | --- |
| 0 | 返回游戏 | Return | `VIR_RETURN` (resume; `main` ignores this value) |
| 1 | 重新开始 | start again | `VIR_RESTART` — rebuild current world |
| 2 | 退出游戏 | The menu | `VIR_HOME` — title, `life = 5`, `world = 1` |
| 3 | 进行存档 | Write Data | write `world` to `gameRecord.dat`, then `VIR_RETURN` |

The fourth button is save, not a fourth navigation target. The comment above the click test still says “主菜单” from an earlier layout.

`FlushMouseMsgBuffer` runs before the loop so queued clicks from the playfield do not fire a menu item immediately.

## Save format

Path: `gameRecord.dat` in the process working directory (the project already contains a checked-in copy).

Contents: a single `fprintf_s(fp, "%d", world)` integer, no newline required. Load is `fscanf_s(fp, "%d", &flag)`.

That is the entire save: **which world to start**. Not stored:

- lives
- score
- `isShoot`
- hero position
- remaining coins / enemies

A load from world 2 or 3 still starts that world's constructor defaults (full coins, full enemy list, five lives because `main` does not change `life` on a successful load).

There is no checksum. A missing file on “读档” will fail `fopen_s`; the current code does not test the pointer before `fscanf_s`. A corrupt file that parses as `0` or `4+` is the only guarded case.

## HUD

`showScore` / `showLevel` use the “Cooper” face if it is installed, `setbkmode(TRANSPARENT)`, and `outtextxy`. Score is top-left; level is top-right at `x = XSIZE - 90`. They run inside the same `BeginBatchDraw` as the playfield so they composite on the current frame.

## What the portable examples cover

Bit packing and the save integer are OS-agnostic. The examples do not call `GetAsyncKeyState`. They treat a mask as data:

```
mask = CMD_LEFT | CMD_UP        → 5
has(mask, CMD_UP)               → true
is_virtual(VIR_RESTART)         → true
parse_save("2")                 → world 2
parse_save("9")                 → invalid
```
