# Input and UI

## Command bits

`Control::GetCommand` OR-assigns Windows async key state into the mask defined in `define.h`:

| Bit | Value | Key |
| --- | --- | --- |
| `CMD_LEFT` | 1 | A |
| `CMD_RIGHT` | 2 | D |
| `CMD_UP` | 4 | W or K |
| `CMD_DOWN` | 8 | S |
| `CMD_SHOOT` | 16 | J |
| `CMD_ESC` | 32 | Esc |

`getKey` only refreshes the mask when `_kbhit()` is true, then keeps returning the last value.
That means a held chord stays active even between keyboard-message gaps, and a pause that sets a
virtual key (`VIR_*`) overwrites the real mask until the next `_kbhit`.

Virtual keys used after the pause menu:

| Bit | Value | Meaning |
| --- | --- | --- |
| `VIR_RETURN` | 64 | Resume |
| `VIR_RESTART` | 128 | Rebuild this world |
| `VIR_HOME` | 256 | Reset lives, world 1, home screen |

`main` tests `VIR_RESTART` and `VIR_HOME` **before** `Role::action`. Resume is a no-op besides
closing the overlay.

## Home screen

`home.bmp` is loaded at `512 × (5*384)`. The home blit uses offset `(0, 0)`. Other slices:

| Offset | Screen |
| --- | --- |
| `y = 0` | Title / menu |
| `y = -YSIZE` | Game over |
| `y = -2*YSIZE` | Between-world card and death life-icons |
| `y = -3*YSIZE` | All-clear |

Five 90×30 buttons sit at `x ∈ [211, 301]`, `y` starting at `YSIZE/3 = 128`, stacked every 30 px:
Start, Introduction, Directions, Exit, Read save. Hover swaps the fill to green and the caption
to English (`Start`, `Introduction`, `Directions`, `Exit`, `Read Load`).

Introduction and Directions are full-screen text pages with a **返回** hit box in the bottom-right
44×24 corner. Clicking it recursively calls `gameStart()` after clearing flags (the original
re-enters the menu function rather than sharing a redraw helper).

Exit calls `exit(0)`. Read save is described in [save-and-audio.md](save-and-audio.md).

## Pause overlay

Esc draws a green 90×120 panel at the same x-center, four rows: 返回游戏, 重新开始, 退出游戏,
进行存档. Hover turns a row blue and switches the label to English (`Return`, `start again`,
`The menu`, `Write Data`). The third row’s Chinese is “退出游戏” but the handler sets `VIR_HOME`
(main menu), matching the comment, not `exit(0)`.

Mouse-move highlighting uses `getpixel` of the row’s interior to decide whether to restore the
green Chinese label. That is why the first draw must fill the polygon before the hover loop.

`FlushMouseMsgBuffer` runs once before the loop so the Esc click cannot also count as a menu
click.

## HUD

Score at (10, 10) as `得分:  N` in Cooper. Level at (XSIZE − 90, 10) as `关卡:  W`. Both toggle
background mode to transparent for the text and back to opaque afterward.

Death interstitial draws 1–4 cat faces from `role.bmp` (column 2) centered on the between-world
slice. With 5 lives the first death still has 4 remaining, so the four-icon branch is the full
strip; a fifth icon was never drawn because `life == 0` goes to game over instead.

## Timing of interstitial screens

| Screen | Sleep |
| --- | --- |
| Death pose in-world | 3500 ms (`Timer`) |
| Remaining-life card | 2000 ms (`Sleep`) |
| Game over card | 6500 ms |
| Between worlds | 6500 ms music + 2000 ms card |
| All-clear | 6500 + 7800 ms |

`examples/input/command_bits.cpp` checks every documented mask and the home / pause hit rectangles.
