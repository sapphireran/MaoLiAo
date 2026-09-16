# Resources

All gameplay art and audio live under `MaoLiAo/res/`. Paths in the sources are Windows-style (`res\\home.bmp`, `res\\背景音乐.mp3`) and are resolved from the process working directory.

## Bitmaps

| File | Loaded as | How it is used |
| --- | --- | --- |
| `role.bmp` | `Role::img_hero` | Walk (2 frames × 2 facings) and death. Color row on top, mask row under it. Cell 32×32. |
| `ani.bmp` | enemy, bomb, bullet, coin, food | Shared sheet. Enemies: rows 0–1. Bullets: rows 2–3. Bombs: rows 4–7 at 64×64. Coins: rows 8–9. Coin sparkle: rows 10–11. Food: row 12, cell 52×25. |
| `map.bmp` | `Scene::img_map` | Tile IDs 1–10 stacked vertically, 32 px tall each. IDs 7–10 are blitted wider (pipe). |
| `scenery.bmp` | `Scene::img_scenery` | IDs 11–14. Each decoration is 96×64 with a mask row, two animation frames side by side. |
| `mapsky.bmp` | `Scene::img_bg` | Forced to `512 × 1536` (4 windows tall). World `n` uses slice `y = -(n-1)*384`. |
| `home.bmp` | `Control::img_bg` | Forced to `512 × 1920` (5 windows tall). Slices pick the title, game over, mid-clear, and credits screens. |

`MaoLiAo/MaoLiAo.ico` / `IDI_ICON1` is the Win32 window icon only. It is not a gameplay sprite.

## `home.bmp` slices

`putimage(0, -k * YSIZE, &img)` reveals window `k`:

| k | Call site | Screen |
| --- | --- | --- |
| 0 | `gameStart` | Title |
| 1 | `showGameOver` | Game over (held 6.5 s) |
| 2 | `showPassed`, `showDied` | Next-level card / remaining lives |
| 3 | `showPassedAll` | All-clear card (held 7.8 s) |
| 4 | (unused in code) | Extra padding in the 5-window file |

## `mapsky.bmp` slices

| World | `yBg` | Sky |
| --- | --- | --- |
| 1 | 0 | Day grassland |
| 2 | -384 | Next band in the strip |
| 3 | -768 | Next band |
| wrap | when `xBg <= -width`, `xBg = 0` | Horizontal wrap; a second blit is drawn at `width + xBg` |

Parallax math is in [physics.md](physics.md). The sky never moves on its own; `Scene::action` pushes it when the hero is camera-locked and running right.

## Hero frames (`role.bmp`)

`Role::show` picks a walk frame from world-space position `(-x0 + x) / STEP % 4` with `STEP = 10`:

| `pos/10 % 4` | `hero_iframe` |
| --- | --- |
| 0 | 1 |
| 1 | 2 |
| 2 | 2 |
| 3 | 1 |

Facing `turn == 1` uses columns 0–1. Facing `-1` uses columns 3–4 (`iframe + 3`). Death uses column 2. Each `putimage` pair is mask (`SRCAND`, source y = 32) then color (`SRCPAINT`, source y = 0).

## Animation rates

All of these are advanced during `show()`, so pausing the draw also freezes the sheets.

| Actor | Advance per tick | Loop |
| --- | --- | --- |
| Enemy | `TIME * 5` = 0.05 | frames 1–2 |
| Bomb | `TIME * 10` = 0.10 | frames 1–4, then slot cleared |
| Bullet | `TIME * 10` = 0.10 | frames 1–2 |
| Coin | `TIME * 7` = 0.07 | frames 1–4 |
| Coin sparkle | `TIME * 8` = 0.08 | frames 1–4, then slot cleared |
| Food | `TIME * 7` = 0.07 | frames 1–2 |
| Scenery | `F` = `TIME * 0.3` (see quirks) | frames 1–2 |

## MCI aliases

`#pragma comment(lib, "Winmm.lib")` is in both `main.cpp` and `role.cpp`. Aliases are opened with `mciSendString("open res\\….mp3 alias …")`.

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | `背景音乐.mp3` | Loop during play |
| `music_win` | `胜利.mp3` | Any clear |
| `music_passedAll` | `通关.mp3` | World 3 clear |
| `music_end` | `游戏结束.mp3` | Lives exhausted |
| `music_died` | `死亡1.mp3` | Hero death |
| `music_jump` | `跳.mp3` | Jump |
| `music_coin` | `金币.mp3` | Coin |
| `music_tread` | `踩敌人.mp3` | Stomp |
| `music_getWeapon` | `吃到武器.mp3` | Food |
| `music_bullet` | `子弹.mp3` | Shot fired |
| `music_boom` | `子弹撞墙.mp3` | Shot hit wall / max distance |
| `music_boom2` | `子弹打到敌人.mp3` | Shot hit enemy |

`play … from 0` restarts a sting. `play music_bg repeat` loops. `stop music_bg` is issued before death, clear, and title so the stings are audible.

Filenames are Chinese. A checkout on a locale that mangles those names will open empty aliases and stay silent; the rest of the game still runs.

## What the examples do not load

The portable suite never opens a BMP or MP3. It only needs the numeric tables and the kinematic / AABB code. Opening the sheets is a Windows / EasyX concern and is documented here so a later art pass knows which rows are spoken for.
