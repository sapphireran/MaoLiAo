# Sprites and audio

All BMP paths are `res\\*.bmp` with EasyX `loadimage`. Transparent blits are
the classic two-pass:

```
putimage(..., SRCAND);    // mask, usually the lower strip
putimage(..., SRCPAINT);  // color, usually the upper strip
```

Iframe counters live on the objects and tick with `TIME * n`.

## `role.bmp` (hero)

32×32 cells. `Role::show` picks a walk frame from world-x:

```
rolePos = -x0 + x
if rolePos/STEP % 2 == 0 and % 4 != 0 → iframe 2
else if rolePos/STEP % 4 == 0         → iframe 1
```

| Destination | Source (color / mask) |
| --- | --- |
| Face right, walk | `(iframe-1)*32, 0` / `…, 32` |
| Face left, walk | `(iframe-1)*32 + 96, 0` / `…, 32` |
| Dead | `(64, 0)` / `(64, 32)` |

`turn` is 1 (right) or -1 (left), copied from `direction.x` when that is
nonzero so a standing cat keeps the last facing.

## `ani.bmp` (enemies, coins, food, bullets, bombs)

Shared sheet. Rows below are in 32-px units as used in `putimage`:

| Thing | Color row (y0) | Mask row | Frame count | Speed |
| --- | --- | --- | --- | --- |
| Enemy walk | 0 | 1 × HEIGHT | 2 (iframe 1..2) | `TIME * 5` |
| Bullet | 2 × HEIGHT | 3 × HEIGHT | 2 | `TIME * 10` |
| Bomb | 4 × HEIGHT, 64×64 | 6 × HEIGHT | 4 | `TIME * 10` |
| Coin | 8 × HEIGHT | 9 × HEIGHT | 4 | `TIME * 7` |
| Score sparkle | 10 × HEIGHT | 11 × HEIGHT | 4 | `TIME * 8` |
| Food / weapon | 12 × HEIGHT, 52×25 | + 4/5 HEIGHT | 2 | `TIME * 7` |

Bomb blit is centered (`x - WIDTH/2`, `y - HEIGHT/2`) and offset by `x0`.
Bullets are drawn in *screen* space (they already store screen x).

## `map.bmp` and `scenery.bmp`

Solid tiles: `putimage(x, y, 32, 32, &img_map, 0, (id-1)*32)` except:

- id 7: 64×32 (pipe shaft)
- id 8 and 10: 64×64 (pipe mouths)

Scenery ids 11–14: 96×64 from `scenery.bmp`, two-frame swing
(`scenery_iframe` 1..2, `+= F` which is `TIME*0.3`). Source y is
`(id-11)*4*HEIGHT`.

## `mapsky.bmp` and `home.bmp`

Sky is loaded scaled to `XSIZE × 4*YSIZE` (four stacked worlds; only three
are used). Home / interstitials are `XSIZE × 5*YSIZE`. See
[input-and-ui.md](input-and-ui.md) for which negative `y` is which screen.

## MCI aliases

Opened once (never closed in practice). `play … from 0` rewinds.

| Alias | File | Who opens | When |
| --- | --- | --- | --- |
| `music_bg` | `背景音乐.mp3` | `main` | Loop during play |
| `music_win` | `胜利.mp3` | `main` | Stage clear |
| `music_passedAll` | `通关.mp3` | `main` | After the win sting on world 3 |
| `music_end` | `游戏结束.mp3` | `main` | Lives == 0 |
| `music_died` | `死亡1.mp3` | `Role` | Death |
| `music_jump` | `跳.mp3` | `Role` | Jump |
| `music_coin` | `金币.mp3` | `Role` | Coin |
| `music_tread` | `踩敌人.mp3` | `Role` | Stomp |
| `music_getWeapon` | `吃到武器.mp3` | `Role` | Food |
| `music_bullet` | `子弹.mp3` | `Role` | Fire |
| `music_boom` | `子弹撞墙.mp3` | `Role` | Bullet vs tile / max range |
| `music_boom2` | `子弹打到敌人.mp3` | `Role` | Bullet vs enemy |

Python cannot play these here; `examples.maoliao_lab.sprites` only catalogs
the sheet math.
