# Input, UI, and saves

## Command bits

`Control::GetCommand` ORs `GetAsyncKeyState` high bits:

| Macro | Value | Keys |
| --- | --- | --- |
| `CMD_LEFT` | 1 | A |
| `CMD_RIGHT` | 2 | D |
| `CMD_UP` | 4 | W or K |
| `CMD_DOWN` | 8 | S (read, unused in `Role::action`) |
| `CMD_SHOOT` | 16 | J |
| `CMD_ESC` | 32 | Esc |

Virtual bits, never from the keyboard:

| Macro | Value | Source |
| --- | --- | --- |
| `VIR_RETURN` | 64 | Pause “返回游戏” or a successful save |
| `VIR_RESTART` | 128 | Pause “重新开始” |
| `VIR_HOME` | 256 | Pause “退出游戏” (label is “quit”; behaviour is title + reset) |

`getKey` only refreshes the latch when `_kbhit()` is true, **except** Esc
which still opens pause if the bit is already latched. After resume the last
movement bits remain until another `_kbhit`. Holding a key without a new
console event can therefore drop the walk bit for a frame. The kit’s replay
applies a bitset every tick and ignores that Win32 quirk.

## Title menu

`home.bmp` is one 512×(5×384) strip. The title uses offset 0.
Buttons: 开始 / 介绍 / 指导 / 退出 / 读档.

- 介绍 and 指导 are overlays with a 返回 hit box at the bottom-right.
- 退出 calls `exit(0)`.
- 读档 reads `gameRecord.dat`.

Hover swaps the Chinese label for a short English one (`Start`,
`Introduction`, `Directions`, `Exit`, `Read Load`).

## Pause

Esc draws a 90×120 green panel. Hover turns a row blue and flips the label
to English (`Return`, `start again`, `The menu`, `Write Data`).

Save path: `fopen_s(..., "gameRecord.dat", "w")` then `fprintf_s("%d", world)`.
No score, no lives, no x. Returns `VIR_RETURN` so the run continues.

## HUD

`showScore` / `showLevel` print `得分:` and `关卡:` in Cooper at (10,10) and
(XSIZE−90, 10). Death leftover lives (1–4) are drawn with the hero’s death
frame from `role.bmp`. Five icons are never shown because `life == 5` only
exists before the first death.

## `gameRecord.dat`

Plain ASCII integer, no newline required. V2.0’s checked-in file is `3`.

| Contents | Load |
| --- | --- |
| `1` `2` `3` | `world = flag`, leave title |
| `<= 0` or `>= 4` | `MessageBox` — **text and caption are swapped** (`"提醒"` is the body, `"存档缺失，请检查存档"` is the title) |
| missing file | `fopen_s` failure is not checked; `fscanf_s` on a null `FILE*` is undefined |

`examples/kit/savefile.py` and `SaveFile` in the C++ header accept 1–3 only
and treat anything else as `invalid`, matching the intended branch.

## Music aliases

Opened once. Restarts use `play ... from 0`. The infinite loop never reaches
`mciSendString("close all")` / `closegraph()`.
