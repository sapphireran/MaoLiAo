# Assets

All paths are relative to the process working directory. The 2020
loader uses backslashes (`"res\\role.bmp"`). Files live in
`MaoLiAo/res/`.

## Bitmaps

Sizes from the files on disk (Windows 3.x 24-bit BMP):

| File | Pixels | Used as |
| --- | --- | --- |
| `role.bmp` | 160 × 64 | hero walk / death, mask + color |
| `ani.bmp` | 256 × 434 | enemies, bullets, bombs, coins, food |
| `map.bmp` | 64 × 352 | 32×32 tile column, 11 rows |
| `scenery.bmp` | 192 × 576 | 96×64 decorations, 2 frames × 3 rows × mask |
| `mapsky.bmp` | 512 × 1536 | 3 stacked 512×384 skies |
| `home.bmp` | 512 × 1920 | 5 stacked 512×384 UI pages |

`MaoLiAo/MaoLiAo.ico` is the Win32 icon (`IDI_ICON1` in `MaoLiAo.rc`).

### `role.bmp`

Two rows of five 32×32 cells. Row 0 is color, row 1 is the AND mask
(`SRCAND` then `SRCPAINT`, EasyX's usual color-key pair).

| Cell x | Facing | Use |
| --- | --- | --- |
| 0 | right | walk frame A (`hero_iframe == 1`) |
| 1 | right | walk frame B (`hero_iframe == 2`) |
| 2 | — | death pose (also the "lives left" icon) |
| 3 | left | walk frame A |
| 4 | left | walk frame B |

`hero_iframe` flips from world-x:

```
rolePos = -(int)x0 + x
if rolePos/10 % 2 == 0 && rolePos/10 % 4 != 0  -> frame 2
if rolePos/10 % 4 == 0                         -> frame 1
```

`STEP` is 10 px. Every other 10-px bucket uses frame 2; multiples of
40 px use frame 1. Odd buckets keep the previous frame.

### `map.bmp`

A vertical strip. Row `id - 1` is the tile for that id. Ids 7 and 8/10
are blitted at 64×32 or 64×64 (`2 * WIDTH` wide).

### `scenery.bmp`

For `id > 10`, `Scene::show` treats the sheet as:

```
y0 = (id - 11) * 4 * HEIGHT
frame = (int)scenery_iframe - 1          // 0 or 1
blit 3*WIDTH x 2*HEIGHT
  mask at (frame * 96, y0 + 64)
  color at (frame * 96, y0)
```

So id 11 = row 0 (grass), id 12 = row 1 (sign), id 13 = row 2 (water),
id 14 = row 3 (tree). `scenery_iframe` advances by `F` (`TIME * 0.3`)
and wraps at 3, but `(int)iframe` is only 1 or 2 because it resets at
`>= 3`.

### `ani.bmp`

Several bands share this sheet. Offsets used in source:

| Band (y in tiles) | Drawn by | Frames |
| --- | --- | --- |
| 0 / 1 | enemy walk (color / mask) | 2, wrap at iframe==3 |
| 2 / 3 | bullet | 2 |
| 4 / 6 | bomb (64×64, color / mask) | 4, wrap at 5 |
| 8 / 9 | coin | 4, wrap at 5 |
| 10 / 11 | coin-pickup flash | 4 |
| 12 | food / weapon (48+4 × 25.6) | 2 |

`food_iframe` wraps at 3. The food blit width is `3*WIDTH/2 + 4` = 52 px.

### `mapsky.bmp` / `home.bmp`

Sky: `yBg = -(world - 1) * 384`. World 1 uses [0, 384), world 2
[384, 768), world 3 [768, 1152). The bitmap is 1536 tall, so a fourth
unused band exists.

Home slices (`putimage(0, -n * YSIZE)`):

| n | Screen |
| --- | --- |
| 0 | title / menu |
| 1 | game over |
| 2 | world clear / remaining lives |
| 3 | all-clear |
| 4 | unused spare |

## Audio (MCI)

Opened as aliases. Chinese file names are the on-disk names.

| Alias | File | Where opened |
| --- | --- | --- |
| `music_bg` | `背景音乐.mp3` | `main` |
| `music_win` | `胜利.mp3` | `main` |
| `music_passedAll` | `通关.mp3` | `main` |
| `music_end` | `游戏结束.mp3` | `main` |
| `music_died` | `死亡1.mp3` | `Role` ctor |
| `music_jump` | `跳.mp3` | `Role` ctor |
| `music_coin` | `金币.mp3` | `Role` ctor |
| `music_tread` | `踩敌人.mp3` | `Role` ctor |
| `music_getWeapon` | `吃到武器.mp3` | `Role` ctor |
| `music_bullet` | `子弹.mp3` | `Role` ctor |
| `music_boom` | `子弹撞墙.mp3` | `Role` ctor |
| `music_boom2` | `子弹打到敌人.mp3` | `Role` ctor |

Most SFX are MPEG ADTS layer III, 96 kbps, 22.05 kHz joint stereo.
`背景音乐.mp3` and `胜利.mp3` are 56 kbps mono. `main` links
`Winmm.lib` for `mciSendString`.

## Working directory

Visual Studio must start the exe with cwd = `MaoLiAo/` (or a folder
that contains `res\`). A cwd of the repo root will fail every
`loadimage` and `mciSendString("open res\\...")`.
