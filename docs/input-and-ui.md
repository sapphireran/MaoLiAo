# Input and UI

## Keyboard

`Control::GetCommand` is edge-less: it samples `GetAsyncKeyState` and OR-s bits. `getKey` only refreshes that mask when `_kbhit()` is true, so a key that is still down but no longer in the console input buffer can **stick** until another console key arrives. Esc is handled whenever the sticky mask still has `CMD_ESC`.

| Bit | Key | Gameplay |
| --- | --- | --- |
| `CMD_LEFT` | A | `a -= 20`, face left |
| `CMD_RIGHT` | D | `a += 20`, face right |
| `CMD_UP` | W or K | jump / flap |
| `CMD_DOWN` | S | empty branch |
| `CMD_SHOOT` | J | spawn bullet if `isShoot` |
| `CMD_ESC` | Esc | open pause |

Opposite directions cancel in `direction.x` (`+= 1` and `-= 1`) but both accels still apply, so A+D nets `a = 0` with `direction.x = 0`.

## Pause menu (`pauseClick`)

A 90×120 green panel centered at (`XSIZE/2`, `YSIZE/3`):

| Row (30 px) | Idle label | Hover label | Result |
| --- | --- | --- | --- |
| 0 | 返回游戏 | Return | `VIR_RETURN` |
| 1 | 重新开始 | start again | `VIR_RESTART` |
| 2 | 退出游戏 | The menu | `VIR_HOME` (title, not process exit) |
| 3 | 进行存档 | Write Data | write `world` to `gameRecord.dat`, then `VIR_RETURN` |

Hover is implemented by `GetMouseMsg` + `getpixel` to see if a cell is already blue. `FlushMouseMsgBuffer` runs once before the loop because the 10 ms game delay would otherwise deliver stale clicks.

## Title (`gameStart`)

Five 90×30 cells on a gray (`0x666666`) panel:

| Cell | Idle | Hover | Action |
| --- | --- | --- | --- |
| 0 | 开始 | Start | leave the menu, start play |
| 1 | 介绍 | Introduction | overlay 游戏介绍 |
| 2 | 指导 | Directions | overlay 操作指南 |
| 3 | 退出 | Exit | `exit(0)` |
| 4 | 读档 | Read Load | parse `gameRecord.dat` into `world` |

介绍 text (as drawn):

- 横版过关, hero 猫里奥, three stages, third is a jump stage, 开发者：PWB

指导 text:

- A/D move, J shoot, W/K jump, Esc pause

A 返回 button at the bottom-right of the overlays calls `gameStart()` recursively after clearing flags. Deep 介绍/指导 clicks therefore grow the C stack; a single back is fine.

读档: `fscanf` one int. If `flag` is not in `{1,2,3}`, a `MessageBox` is shown. The caption/text arguments look swapped (`"提醒"` as the body, `"存档缺失，请检查存档"` as the caption). On success `world = flag` and the menu exits. `fopen` is not null-checked; a missing file is undefined.

## HUD

`showScore` / `showLevel` use font `"Cooper"`, transparent background:

- `得分:  N` at (10, 10)
- `关卡:  N` at (422, 10)

## Interstitials

`home.bmp` is loaded at 512 × 1920 (five stacked 384-high frames). Vertical offsets pick a frame:

| Function | `putimage` Y | Sleep |
| --- | --- | --- |
| title (`img_bg` at 0) | 0 | modal |
| `showGameOver` | `-YSIZE` | 6500 ms |
| `showPassed` | `-2*YSIZE` | 2000 ms + “LEVEL: n” |
| `showDied` | `-2*YSIZE` | 2000 ms + remaining-life faces |
| `showPassedAll` | `-3*YSIZE` | 7800 ms |

`showPassed` also `cleardevice()` first. `showDied` composites `role.bmp` cell `(2*WIDTH, 0)` with `SRCPAINT` for 1–4 lives.

## Command-bit example

`examples/demos/command_demo` packs/unpacks the same masks and prints sticky-key caveats.
