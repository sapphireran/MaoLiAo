# Input and UI

## Command bits

`Control::GetCommand` OR-combines every key that is down **this call**. `Role::action` therefore sees chords (left+jump, right+shoot, …).

| Macro | Value | Source |
| --- | --- | --- |
| `CMD_LEFT` | 1 | `A` |
| `CMD_RIGHT` | 2 | `D` |
| `CMD_UP` | 4 | `W` or `K` |
| `CMD_DOWN` | 8 | `S` (read, unused) |
| `CMD_SHOOT` | 16 | `J` |
| `CMD_ESC` | 32 | `VK_ESCAPE` |
| `VIR_RETURN` | 64 | pause → 返回游戏 |
| `VIR_RESTART` | 128 | pause → 重新开始 |
| `VIR_HOME` | 256 | pause → 退出游戏 (returns to title, not process exit) |

`getKey` only refreshes the mask when `_kbhit()` is true. That means:

- The last non-empty mask **sticks** across frames until another key event arrives.
- Releasing keys does not clear `key` unless a new `_kbhit` happens (which still calls `GetCommand` and can return 0 if nothing is down).

Escape sets `CMD_ESC` on the sticky mask and opens `pauseClick()`. The pause routine overwrites `key` with a `VIR_*` value and returns.

`main` treats `VIR_RESTART` and `VIR_HOME` as level reconstructs. `VIR_RETURN` is ignored there, so play continues.

A bit-twiddling walkthrough is `examples/command_bits.cpp`.

## Title (`gameStart`)

`home.bmp` is scaled to `512 × 5*384` and the title uses the top 384 px band.

Five stacked 90×30 buttons, centered horizontally at `y = YSIZE/3 + i*30`:

0. 开始 / Start — leave the menu, enter world `world` (usually 1).
1. 介绍 / Introduction — short Chinese blurb (three worlds, world 3 is a jump stage, credit `PWB`).
2. 指导 / Directions — the control table.
3. 退出 / Exit — `exit(0)`.
4. 读档 / Read Load — read `gameRecord.dat` (see [save-format.md](save-format.md)).

Hover recolors the hit cell green and swaps the label to English. Intro / guide pages have a bottom-right 返回 button.

Nested `gameStart()` on 返回 rebuilds the menu; `_HOME` is then set false so the outer `while` exits. That is intentional but easy to break if you add more pages.

## Pause (`pauseClick`)

A green 90×120 panel with four rows: 返回游戏, 重新开始, 退出游戏, 进行存档.

- Return → `VIR_RETURN`.
- Restart → `VIR_RESTART` (same world, new `Role`/`Scene`, rewind BGM).
- “Exit game” → `VIR_HOME` (title, `life = LIFE`, `world = 1`).
- Save → write `world` as decimal text to `gameRecord.dat`, then `VIR_RETURN`.

Hover uses blue fill and English labels (`Return`, `start again`, `The menu`, `Write Data`). `FlushMouseMsgBuffer` runs before the loop so the click that opened pause is not reused.

## HUD

Every gameplay frame, after the scene and hero:

- `showScore`: `"得分:  "` + integer, font Cooper, top-left (10, 10).
- `showLevel`: `"关卡:  "` + `world`, top-right (`XSIZE - 90`, 10).

Both temporarily set `TRANSPARENT` background mode so they sit on the sky.

## Transition screens

| Method | Art | Dwell |
| --- | --- | --- |
| `showDied(life)` | `home.bmp` band at `-2*YSIZE`, plus 1–4 dead-pose hero frames | 2.0 s |
| `showGameOver` | `home.bmp` at `-YSIZE` | 6.5 s |
| `showPassed(nextWorld)` | same band as died, plus `"LEVEL: N"` | 2.0 s after a 6.5 s win jingle in `main` |
| `showPassedAll` | `home.bmp` at `-3*YSIZE` | 7.8 s after win + all-clear tracks |

`showDied` only draws poses for `life` in `{1,2,3,4}`. After the first death you have 4 lives left, so the splash matches. A fifth remaining life would show a blank band (`LIFE` is 5, first death → 4).

## Fonts

Menus request 黑体. HUD uses Cooper / Goudy Stout. Those names only resolve if the Windows box has the fonts; EasyX falls back to a default face otherwise. Debug|Win32 is multi-byte; Release configs are Unicode — Chinese literals can break in Release unless you change the character set. See [build.md](build.md).
