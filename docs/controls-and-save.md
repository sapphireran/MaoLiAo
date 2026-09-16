# Controls and save

## Command bits

`define.h` uses powers of two so `GetCommand` can OR several keys in one frame.

| Macro | Value | Source |
| --- | ---: | --- |
| `CMD_LEFT` | 1 | `A` |
| `CMD_RIGHT` | 2 | `D` |
| `CMD_UP` | 4 | `W` or `K` |
| `CMD_DOWN` | 8 | `S` (read, unused in `action`) |
| `CMD_SHOOT` | 16 | `J` |
| `CMD_ESC` | 32 | `VK_ESCAPE` |
| `VIR_RETURN` | 64 | pause: 返回游戏 |
| `VIR_RESTART` | 128 | pause: 重新开始 |
| `VIR_HOME` | 256 | pause: 退出游戏 |

`Control::GetCommand` uses `GetAsyncKeyState(...) & 0x8000` so opposite keys can both be down (`LEFT|RIGHT` nets zero wish acceleration after `direction.x` += / -=).

`getKey` only refreshes the mask when `_kbhit()` is true, then if `CMD_ESC` is set it blocks in `pauseClick`. After pause, the virtual bit **replaces** the mask (`key = VIR_RETURN` etc.), it is not OR-ed.

## Title menu hit boxes

All five buttons are 90×30 px centered at `x = XSIZE/2` starting at `y = YSIZE/3`:

| Index | Label | Hover English | Action |
| ---: | --- | --- | --- |
| 0 | 开始 | Start | leave `gameStart` |
| 1 | 介绍 | Introduction | overlay + 返回 |
| 2 | 指导 | Directions | overlay + 返回 |
| 3 | 退出 | Exit | `exit(0)` |
| 4 | 读档 | Read Load | parse `gameRecord.dat` |

介绍 / 指导 text is hard-coded in `control.cpp` (three worlds, 第三关为跳跃关卡, author line “PWB”). 返回 is the 44×24 box at the bottom-right.

Hover highlight uses `getpixel` of the fill color (green on title, blue on pause, red on 返回). That is why the buttons are opaque polygons first.

## Pause menu

Same 90×30 stack, four rows, green fill. Hover swaps the label to English (`Return`, `start again`, `The menu`, `Write Data`).

存档 path:

```
fopen_s(..., "gameRecord.dat", "w")
fprintf_s(fp, "%d", world)
```

No score, no lives, no hero position — **world index only**.

## Load

```
fopen_s(..., "gameRecord.dat", "r")
fscanf_s(fp, "%d", &flag)
if (flag >= 4 || flag <= 0)  MessageBox("提醒", "存档缺失，请检查存档")
else world = flag
```

The checked-in `MaoLiAo/gameRecord.dat` contains `3` (world 3). There is no `fclose` on the error path. `fopen` failure is not tested before `fscanf`.

A valid load sets `world` and then `main` constructs `Role(world)` / `Scene(world)` — so 读档 starts that stage with full (reset) hero state and the current `life` (still 5 if you just launched).

## Portable reconstruction

`examples/common/save_portable.{h,cpp}` writes the same one-integer file.
`examples/04_save_record` round-trips worlds 1–3 and rejects 0 and 4 the same way.
`examples/03_command_bits` checks the OR tables and “Esc replaces the mask” pause rule.

Working directory: the game looks for `gameRecord.dat` next to the **process CWD**, which Visual Studio usually sets to `MaoLiAo/`. Examples write under `examples/build/` so they never clobber the checked-in file.
