# Controls

## Bit flags (`define.h`)

Input is a packed integer. `GetCommand` OR-combines whatever `GetAsyncKeyState` reports **this call**; it does not edge-detect.

| Macro | Value | Key(s) | Gameplay |
| --- | --- | --- | --- |
| `CMD_LEFT` | 1 | `A` | `a -= A_ROLE` |
| `CMD_RIGHT` | 2 | `D` | `a += A_ROLE` |
| `CMD_UP` | 4 | `W` or `K` | jump / world-3 flap |
| `CMD_DOWN` | 8 | `S` | empty branch |
| `CMD_SHOOT` | 16 | `J` | fire if `isShoot` |
| `CMD_ESC` | 32 | `Esc` | pause overlay |
| `VIR_RETURN` | 64 | pause “返回游戏” | resume |
| `VIR_RESTART` | 128 | pause “重新开始” | rebuild world |
| `VIR_HOME` | 256 | pause “退出游戏” | title (does not `exit`) |

`main` only special-cases `VIR_RESTART` and `VIR_HOME`. `VIR_RETURN` is just another bitmask; `action` ignores those high bits.

Because flags are bits, `A+D` sets `a = 0` but both `direction.x` increments apply (`−1` then `+1` → 0). `W+J` is a legal chord.

Sticky keys: `Control::getKey` updates `key` only when `_kbhit()` is true. EasyX / conio may not `_kbhit` on every frame for `GetAsyncKeyState` keys, so the last bitmask is reused. That is why a direction can persist for a few frames after release. Esc is tested on that latched value (`key & CMD_ESC`), so once Esc has been seen the pause menu opens every subsequent `getKey` until `pauseClick` replaces `key` with a `VIR_*` constant.

## Title screen (`gameStart`)

Blocking `GetMouseMsg` loop. Hit boxes are 90×30 px centred at `XSIZE/2`, stacked from `YSIZE/3`:

| Index | Chinese | Hover English | Action |
| --- | --- | --- | --- |
| 0 | 开始 | Start | return to `main` |
| 1 | 介绍 | Introduction | overlay text (three worlds, Flappy third, credit PWB) |
| 2 | 指导 | Directions | key list |
| 3 | 退出 | Exit | `exit(0)` |
| 4 | 读档 | Read Load | `fscanf` world from `gameRecord.dat` |

Intro/guide “返回” is the bottom-right 44×24 box. Nested `gameStart()` on return (recursion) rather than a state enum.

Load validation: `flag >= 4 || flag <= 0` shows a `MessageBox`. The caption/text arguments look swapped (`"提醒"` as the body, `"存档缺失，请检查存档"` as the caption). A missing file is not checked before `fscanf`.

## Pause overlay (`pauseClick`)

Green panel `90 × 120` at `(XSIZE/2 − 45, YSIZE/3)`. Four rows:

| Row | Chinese | Hover | Result |
| --- | --- | --- | --- |
| 0 | 返回游戏 | Return | `VIR_RETURN` |
| 1 | 重新开始 | start again | `VIR_RESTART` |
| 2 | 退出游戏 | The menu | `VIR_HOME` (title, not process exit) |
| 3 | 进行存档 | Write Data | `fprintf` `"%d"` of `world`; then `VIR_RETURN` |

Save does not record lives, score, or mushroom state — only the world index.

## HUD

- Score: `得分:  N` at `(10, 10)`, font Cooper
- Level: `关卡:  N` at `(XSIZE − 90, 10)`

Death interstitial (`showDied`) blits 1–4 hero faces from `role.bmp` depending on **remaining** lives after the decrement (the `life == 5` case is not drawn because `LIFE` is 5 and the first death goes to 4). Zero remaining lives skip this and go to `showGameOver` (a slice of `home.bmp` at `y = -YSIZE`).

## Example: decode a mask

`21` = `16 | 4 | 1` → shoot + jump + left. The Python and C++ command-flag examples decode any integer the same way the game would.
