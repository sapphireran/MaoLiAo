# Assets

Everything the exe loads is under `MaoLiAo/res/`. Paths in source use backslashes (`res\\map.bmp`). Names below are the on-disk files (Chinese MP3s are UTF-8 in git).

## Bitmaps

| File | Used by | Layout |
| --- | --- | --- |
| `mapsky.bmp` | `Scene` | Loaded as 512×1536 (`XSIZE` × `4*YSIZE`). World *n* draws at `yBg = -(n-1)*384`, so worlds 1–3 are three stacked skies; the fourth band is unused. Horizontally tiled when `xBg` wraps. |
| `map.bmp` | `Scene` | Terrain sheet. Row `id-1` is a 32×32 cell. IDs 7–8/10 use 2× width (and 2× height for 8/10). |
| `scenery.bmp` | `Scene` | Decorative IDs 11–14. Two frames, mask row then color row, 96×64. `scenery_iframe` oscillates 1→2. |
| `ani.bmp` | `Scene` + `Role` | Shared sprite sheet: enemies (rows 0–1), bullets (2–3), explosions (4–6), coins (8–9), score pop (10–11), flower (12). |
| `role.bmp` | `Role` + death UI | Walk frames 0–1 facing right, death at column 2, mirrored walk at columns 3–4. Mask in the lower row (`SRCAND` + `SRCPAINT`). |
| `home.bmp` | `Control` | Loaded as 512×1920 (`5*YSIZE`). Slice 0 title; −1 game over; −2 level-clear / death backdrop; −3 all-clear. Slice 4 unused. |
| `MaoLiAo.ico` | `MaoLiAo.rc` `IDI_ICON1` | Window icon. |

`loadimage` in `Scene` / `Role` / `Control` happens in constructors. Rebuilding those objects on death re-reads the BMPs from disk every life.

## MCI aliases

Opened in `main.cpp` (loop-level) or `Role::Role` (per-life). `play … from 0` rewinds.

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | `res\背景音乐.mp3` | Loop during play; stopped on death / clear / title |
| `music_win` | `res\胜利.mp3` | Any world clear |
| `music_passedAll` | `res\通关.mp3` | After world 3, before the all-clear still |
| `music_end` | `res\游戏结束.mp3` | `life == 0` |
| `music_died` | `res\死亡1.mp3` | Hit / fall |
| `music_jump` | `res\跳.mp3` | Every accepted jump |
| `music_coin` | `res\金币.mp3` | Coin pickup |
| `music_tread` | `res\踩敌人.mp3` | Stomp |
| `music_getWeapon` | `res\吃到武器.mp3` | Flower |
| `music_bullet` | `res\子弹.mp3` | Shot |
| `music_boom` | `res\子弹撞墙.mp3` | Bullet vs tile / max distance |
| `music_boom2` | `res\子弹打到敌人.mp3` | Bullet vs enemy |

There is no volume control and no fade. `close all` is after the infinite loop, so it never runs.

## Fonts the UI asks for

`Gill Sans` (unused title path), `黑体` (menus), `Cooper` (HUD), `Goudy Stout` (death). Missing faces fall back to whatever EasyX/`CreateFont` does on that Windows install. Hover English strings are ASCII and do not need 黑体.

## What is *not* in git as data

Level geometry is C++ literals, not TMX / JSON the game loads. `fixtures/` is documentation that happens to be executable, not a runtime pack.
