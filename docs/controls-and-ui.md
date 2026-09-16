# Controls and UI

Input is **asynchronous**. `Control::GetCommand` ORs
`GetAsyncKeyState` bits so several keys can be down on one frame.
`getKey` only refreshes that mask when `_kbhit()` is true; the last
mask is reused otherwise. Esc (`CMD_ESC`) opens the pause overlay and
replaces `key` with a virtual command.

## Command bits (`define.h`)

| Bit | Value | Source |
| --- | --- | --- |
| `CMD_LEFT` | 1 | `A` |
| `CMD_RIGHT` | 2 | `D` |
| `CMD_UP` | 4 | `W` or `K` |
| `CMD_DOWN` | 8 | `S` (read, unused in `action`) |
| `CMD_SHOOT` | 16 | `J` |
| `CMD_ESC` | 32 | `VK_ESCAPE` |
| `VIR_RETURN` | 64 | Pause → 返回游戏 |
| `VIR_RESTART` | 128 | Pause → 重新开始 |
| `VIR_HOME` | 256 | Pause → 退出游戏 (returns to title) |

`main` treats `VIR_RESTART` and `VIR_HOME` specially. Everything else
is passed into `Role::action`.

`examples/05_command_mask` packs and unpacks the same numbers.

## Title (`Control::gameStart`)

Background: first 512×384 strip of `home.bmp`.

Five buttons, 90×30, centered at x = 256, starting at y = 128:

| Index | Chinese | Hover English | Action |
| --- | --- | --- | --- |
| 0 | 开始 | Start | Leave the title loop |
| 1 | 介绍 | Introduction | Story blurb + 返回 |
| 2 | 指导 | Directions | Key list + 返回 |
| 3 | 退出 | Exit | `exit(0)` |
| 4 | 读档 | Read Load | `fscanf` world from `gameRecord.dat` |

Introduction copy (as shipped):

- 横版过关, hero 猫里奥, three worlds
- World 3 is the jump stage
- Credit line: `游戏开发者：PWB`

Directions:

- A / D move, J shoot, W or K jump, Esc pause

Hovering a button fills it green and swaps in the English label.
The intro / directions "返回" button fills red and reads `Return`.

A successful read of `1`, `2`, or `3` sets the global `world` and
starts that stage. Anything else (`<= 0` or `>= 4`) shows
`MessageBox(..., "存档缺失，请检查存档")`.

Returning from intro/directions calls `gameStart()` recursively.

## Pause (`Control::pauseClick`)

Green 90×120 panel. Four rows:

| Row | Chinese | Hover English | Result |
| --- | --- | --- | --- |
| 0 | 返回游戏 | Return | `VIR_RETURN` |
| 1 | 重新开始 | start again | `VIR_RESTART` |
| 2 | 退出游戏 | The menu | `VIR_HOME` |
| 3 | 进行存档 | Write Data | write `world`, then `VIR_RETURN` |

Mouse-move redraws one row blue (in) or green (out). The click test
uses the same rectangles. `FlushMouseMsgBuffer` runs before the loop
because the frame delay otherwise leaves stale clicks.

Save write:

```c
fopen_s(&fp, "gameRecord.dat", "w");
fprintf_s(fp, "%d", world);
```

Working directory is whatever launched the `.exe`. The committed
`MaoLiAo/gameRecord.dat` is a one-byte placeholder.

## HUD

Every frame, after the world blit:

- `showScore` — `"得分:  "` + integer, font Cooper, top-left (10, 10)
- `showLevel` — `"关卡:  "` + `world`, top-right (422, 10)

## Interstitials

| Event | Function | Art |
| --- | --- | --- |
| Death with lives left | `showDied(life)` | `home.bmp` strip + 1–4 death sprites |
| Life == 0 | `showGameOver` | `home.bmp` at `y = -384`, 6.5 s |
| Clear world 1 or 2 | `showPassed(next)` | strip + `"LEVEL: N"`, 2 s |
| Clear world 3 | `showPassedAll` | next strip, 7.8 s |

`showDied` only draws sprites for `life` in `{1,2,3,4}`. After losing
the first of five lives the screen is the background only.

Death in the loop sleeps 3.5 s on the last gameplay frame before
that interstitial. World-clear sleeps 6.5 s on the win jingle first.

## Window

```
initgraph(XSIZE, YSIZE)     // 512 × 384
SetWindowText(..., "猫里奥 V2.0")
```

Icon: `MaoLiAo.ico` via `MaoLiAo.rc` (`IDI_ICON1 = 101`).
