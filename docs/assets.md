# Assets

All runtime art and audio live in `MaoLiAo/res/`. Paths in source are Windows-style (`res\\map.bmp`). Dimensions below come from the files on disk plus the blit rectangles in `scene.cpp` / `role.cpp` / `control.cpp`.

## Bitmaps

| File | Pixels | Used as |
| --- | ---: | --- |
| `role.bmp` | 160 × 64 | Hero walk (2 frames × 2 facings) + death |
| `ani.bmp` | 256 × 434 | Enemies, bullets, explosions, coins, score puffs, food |
| `map.bmp` | 64 × 352 | 2 × 11 tiles of terrain (ids 1–11 conceptually; id 11 is scenery) |
| `scenery.bmp` | 192 × 576 | Animated grass / plaque / water / trees |
| `mapsky.bmp` | 512 × 1536 | Four stacked skies, one per world index (world 1 at y=0) |
| `home.bmp` | 512 × 1920 | Five stacked UI screens |

Color-keying is the classic EasyX pair: `SRCAND` of a mask row, then `SRCPAINT` of the color row.

### `role.bmp` frames

32×32 cells, two rows (color on row 0, mask on row 1):

| Cell x | Facing / pose |
| ---: | --- |
| 0, 32 | Walk right, frames 1 and 2 |
| 64 | Death (used on the field and on the remaining-life card) |
| 96, 128 | Walk left, frames 1 and 2 |

Walk frame index is `rolePos / STEP % 4` with `STEP = 10` (`hero_iframe` 1 or 2).

### `ani.bmp` rows (32 px)

| Row (y / 32) | Content |
| ---: | --- |
| 0 / 1 | Enemy color / mask, 2 frames |
| 2 / 3 | Bullet color / mask, 2 frames |
| 4–7 | Explosion, 64×64, 4 frames |
| 8 / 9 | Coin color / mask, 4 frames |
| 10 / 11 | “+score” puff color / mask |
| 12 | Food / weapon, ~52×26, 2 frames |

Coin animation: `coin_iframe += TIME * 7`, wrap at 5. Enemy: `TIME * 5`, wrap at 3. Explosion: `TIME * 10`, wrap at 5 then free the slot.

### `map.bmp`

64 px wide (two 32 px columns; the second column is unused for ids 1–6). 352 / 32 = 11 rows. `putimage` source y is `(id - 1) * HEIGHT`. Id 7 blits 64×32; ids 8 and 10 blit 64×64.

### `scenery.bmp`

For `id > 7` except 8 and 10:

```
y0 = (id - 11) * 4 * HEIGHT
frame w = 3 * WIDTH (96)
frame h = 2 * HEIGHT (64)
frame x = (scenery_iframe - 1) * 96
```

`scenery_iframe` walks 1 → 2 and wraps, stepped by `F` (`TIME * 0.3`).

### Sky and home strips

`mapsky.bmp` height 1536 = 4 × 384. `yBg = -(world - 1) * 384`, so world 3 uses the third band. The fourth band is unused by the loop.

`home.bmp` height 1920 = 5 × 384. See [controls-and-save.md](controls-and-save.md) for which negative y is Game Over vs all-clear.

## Audio (MP3, MCI aliases)

Opened with `mciSendString("open res\\….mp3 alias …")`. Chinese filenames are part of the original asset set.

| File | Alias | Where |
| --- | --- | --- |
| `背景音乐.mp3` | `music_bg` | Loop in `main` |
| `胜利.mp3` | `music_win` | Stage clear |
| `通关.mp3` | `music_passedAll` | After the last stage |
| `游戏结束.mp3` | `music_end` | Zero lives |
| `死亡1.mp3` | `music_died` | Fall or side-hit |
| `跳.mp3` | `music_jump` | Jump |
| `金币.mp3` | `music_coin` | Coin |
| `踩敌人.mp3` | `music_tread` | Stomp |
| `吃到武器.mp3` | `music_getWeapon` | Food |
| `子弹.mp3` | `music_bullet` | Fire |
| `子弹撞墙.mp3` | `music_boom` | Bullet vs tile / range |
| `子弹打到敌人.mp3` | `music_boom2` | Bullet vs enemy |

Bit rates on disk are 56–96 kbps, 22.05 kHz. BGM is mono; most SFX are joint stereo.

## Icon

`MaoLiAo/MaoLiAo.ico` is compiled through `MaoLiAo.rc`. Window title is `"猫里奥 V2.0"`.
