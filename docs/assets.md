# Assets

All runtime art and audio live under `MaoLiAo/res/`. Paths in the sources use Windows backslashes (`res\\map.bmp`). The working directory at launch must be the folder that contains `res\`.

BMP files are load-once in constructors. MP3 files are opened as MCI aliases (`Winmm.lib`) and replayed with `play <alias> from 0` or `play music_bg repeat`.

## Bitmaps

| File | Approx. size | Who loads it | Layout |
| --- | --- | --- | --- |
| `mapsky.bmp` | 512 × 1536 | `Scene` | Vertical atlas of three 512×384 skies. World *n* uses `yBg = -(n-1)*384`. Drawn twice horizontally and wrapped when `xBg <= -width`. |
| `map.bmp` | tile strip | `Scene` | One 32×32 (or 64×32 / 64×64 for pipes) row per tile id, selected by `y0 = (id-1)*32`. |
| `scenery.bmp` | animated props | `Scene` | 96×128 cells. Frame index `scenery_iframe` in `{1, 2}` cycles with macro `F`. Mask row then color row (`SRCAND` / `SRCPAINT`). Used for ids 11–14. |
| `ani.bmp` | sprite sheet | `Scene` + `Role` | Coins (rows 8–9), score pop (10–11), food / mushroom (row 12), enemies (rows 0–1), bullets (2–3), bombs (4–6). |
| `role.bmp` | 32×64 frames | `Role`, death card | Walk frames 0–1 facing right, death frame at x = 64, mirrored walk at x = 96+. Mask in the lower 32 px. |
| `home.bmp` | 512 × 1920 | `Control` | Five stacked 512×384 panels: title (0), game over (−384), level / death (−768), credits-style clear-all (−1152), spare (−1536). |

`loadimage(&img_bg, "res\\mapsky.bmp", XSIZE, 4 * YSIZE)` forces the sky to 512×1536. `home.bmp` is forced to 512×1920.

The walk cycle does not use a timer. `hero_iframe` flips between 1 and 2 from world-X:

```
rolePos = -x0 + x
if rolePos/10 % 2 == 0 and rolePos/10 % 4 != 0: frame 2
if rolePos/10 % 4 == 0: frame 1
```

`STEP = 10` is the walk-cycle quantum, not the physics step.

## Audio (MCI aliases)

Opened in `main.cpp`:

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | `背景音乐.mp3` | Loop during play; `from 0` on respawn |
| `music_win` | `胜利.mp3` | Any world clear |
| `music_passedAll` | `通关.mp3` | After the world-3 win sting |
| `music_end` | `游戏结束.mp3` | Lives exhausted |

Opened in `Role::Role`:

| Alias | File | When |
| --- | --- | --- |
| `music_died` | `死亡1.mp3` | Enemy touch or fall out of the world |
| `music_jump` | `跳.mp3` | Every accepted jump |
| `music_coin` | `金币.mp3` | Coin pickup |
| `music_tread` | `踩敌人.mp3` | Stomp |
| `music_getWeapon` | `吃到武器.mp3` | Mushroom |
| `music_bullet` | `子弹.mp3` | Shot fired |
| `music_boom` | `子弹撞墙.mp3` | Bullet vs tile or max range |
| `music_boom2` | `子弹打到敌人.mp3` | Bullet vs enemy |

There is no volume control and no pause-mute. `stop music_bg` runs on death, clear, and title return. Aliases opened in `Role` are re-opened every respawn (`open` on an existing alias is a no-op or a warning depending on the MCI driver).

## Icon

`MaoLiAo/MaoLiAo.ico` is compiled through `MaoLiAo.rc` as `IDI_ICON1` (101). The window title is hard-coded: `SetWindowText(hwnd, "猫里奥 V2.0")`.

## Checklist for a missing-asset debug

If EasyX pops a load failure:

1. Confirm cwd. Running the `.exe` from `MaoLiAo/Debug` without copying `res\` is the usual miss.
2. Confirm the six BMPs and twelve MP3s listed above are present. Git stores them; they are not LFS in this clone.
3. Debug|Win32 uses a multi-byte charset. Release|Win32/x64 flip to Unicode — Chinese `loadimage` paths and `outtextxy` strings can break there. Prefer Debug Win32 unless you change the project charset.

`examples/` does not load any of these files. Headless tools only need the numeric tables in `examples/maoliao_sim/`.
