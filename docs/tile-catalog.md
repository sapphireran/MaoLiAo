# Tile catalog

Sprites are 32×32 unless noted. `Scene::show` samples `map.bmp` at `(0, (id-1)*HEIGHT)` for ids 1–7. Ids ≥ 8 take larger blits; ids ≥ 11 switch to `scenery.bmp` with a 2-frame animation.

Collision: `hitMap` only considers `1 ≤ id ≤ 10`.

| id | Name (from comments in `createMap`) | Collides | Draw | Friction bucket (worlds 1–2) |
| --- | --- | --- | --- | --- |
| 1 | 草砖 grass surface | yes | 32×32 `map.bmp` row 0 | T2 |
| 2 | 云朵 cloud | yes | 32×32 row 1 | T2 |
| 3 | 草砖地下 / desert underground | yes | row 2 | T2 |
| 4 | 雪地地下 snow underground | yes | row 3 | T2 |
| 5 | 雪地地皮 snow surface | yes | row 4 | T2 |
| 6 | 管道中间 pipe body (also used as a high-friction floor in world 1) | yes | row 5 | **T1** |
| 7 | 向下的管道口 / pipe extension (world 3 column) | yes | **64×32** (`2*WIDTH` × `HEIGHT`) | T3 |
| 8 | inverted pipe mouth | yes | **64×64**; amount uses 2× tile size in `hitMap` | T3 |
| 9 | (reserved in comments: 向下的管道口 pair) | yes | 64×64 if `id > 7` | T3 |
| 10 | 向上的管道口 upward mouth | yes | 64×64; 2× amount in `hitMap` | T3 |
| 11 | 背景草 background grass | no | `scenery.bmp` strip 0, 96×64, 2 frames | n/a (no collide) |
| 12 | 胜利牌子 goal sign | no | scenery strip 1 | n/a |
| 13 | 水流 water | no | scenery strip 2 | n/a |
| 14 | 树 tree | no | scenery strip 3 | n/a |

World 3 overrides the friction switch: ids 1–6 use **T1**, id 2 uses T2, else T3. Pipe mouths therefore get T3 unless they fall through `default`.

## `map.bmp` rows

Row index = `id - 1`. The world-3 blit for id 7 uses width `2*WIDTH` from that row (two tiles side by side on the sheet, or a double-wide pipe graphic). Ids 8 and 10 blit `2*WIDTH × 2*HEIGHT`.

## `scenery.bmp`

For `id > 7` and not 8/10:

```
y0 = (id - 11) * 4 * HEIGHT
frame = (int)scenery_iframe - 1     // 0 or 1
AND  blit: (frame * 3*WIDTH, y0 + 2*HEIGHT)
OR   blit: (frame * 3*WIDTH, y0)
size: 3*WIDTH × 2*HEIGHT  (96×64)
```

`scenery_iframe` advances by `F` (`TIME*0.3`) and wraps at 3 → 1, so the animation is two frames.

## `ani.bmp` (shared by coins, food, enemies, bullets, bombs)

| Use | AND row | OR / color row | Frame width |
| --- | --- | --- | --- |
| Enemy walk | `HEIGHT` (row 1) | row 0 | 32, 2 frames |
| Bullet | `3*HEIGHT` | `2*HEIGHT` | 32, 2 frames |
| Bomb | `6*HEIGHT`, size 64×64 | `4*HEIGHT` | 64, 4 frames |
| Coin | `9*HEIGHT` | `8*HEIGHT` | 32, 4 frames |
| Coin sparkle | `11*HEIGHT` | `10*HEIGHT` | 32, 4 frames |
| Food | `12*HEIGHT + 4*HEIGHT/5` | `12*HEIGHT` | `3*WIDTH/2+4` = 52, 2 frames |

`role.bmp` (hero):

| State | AND | OR |
| --- | --- | --- |
| Walk right, frame i=1,2 | `(i-1)*32, HEIGHT` | `(i-1)*32, 0` |
| Walk left | that x plus `3*WIDTH` | same |
| Dead | `2*WIDTH, HEIGHT` | `2*WIDTH, 0` |

Walk frame picks `hero_iframe` 1 or 2 from `(-x0 + x) / STEP % 4` with `STEP = 10`.

## Sky

`mapsky.bmp` is loaded as `XSIZE × 4*YSIZE` (512×1536). `yBg = -(world-1)*YSIZE` selects strip 0, 1, or 2. `xBg` wraps at `-img_bg.getwidth()` and a second copy is drawn to the right for scrolling.
