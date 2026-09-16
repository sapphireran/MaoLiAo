# Assets

The game loads bitmaps and MP3s from a `res\` directory next to the working directory. Those files are **not** in this git tree (they lived on the original author's machine and in the VS output folder). The constructors still name them; a checkout without `res\` will show blank `IMAGE`s and silent MCI calls.

This page is a catalogue of what the **source** expects, so a future asset dump or a recreation has a shopping list.

## Window and icon

| Item | Source |
| --- | --- |
| Client size | `XSIZE=512`, `YSIZE=384` |
| Title | `猫里奥 V2.0` |
| Icon | `MaoLiAo/MaoLiAo.ico` via `MaoLiAo.rc` / `IDI_ICON1` |

Debug builds use the multi-byte charset (`CharacterSet` = `MultiByte` in the vcxproj). Release x86/x64 are set to Unicode — `outtextxy` with narrow string literals is a known mismatch if you ship Release as-is. See [known-quirks.md](known-quirks.md).

## Bitmaps

| Path | Loaded by | How it is used |
| --- | --- | --- |
| `res\mapsky.bmp` | `Scene` | Sky. Forced to `512 × 1536` (`XSIZE` by `4 * YSIZE`). Four stacked 384-pixel bands, one per world (world 4 unused). Looped horizontally. |
| `res\map.bmp` | `Scene` | 32×32 (and 64×64) terrain. Row `id - 1` is the tile. Id 7 blits two tiles wide; ids 8 and 10 blit 64×64. |
| `res\scenery.bmp` | `Scene` | Animated props. Frame width `3 * 32`. Vertical groups of `4 * 32` starting at `(id - 11) * 4 * HEIGHT`. Two-pass blit: mask row then color row. `scenery_iframe` cycles `1..2`. |
| `res\ani.bmp` | `Scene` + `Role` | Shared sheet for coins, food, enemies, bullets, bombs. |
| `res\role.bmp` | `Role`, `Control::showDied` | Hero walk (columns 0–1), death (column 2), mirrored set starting at column 3. Mask row at `y = HEIGHT`, color at `y = 0`. |
| `res\home.bmp` | `Control` | UI atlas. Forced to `512 × 1920` (`5 * YSIZE`). Page 0 title, page 1 game over (`y = -YSIZE`), page 2 death / “next level” (`y = -2*YSIZE`), page 3 all-clear (`y = -3*YSIZE`). |

### `ani.bmp` rows the code actually samples

Coordinates are top-left of the source rect, in pixels, `WIDTH=32` unless noted.

| Feature | Color row (`SRCPAINT`) | Mask row (`SRCAND`) | Frame count |
| --- | --- | --- | --- |
| Enemy walk | `y = 0` | `y = 32` | 2 (`enemy_iframe` 1..2) |
| Bullet | `y = 64` | `y = 96` | 2 |
| Bomb | `y = 128`, size 64×64 | `y = 192` | 4 |
| Coin | `y = 256` | `y = 288` | 4 |
| Score pop | `y = 320` | `y = 352` | 4 |
| Food / weapon | `y = 384`, size `52 × 25.6` | `y = 384 + 25.6` | 2 |

`Scene` and `Role` both `loadimage` the same path into different `IMAGE` objects.

### `role.bmp` layout

| Facing | Walk frames | Source x |
| --- | --- | --- |
| Right | 1, 2 | `0`, `32` |
| Death | one frame | `64` |
| Left | 1, 2 | `96`, `128` |

Walk frame picks from world-space position: `rolePos = -x0 + x`, then `rolePos / STEP` (`STEP = 10`) modulo 4 chooses iframe 1 or 2. Distance, not time, drives the gait.

## Audio (`mciSendString`)

Opened in `main`:

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | `res\背景音乐.mp3` | Loop during play |
| `music_win` | `res\胜利.mp3` | Any clear |
| `music_passedAll` | `res\通关.mp3` | After world 3 |
| `music_end` | `res\游戏结束.mp3` | Lives exhausted |

Opened in `Role::Role`:

| Alias | File | When |
| --- | --- | --- |
| `music_died` | `res\死亡1.mp3` | Fall or enemy contact |
| `music_jump` | `res\跳.mp3` | Every takeoff (including flaps) |
| `music_coin` | `res\金币.mp3` | Coin pickup |
| `music_tread` | `res\踩敌人.mp3` | Stomp |
| `music_getWeapon` | `res\吃到武器.mp3` | Food |
| `music_bullet` | `res\子弹.mp3` | Shot fired |
| `music_boom` | `res\子弹撞墙.mp3` | Bullet vs tile / max range |
| `music_boom2` | `res\子弹打到敌人.mp3` | Bullet vs enemy |

Commands used: `open … alias …`, `play … repeat`, `play … from 0`, `stop …`, `close all` (unreachable — the play loop never exits). `#pragma comment(lib, "Winmm.lib")` sits in both `main.cpp` and `role.cpp`.

## Transparent blit

EasyX has no built-in alpha on these code paths. Every sprite is two `putimage` calls:

1. `SRCAND` with the mask (black silhouette on white).
2. `SRCPAINT` with the color (sprite on black).

That is the classic 1-bit color-key pair. The portable examples do not blit; they only document the rectangles.

## Save file

`gameRecord.dat` is listed under `MaoLiAo/` in git. It is a one-integer text file, not an asset sheet. See [input-and-save.md](input-and-save.md).

## Recreating a `res` folder

If you are rebuilding art for a personal fork:

1. Keep every sheet on a 32-pixel grid unless a comment says otherwise (food and bombs are the exceptions).
2. Put mask rows immediately below (or at the documented offset from) color rows.
3. `home.bmp` and `mapsky.bmp` are vertical atlases; page height is exactly `YSIZE`.
4. MP3 aliases are Chinese filenames; MCI needs the exact bytes the `open` string uses (GBK on a Chinese Windows locale).

The examples never open these files. They reconstruct rules, not pixels.
