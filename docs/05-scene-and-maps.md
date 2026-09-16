# 05 — Scene and maps

`Scene` owns the static world: sky, tile runs, coins, mushrooms, and the short-lived sparkle slots used when a coin is collected.

## Construction

```text
load mapsky.bmp, map.bmp, scenery.bmp, ani.bmp
zero map[] / coins[] / score[] / food[]
world = W
createCoin(); createFood(); createMap(W)
```

`xBg, yBg` start at 0. `xMap, yMap` start at 0. Frame counters for scenery / coins / food start at 1.

## Tile run (`struct Map`)

```text
x, y       tile column / row of the run’s top-left
id         which 32-px strip in map.bmp (1-based) or scenery kind
xAmount    how many cells across
yAmount    how many cells down
u          friction coefficient (see physics)
```

A run is a filled rectangle of identical cells. Drawing nested-loops `j < xAmount`, `k < yAmount` and blits `map.bmp` at source `(0, (id-1)*HEIGHT)` for ordinary ids.

### ID catalog (from comments in `createMap`)

| id | Art | Solid? | Notes |
| --- | --- | --- | --- |
| 1 | Grass brick | yes | World 1 ground |
| 2 | Cloud | yes | Also used as safe tiles in world 3 |
| 3 | Grass / desert underground | yes | Filler under id 1 |
| 4 | Snow underground | yes | World 2 filler |
| 5 | Snow surface | yes | World 1 mid-stretch and world 2 walkway |
| 6 | Pipe middle | yes | High friction (T1) |
| 7 | Downward pipe shaft | yes | World 3, drawn 64×32 |
| 8 | Downward pipe mouth | yes | World 3, 64×64 hit + blit |
| 9 | Upward pipe mouth (unused in data) | yes | — |
| 10 | Upward pipe mouth | yes | 64×64 |
| 11 | Background grass | no | Animated scenery |
| 12 | Victory sign | no | End of stage art |
| 13 | Water | no | Animated |
| 14 | Tree | no | Animated |

`show()` treats `id > 7` as “special”: 8 and 10 come from `map.bmp` at 2×2 tiles; 11–14 come from `scenery.bmp` as 3×2 tiles with a 2-frame sway (`scenery_iframe` ping-pongs 1→2).

`id == 7` is blitted 2 tiles wide even though the hit box uses 1×`WIDTH` unless `xAmount` says otherwise. World 3 shafts therefore look thicker than they collide.

## Camera and parallax

`Scene::action`:

```text
xMap = (int)hero.x0
bgStep = |vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG
if hero.x == XRIGHT and vX > 0 and xMap changed:
    xBg -= bgStep
```

Sky only eases when the cat is pinned to the right edge and the world origin actually moved. `K_MAP_BG = 5` makes the sky crawl at 1/5 of the “feel” speed.

`show()` wraps the sky:

```text
if xBg <= -img_bg.width: xBg = 0
yBg = -(world - 1) * YSIZE
blit(xBg, yBg); blit(width + xBg, yBg)
```

`mapsky.bmp` is loaded as `XSIZE × 4*YSIZE` (512×1536). Each world uses a different 384-px horizontal band: world 1 the top, world 2 the second, world 3 the third. That is why the sky tint changes per stage without extra files.

## Coins and sparkles

`createCoin` writes tile coordinates. `show` blits `ani.bmp` rows 8 (color) / 9 (mask), 4 frames, advanced by `TIME * 7`.

`setScorePos` finds an empty `score[]` slot. Those slots play rows 10/11 for 4 frames (`TIME * 8`) and then clear.

World 3 coins pick `random(3, 7)` for seven x-positions `{5,15,...,65}`.

## Food (mushrooms)

One mushroom per world in the shipped data.

| World | Pixel position |
| --- | --- |
| 1 | `(14*WIDTH, 5*HEIGHT)` = (448, 160) |
| 2 | `(39*WIDTH - WIDTH/3, 3*HEIGHT + HEIGHT/5)` ≈ (1237, 102) |
| 3 | `(10, 10)` — almost the top-left; easy to miss |

Art is `ani.bmp` row 12, 2 frames, cell size 52×25.

## End of stage

`isEnding(distance)` with `distance = -hero.x0 + hero.x` (world x of the cat):

| World | Threshold |
| --- | --- |
| 1 | `94 * 32` = 3008 |
| 2 | `104 * 32` = 3328 |
| 3 | `94 * 32` = 3008 |

Past that, `Role` sets `ending` and forces right-walk. The victory sign (`id 12`) is decoration sitting near those x-values (world 1 at column 101, world 2 at 111, world 3 at 101).
