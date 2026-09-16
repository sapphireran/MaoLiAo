# Assets

Runtime paths are relative to the process working directory and use Windows backslashes (`res\\map.bmp`). Launch from `MaoLiAo/` or the paths miss.

This clone may not contain the `res/` folder (binary art / mp3). The table below is the **complete load list** from the current sources so you can restore a local `res/` from an older build or a backup.

## Bitmaps

| File | Loaded by | Size / usage |
| --- | --- | --- |
| `res\\mapsky.bmp` | `Scene` | Forced to `512 × 1536` (4 stacked skies). `yBg = -(world-1)*384` selects the row. Loops horizontally. |
| `res\\map.bmp` | `Scene` | Vertical tile strip. Cell `(0, (id-1)*32)` is a 32×32 tile. Ids 7–10 use 64-wide / 64-tall windows. |
| `res\\scenery.bmp` | `Scene` | Animated props. Frame width 96, block height 128; `y0 = (id-11)*128`. Two-pass `SRCAND` + `SRCPAINT`. 3 frames. |
| `res\\ani.bmp` | `Scene`, `Role` | Shared animation sheet: coins, food, enemies, bullets, bombs. |
| `res\\role.bmp` | `Role`, `Control::showDied` | Hero walk (2 frames × 2 facings) plus a death frame at `x = 64`. Mask row at `y = 32`, color row at `y = 0`. |
| `res\\home.bmp` | `Control` | Forced to `512 × 1920` (5 UI bands): title, game over, level / died, all-clear, spare. |
| `MaoLiAo.ico` | `MaoLiAo.rc` | Window / exe icon (`IDI_ICON1`). |

## `ani.bmp` rows used in code

Heights are 32 px rows unless noted.

| Rows (color / mask) | Consumer |
| --- | --- |
| 0 / 1 | Walking enemy |
| 2 / 3 | Bullet (2-frame loop) |
| 4–5 / 6–7 | Bomb (2×2 tiles, 4 frames) |
| 8 / 9 | Coin (4-frame sparkle) |
| 10 / 11 | Collect spark |
| 12 (+ 12 + 4/5 mask) | Food / weapon, 52×25 window |

## Hero sheet (`role.bmp`)

| Source x | Facing / pose |
| --- | --- |
| 0, 32 | Walk frames, facing right (`turn == 1`) |
| 96, 128 | Walk frames, facing left |
| 64 | Death pose (`showDied` and `Role::show` when `died`) |

Walk frame picks `hero_iframe` from world-space X: every `STEP` (10 px) toggles, every 40 px returns to frame 1.

Transparent blit is always:

```text
putimage(..., SRCAND);   // punch the mask
putimage(..., SRCPAINT); // paint color into the hole
```

Color is stored **above** the mask for the hero (`y=0` color, `y=32` mask) and **below** the mask for several `ani.bmp` actors. Copy the existing pair if you add a frame.

## Music (MCI, `Winmm.lib`)

Opened in `main`:

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | `res\\背景音乐.mp3` | Loop during play |
| `music_win` | `res\\胜利.mp3` | World clear |
| `music_passedAll` | `res\\通关.mp3` | After world 3 |
| `music_end` | `res\\游戏结束.mp3` | Lives exhausted |

Opened in `Role`:

| Alias | File | When |
| --- | --- | --- |
| `music_died` | `res\\死亡1.mp3` | Death |
| `music_jump` | `res\\跳.mp3` | Jump / flap |
| `music_coin` | `res\\金币.mp3` | Coin |
| `music_tread` | `res\\踩敌人.mp3` | Stomp |
| `music_getWeapon` | `res\\吃到武器.mp3` | Food |
| `music_bullet` | `res\\子弹.mp3` | Shot |
| `music_boom` | `res\\子弹撞墙.mp3` | Bullet vs tile / max range |
| `music_boom2` | `res\\子弹打到敌人.mp3` | Bullet vs enemy |

Commands use the `mciSendString` string API (`open`, `play … repeat`, `play … from 0`, `stop`, `close all`). There is no volume control and no failure check — a missing mp3 is a silent no-op.

## Icon / resources

`MaoLiAo.rc` includes `MaoLiAo.ico` as `IDI_ICON1`. `resource.h` is the stock Visual C++ generated header. You do not need to touch it unless you add a new Windows resource.

## Restoring `res/`

If you only have an old `.exe` next to a `res` folder, copy that folder to `MaoLiAo/res/`. The code does not pack bitmaps into the `.rc`; they stay loose files on disk.
