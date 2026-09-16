# Assets

Measured from the files in `MaoLiAo/res/` on this checkout (BMP `BITMAPINFOHEADER`, 24-bit, bottom-up).

## Bitmaps

| File | Bytes | Size | How it is used |
|---|---|---|---|
| `role.bmp` | 30 774 | **160×64** | hero walk / death. 5 columns × 2 rows of 32 px. Row 0 color (`SRCPAINT`), row 1 mask (`SRCAND`). Cols 0–1 face right, col 2 death, cols 3–4 face left. |
| `map.bmp` | 67 638 | **64×352** | terrain sheet. 64 px wide (two tiles — pipes). 352/32 = 11 rows → ids 1–11 at `y0 = (id-1)*32`. Id 7 blits 64×32; ids 8 and 10 blit 64×64. |
| `scenery.bmp` | 331 830 | **192×576** | animated decor. Frame width `3*32 = 96`, two frames across. Each id 11–14 uses a 4-tile-tall block: `y0 = (id-11)*128`. Color at `y0`, mask at `y0+64`. `scenery_iframe` loops 1→2. |
| `ani.bmp` | 333 366 | **256×434** | shared VFX / pickups (see rows below). 256 = 8 × 32. Height is **not** a multiple of 32 because food uses 25 px rows. |
| `mapsky.bmp` | 2 359 350 | **512×1536** | four stacked 384 px skies. `yBg = -(world-1)*384` selects world 1–3; a fourth strip is unused. Looped horizontally. |
| `home.bmp` | 2 949 174 | **512×1920** | five stacked 384 px UI pages (title, game over, level card, all-clear, spare). |
| `MaoLiAo.ico` | 9 662 | (icon) | `IDI_ICON1` in `MaoLiAo.rc`. |

EasyX transparent blit is always the pair

```
putimage(..., SRCAND);    # punch the mask
putimage(..., SRCPAINT);  # paint the color
```

### `ani.bmp` rows (from `role.cpp` / `scene.cpp`)

| Rows (32 px, unless noted) | Content | Frames |
|---|---|---|
| 0 color / 1 mask | walking enemy | iframe 1–2, `+= TIME*5` |
| 2 color / 3 mask | bullet | iframe 1–2, `+= TIME*10` |
| 4–5 color / 6–7 mask | bomb (64×64) | iframe 1–4 |
| 8 color / 9 mask | coin | iframe 1–4, `+= TIME*7` |
| 10 color / 11 mask | score pop | iframe 1–4, `+= TIME*8` |
| y = 384, h = 25 color / +25 mask | food / weapon (`52 × 25`) | iframe 1–2 |

434 − 384 = 50, which matches two 25 px food rows.

### `map.bmp` rows

| Row | id | Comment in `createMap` |
|---|---|---|
| 0 | 1 | 草砖 |
| 1 | 2 | 云朵 |
| 2 | 3 | 草砖或沙漠的地下 |
| 3 | 4 | 雪地地下 |
| 4 | 5 | 雪地地皮 |
| 5 | 6 | 管道中间 / slick slab |
| 6 | 7 | 管道延长 (world 3) |
| 7 | 8 | 倒着的管道口 |
| 8 | 9 | (unused in data) |
| 9 | 10 | 向上的管道口 |
| 10 | 11 | (sheet leftover; scenery lives in `scenery.bmp`) |

## Music (`mciSendString` aliases)

| File | Bytes | Alias | When |
|---|---|---|---|
| `背景音乐.mp3` | 622 446 | `music_bg` | loop in `main` |
| `胜利.mp3` | 39 131 | `music_win` | world clear |
| `通关.mp3` | 77 184 | `music_passedAll` | after world 3 |
| `游戏结束.mp3` | 45 210 | `music_end` | lives = 0 |
| `死亡1.mp3` | 35 179 | `music_died` | hero death |
| `跳.mp3` | 7 837 | `music_jump` | jump |
| `金币.mp3` | 11 912 | `music_coin` | coin |
| `踩敌人.mp3` | 3 762 | `music_tread` | stomp |
| `吃到武器.mp3` | 12 225 | `music_getWeapon` | food |
| `子弹.mp3` | 1 881 | `music_bullet` | shot |
| `子弹撞墙.mp3` | 3 135 | `music_boom` | bullet vs tile / range |
| `子弹打到敌人.mp3` | 2 508 | `music_boom2` | bullet vs enemy |

Aliases are opened with `open res\<name>.mp3 alias ...` (backslash paths). `play ... from 0` restarts a cue. Nothing is closed until the dead code after the infinite loop (`close all`).

## Fonts the UI asks for

Home title style `Gill Sans` (the “猫里奥” `drawtext` is commented out). Buttons `黑体`. HUD `Cooper`. Death screen `Goudy Stout`. Missing fonts fall back to whatever EasyX / GDI substitutes; the layout still uses the same `RECT`s.
