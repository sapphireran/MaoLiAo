# Controls and save file

Input is a bit mask, not a queue of key events. `Control::GetCommand` ORs every key that `GetAsyncKeyState` reports as down (`0x8000`) in that call.

## Command bits

From `define.h`:

| Macro | Value | Hardware |
| --- | --- | --- |
| `CMD_LEFT` | 1 | `A` |
| `CMD_RIGHT` | 2 | `D` |
| `CMD_UP` | 4 | `W` or `K` |
| `CMD_DOWN` | 8 | `S` (read, never used by `Role::action`) |
| `CMD_SHOOT` | 16 | `J` |
| `CMD_ESC` | 32 | `Esc` |
| `VIR_RETURN` | 64 | Pause → 返回游戏 |
| `VIR_RESTART` | 128 | Pause → 重新开始 |
| `VIR_HOME` | 256 | Pause → 退出游戏 (returns to title, does not `exit`) |

`examples/maoliao_lib/commands.py` names the same bits and provides `pack` / `unpack` helpers.

`getKey` only calls `GetCommand` when `_kbhit()` is true. The last mask stays latched otherwise. That is why a direction can keep applying for a frame or two after the key is released if nothing else hits the console buffer — and why `Esc` must be seen by both the console and `GetAsyncKeyState` to open the pause menu.

`Role::action` tests bits with `KEY & CMD_*`. Several keys can be down at once (`A`+`J`, `D`+`W`). Left and right both down cancel on the acceleration (`a -= 20` then `a += 20`) and leave `direction.x` at 0.

## Title menu (`gameStart`)

Drawn on the top slice of `home.bmp` (see [resources.md](resources.md)). Five hit boxes, each 90×30 px, centered at x = 256, starting at y = 128 (`YSIZE/3`):

| Index | Chinese | Hover English | Action |
| --- | --- | --- | --- |
| 0 | 开始 | Start | Leave the menu; `main` builds `Role`/`Scene` for the current `world` |
| 1 | 介绍 | Introduction | Overlay: three worlds, world 3 is the jump stage, credit `PWB` |
| 2 | 指导 | Directions | Overlay: A/D/J/W·K/Esc |
| 3 | 退出 | Exit | `exit(0)` |
| 4 | 读档 | Read Load | Open `gameRecord.dat`, replace `world` |

介绍 / 指导 draw a 返回 box at the bottom-right. Clicking it recursively calls `gameStart()` after clearing the page flags.

读档:

```cpp
fopen_s(&fp, "gameRecord.dat", "r");
fscanf_s(fp, "%d", &flag);
if (flag >= 4 || flag <= 0)
    MessageBox(..., "存档缺失，请检查存档", ...);
else
    world = flag;
```

A missing file is not checked before `fscanf_s`. A value outside `1 … 3` is rejected. The file in the tree currently holds `3`.

## Pause menu (`pauseClick`)

`Esc` sets `CMD_ESC`. `getKey` then blocks in `pauseClick` until a click picks a row. Four 90×30 boxes starting at the same origin as the title stack:

| Index | Chinese | Hover English | Result bit |
| --- | --- | --- | --- |
| 0 | 返回游戏 | Return | `VIR_RETURN` — loop continues with the latched key |
| 1 | 重新开始 | start again | `VIR_RESTART` — `main` reconstructs this world |
| 2 | 退出游戏 | The menu | `VIR_HOME` — title, `life = 5`, `world = 1` |
| 3 | 进行存档 | Write Data | write `world`, then `VIR_RETURN` |

The Chinese label on row 2 is **退出游戏** but the comment and the `main` branch treat it as **主菜单**. It does not call `exit`.

Save from pause:

```cpp
fopen_s(&fp, "gameRecord.dat", "w");
fprintf_s(fp, "%d", world);
```

Only the world index is stored. Lives, score, `isShoot`, coin state, and camera `x0` are not. Loading always rebuilds a fresh world at that index.

The working directory must be `MaoLiAo/` (or the process must otherwise see `gameRecord.dat` next to the exe). That is the same requirement the `res\...` paths have.

## HUD

`showScore` / `showLevel` draw Cooper-style text, transparent, at (10, 10) and (422, 10). They do not participate in input.

`showDied(life)` paints the world-2 slice of `home.bmp` and one 猫里奥 sprite per remaining life (1–4). After the fifth death `life` is already 0 and `showGameOver` is used instead, so the five-sprite case is never drawn.

## Save file format

`MaoLiAo/gameRecord.dat` is a text file containing a single decimal integer and nothing else. No newline is required. The checked-in file is:

```
3
```

`examples/save/test_save.py` reads that file, rejects out-of-range values with the same rule as 读档, and writes a temp copy the way 进行存档 does.

## Portable bit packing

```python
from maoliao_lib.commands import CMD, pack, unpack

mask = pack("left", "shoot")   # 1 | 16 = 17
assert unpack(mask) == ("left", "shoot")
```

`examples/input/test_commands.py` checks every named bit and the pause / title result codes.
