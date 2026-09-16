# Controls and save file

Input is a bitset, not a queue. Several keys can be down in one tick.

## Command bits

`Control::GetCommand` ORs `GetAsyncKeyState` high bits:

| Macro | Value | Key | Gameplay |
| --- | --- | --- | --- |
| `CMD_LEFT` | 1 | `A` | Accelerate left |
| `CMD_RIGHT` | 2 | `D` | Accelerate right |
| `CMD_UP` | 4 | `W` or `K` | Jump |
| `CMD_DOWN` | 8 | `S` | Read, unused |
| `CMD_SHOOT` | 16 | `J` | Fire if `isShoot` |
| `CMD_ESC` | 32 | Esc | Open pause |

Virtual bits written by the pause menu (not from the keyboard):

| Macro | Value | Meaning |
| --- | --- | --- |
| `VIR_RETURN` | 64 | Close pause |
| `VIR_RESTART` | 128 | Rebuild current world |
| `VIR_HOME` | 256 | Title screen |

`getKey` only refreshes the bitset when `_kbhit()` is true. Held keys
therefore keep their last sampled mask across ticks that do not produce
a console key event. `GetAsyncKeyState` still sees the physical state
on those `_kbhit` ticks, so the pair is "sample on any console input."

Pause is entered when the stored `key` still has `CMD_ESC`. The menu
is a blocking `GetMouseMsg` loop (see below).

## Title menu (`gameStart`)

Five hit boxes, 90×30 px, centered at `XSIZE/2`, starting at
`YSIZE/3`:

| Index | Chinese | Hover English | Action |
| --- | --- | --- | --- |
| 0 | 开始 | Start | Return to `main` |
| 1 | 介绍 | Introduction | Story card + 返回 |
| 2 | 指导 | Directions | Key list + 返回 |
| 3 | 退出 | Exit | `exit(0)` |
| 4 | 读档 | Read Load | `fscanf` world from `gameRecord.dat` |

Introduction copy (as shipped):

- Horizontal platformer
- Hero is 猫里奥
- Three worlds; world 3 is the jump gauntlet
- Credit line: `游戏开发者：PWB`

操作指南 matches the table above.

Hover recolors the box (green on title, red on 返回) and swaps the
label to English. Moving off restores Chinese. The test for "need to
restore" is `getpixel` of a corner — EasyX-specific.

## Pause menu (`pauseClick`)

Four 90×30 boxes, green fill, 黑体 15:

| Index | Chinese | Hover English | Result |
| --- | --- | --- | --- |
| 0 | 返回游戏 | Return | `VIR_RETURN` |
| 1 | 重新开始 | start again | `VIR_RESTART` |
| 2 | 退出游戏 | The menu | `VIR_HOME` |
| 3 | 进行存档 | Write Data | write save, then `VIR_RETURN` |

`FlushMouseMsgBuffer` runs before the loop so clicks during the
previous frame do not instantly confirm an item.

## Save file

Path: `gameRecord.dat` in the **process working directory** (the exe
folder if launched from Explorer; the VS debug cwd if launched from
the IDE).

Format: one integer, `fprintf_s(fp, "%d", world)`.

Load:

```cpp
fscanf_s(fp, "%d", &flag);
if (flag >= 4 || flag <= 0)
    MessageBox(..., "存档缺失，请检查存档", ...);
else
    world = flag;
```

There is no checksum, no life count, no score, no hero position. A
missing file is not handled: `fopen_s` failure still flows into
`fscanf_s`. A robust loader is in `examples/04_save_load`.

## HUD

`showScore` / `showLevel` print `得分:` and `关卡:` with the Cooper
font, transparent background, then restore `OPAQUE`. Score is
`Role::score` (coins 10, kills 5). Level is the `world` global.

## Portable packing

`maoliao::pack_command(left, right, up, down, shoot, esc)` and
`maoliao::has_command(bits, mask)` mirror the macros so examples can
assert combinations such as left+jump (`CMD_LEFT | CMD_UP` == 5)
without Windows headers.
