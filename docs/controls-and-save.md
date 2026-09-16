# Controls, HUD, and save file

Input is a bit mask, not a queue of events. `Control::GetCommand` ORs every key that `GetAsyncKeyState` reports as down (`0x8000`).

## Keyboard

| Key | Macro | Bit | Action |
| --- | --- | ---: | --- |
| A | `CMD_LEFT` | 1 | Run left (`a -= A_ROLE`) |
| D | `CMD_RIGHT` | 2 | Run right |
| W or K | `CMD_UP` | 4 | Jump (world 3: tap/hold for extra hops) |
| S | `CMD_DOWN` | 8 | Read, unused |
| J | `CMD_SHOOT` | 16 | Fire if `Hero.isShoot` |
| Esc | `CMD_ESC` | 32 | Pause overlay |

Virtual commands (pause menu), also bits:

| Menu row | Macro | Bit | Effect in `main` |
| --- | --- | ---: | --- |
| 返回游戏 / Return | `VIR_RETURN` | 64 | Close pause, keep state |
| 重新开始 / start again | `VIR_RESTART` | 128 | Rebuild current world |
| 退出游戏 / The menu | `VIR_HOME` | 256 | Title screen, `life = 5`, `world = 1` |
| 进行存档 / Write Data | (writes file, then `VIR_RETURN`) | — | See below |

The home screen is mouse-only. In-game movement is keyboard-only. Pause is Esc then mouse.

## Home menu (`gameStart`)

Five stacked hit boxes, 90×30 px, centered at x = 256.

| Row (Chinese idle) | Hover English | Action |
| --- | --- | --- |
| 开始 | Start | Leave the menu, enter `world` (1 unless a load succeeded) |
| 介绍 | Introduction | Blurb: three stages, world 3 is the jump stage, credit `PWB` |
| 指导 | Directions | Control cheat sheet |
| 退出 | Exit | `exit(0)` |
| 读档 | Read Load | Read `gameRecord.dat` into `world` |

Introduction and Directions draw a 返回 button at the bottom-right. Clicking it recursively calls `gameStart()` again.

Load validates `1 <= flag <= 3`. Anything else shows a `MessageBox` whose title/text arguments are swapped (`"提醒"` is the body, `"存档缺失，请检查存档"` is the caption) — cosmetic, the dialog still appears.

## Pause menu

Same 90 px column, four rows. Saving does **not** leave the pause state; it writes the file and returns `VIR_RETURN` so the run continues.

`FlushMouseMsgBuffer` runs before the inner mouse loop because EasyX otherwise delivers clicks that happened during the previous `Sleep`.

## HUD

Every frame, after sprites:

- `showScore` at (10, 10), font Cooper, `"得分:  "` + integer
- `showLevel` at (XSIZE - 90, 10), `"关卡:  "` + `world`

## Lives and interstitial art

`LIFE` is 5. `home.bmp` is a 512×1920 strip (five 384 px screens):

| `putimage` y | Screen |
| ---: | --- |
| 0 | Title |
| `-YSIZE` | Game Over |
| `-2 * YSIZE` | Stage clear **and** death (remaining-life icons) |
| `-3 * YSIZE` | All-clear |

`showDied(life)` blits 1–4 dead-pose hero frames (`role.bmp` column x = 64) across the center. With 5 lives the first death passes `life == 4` after the decrement, so four icons show. The fifth life spent goes to Game Over instead.

Sleeps (milliseconds) are authored, not frame-counted:

| Beat | Sleep |
| --- | ---: |
| Death pose on the field | 3500 |
| Remaining-life card | 2000 |
| Stage-clear jingle before next card | 6500 |
| Stage-clear card | 2000 |
| All-clear music tail | 6500 then 7800 |
| Game Over card | 6500 |

## `gameRecord.dat`

Path: working directory, not `res/`. Format: a single `fprintf` integer, no newline required (the original writes `"%d"`).

```
3
```

means “continue at world 3”. Score, lives, `isShoot`, coins, and world-3 pipe heights are **not** saved.

The repository already contains `MaoLiAo/gameRecord.dat` (one byte). Treat it as a sample, not as a player profile.

`examples/save_roundtrip` reads and writes the same format without EasyX.

## Command decoding example

The portable helper is `examples/lib/input_bits.hpp`. A mask of `A` + `J` is `CMD_LEFT | CMD_SHOOT == 17`. `main` never needs to decode more than one virtual command at a time; the pause menu assigns `key` rather than OR-ing it.
