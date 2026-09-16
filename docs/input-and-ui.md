# Input, menus, and save file

## Command bits

`Control::GetCommand` OR-combines whatever `GetAsyncKeyState` reports as
down (`0x8000`). `getKey` only refreshes that mask when `_kbhit()` is true,
so a key that was read once **sticks** until another key event arrives.
That is why the cat keeps walking after you release `D` until you tap
something else — the last mask is reused.

| Macro | Value | Key |
| --- | --- | --- |
| `CMD_LEFT` | 1 | `A` |
| `CMD_RIGHT` | 2 | `D` |
| `CMD_UP` | 4 | `W` or `K` |
| `CMD_DOWN` | 8 | `S` (read, unused in `action`) |
| `CMD_SHOOT` | 16 | `J` |
| `CMD_ESC` | 32 | `Esc` |
| `VIR_RETURN` | 64 | pause → resume |
| `VIR_RESTART` | 128 | pause → rebuild world |
| `VIR_HOME` | 256 | pause → title |

Combinations are legal: `A+D` sets both bits, then `action` applies
`-A_ROLE` and `+A_ROLE` and they cancel. `W+K` is still just `CMD_UP`.

Virtual bits are not produced by the keyboard. `pauseClick` assigns them to
`Control::key` and `getKey` returns that value on the next tick. `main`
looks for `VIR_RESTART` and `VIR_HOME` before calling `Role::action`.
`Role::action` itself ignores those bits (they do not overlap the CMD
range in a way that triggers walk / jump).

## Title screen state machine

`gameStart` is a blocking mouse loop with four flags:

```
_HOME          main five-button column
_INTRODUCTION  credits page
_OPERATION     control list
_READ          declared, never set
```

Clicking **介绍** or **指导** draws a new page and a **返回** rectangle at
`(XSIZE-46, YSIZE-26)`. That return button does not just flip flags: it
calls `gameStart()` recursively after clearing them. Deep menu toggling
therefore grows the stack. **开始** and a successful **读档** clear `_HOME`
and fall out of the loop into the game.

Exit calls `exit(0)` and never closes MCI aliases or the EasyX device.

## Pause screen

`pauseClick` also blocks, this time on `GetMouseMsg`. It empties the mouse
queue first (`FlushMouseMsgBuffer`) because the 10 ms gameplay delay would
otherwise deliver stale clicks.

The four hit boxes are 90×30 px, stacked from `YSIZE/3`. Hover re-paints a
box blue with an English label; leaving it restores green + Chinese.

## Save format

Path: `MaoLiAo/gameRecord.dat` (working-directory relative).

Write (pause → 进行存档):

```
fopen_s(..., "w");
fprintf_s(fp, "%d", world);
```

Read (title → 读档):

```
fopen_s(..., "r");
fscanf_s(fp, "%d", &flag);
if (flag >= 4 || flag <= 0)  MessageBox("存档缺失，请检查存档")
else world = flag
```

There is no checksum, no score, no life count, no flower flag. A file that
contains only `2` starts you on the snow world with five lives and no
weapon.

The copy that ships in the repo is a one-byte file. Treat it as a sample
slot, not as a required binary format. The portable helper
`examples/include/maoliao/save.hpp` reads and writes the same single
integer and rejects anything outside `1..3`.

Load does not check `fopen_s` for failure before `fscanf_s`. A missing file
is therefore a crash, not the MessageBox path. The MessageBox path is only
for an existing file whose integer is out of range. The box argument order
is also swapped (`"提醒"` is passed as the text and `"存档缺失…"` as the
caption).

## HUD

`showScore` / `showLevel` print Cooper-font strings at (10, 10) and
(XSIZE-90, 10):

```
得分:  N
关卡:  N
```

They toggle `setbkmode(TRANSPARENT)` for the text and restore `OPAQUE`
afterward so later `putimage` calls do not keep the transparent flag.

## Life icon row

`showDied(life)` blits the death frame of `role.bmp` once per remaining life,
centered. It handles 1–4 icons. After the first death you have 4 lives left,
so the 4-icon branch runs; the 5-life start never appears on this screen
because the counter is decremented first.

## Virtual-key example

`examples/src/demo_commands.cpp` builds masks the same way `GetCommand`
would, then decodes them back to a label list. The test suite checks that
`A|D|W|J` equals `1|2|4|16` and that virtual bits stay out of the gameplay
mask.
