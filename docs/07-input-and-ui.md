# 07 — Input and UI

`Control` is the only class that talks to the keyboard, the mouse, and the home-art strip.

## Keyboard → command bits

`getKey()`:

1. If `_kbhit()`, replace `key` with `GetCommand()`.
2. If `key & CMD_ESC`, block in `pauseClick()`.
3. Return `key`.

`GetCommand` is edge-less. It samples `GetAsyncKeyState` and **ors** bits. Because `getKey` only refreshes when the console keyboard buffer is non-empty, a key that is held after the buffer is drained can “stick” as the last bitset until another `_kbhit()`. That is why the pause Esc bit stays set and why `VIR_*` values persist for subsequent frames until a new real key arrives.

| Bit | Macro | Key |
| --- | --- | --- |
| 1 | `CMD_LEFT` | A |
| 2 | `CMD_RIGHT` | D |
| 4 | `CMD_UP` | W **or** K |
| 8 | `CMD_DOWN` | S (no gameplay effect) |
| 16 | `CMD_SHOOT` | J |
| 32 | `CMD_ESC` | Esc |

Virtual bits (pause menu) are not or-ed with the real ones; they **overwrite** `key`.

## Home screen — `gameStart`

`home.bmp` is loaded once in the constructor as `XSIZE × 5*YSIZE` (512×1920): five stacked 384-px panels.

| Panel (y offset) | Used by |
| --- | --- |
| 0 | Home / intro / guide background |
| `-YSIZE` | Game over (`showGameOver`) |
| `-2*YSIZE` | Level-clear card (`showPassed`) and death backdrop |
| `-3*YSIZE` | All-clear (`showPassedAll`) |
| 4th unused | reserved in the bitmap |

The menu is a 90×150 gray (`0x666666`) column centered at `XSIZE/2`, starting at `YSIZE/3`:

| Row | Chinese | Hover English |
| --- | --- | --- |
| 0 | 开始 | Start |
| 1 | 介绍 | Introduction |
| 2 | 指导 | Directions |
| 3 | 退出 | Exit |
| 4 | 读档 | Read Load |

介绍 text (as shipped):

> 这是一款横版过关游戏。游戏主角叫猫里奥，共有三关。第三关为跳跃关卡。游戏开发者：PWB

指导 text:

> 向左移动：A键 / 向右移动：D键 / 开火射击：J键 / 跳跃：W键/K键 / 暂停键：Esc键

Hover uses `getpixel` of the button interior to decide whether to repaint gray. Intro / guide “返回” hover is red vs black.

读档 opens `gameRecord.dat`, reads an int, and accepts it only if it is in `{1,2,3}`. Otherwise a `MessageBox("提醒", "存档缺失，请检查存档", 1)` is shown. The title and body arguments are swapped relative to the usual `MessageBox(hwnd, text, caption, type)` order — the dialog caption becomes the reminder sentence.

退出 is `exit(0)`.

## Pause — `pauseClick`

A 90×120 green column, four rows:

| Row | Chinese | Hover English | Result |
| --- | --- | --- | --- |
| 0 | 返回游戏 | Return | `VIR_RETURN` |
| 1 | 重新开始 | start again | `VIR_RESTART` |
| 2 | 退出游戏 | The menu | `VIR_HOME` |
| 3 | 进行存档 | Write Data | write `world`, then `VIR_RETURN` |

`FlushMouseMsgBuffer` runs before the loop so the Esc click does not also count as a menu click.

The third row is labeled 退出游戏 / “The menu” but `main` treats `VIR_HOME` as **return to home**, not process exit.

## HUD

`showScore` / `showLevel` use font `"Cooper"`, transparent background:

- `得分:  N` at (10, 10)
- `关卡:  W` at (XSIZE - 90, 10)

## Death icons

`showDied(life)` blits the death frame of `role.bmp` once per remaining life, centered, for `life` in 1..4. After the 5th life is spent you never see this screen — `showGameOver` runs instead. With `LIFE == 5`, the first death shows four cats.

## Fonts referenced

黑体 (menu), Gill Sans (unused title style is set then overwritten), Cooper (HUD), Goudy Stout (death screen style is set, then images cover it). Missing fonts fall back to whatever GDI has.
