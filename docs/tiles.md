# Tile atlas

Map records are `{x, y, id, xAmount, yAmount, u}`. `x`/`y`/`xAmount`/`yAmount`
are in tiles. `id` selects a row of `res/map.bmp` or a strip of
`res/scenery.bmp`. `u` is the friction coefficient written by `createMap`.

## Collision versus decoration

`hitMap` only walks records with `0 < id < 11`. Everything at 11 and above is
drawn and animated but is not solid.

| id | Name in the source comments | Solid? | Blit size | Source |
| --- | --- | --- | --- | --- |
| 1 | 草砖 grass brick | yes | 32×32 | `map.bmp` row 0 |
| 2 | 云朵 cloud | yes | 32×32 | `map.bmp` row 1 |
| 3 | 草砖地下 grass fill | yes | 32×32 | `map.bmp` row 2 |
| 4 | 雪地地下 snow fill | yes | 32×32 | `map.bmp` row 3 |
| 5 | 雪地地皮 snow cap | yes | 32×32 | `map.bmp` row 4 |
| 6 | 管道中间 pipe body | yes | 32×32 | `map.bmp` row 5 |
| 7 | 向下的管道口 / pipe shaft | yes | **64×32** | `map.bmp` row 6, special blit |
| 8 | 倒着的管道口 down-facing mouth | yes | **64×64** | `map.bmp` row 7 |
| 9 | 向上的管道口 (unused in v2 maps) | yes | 32×32 default | `map.bmp` row 8 |
| 10 | 向上的管道口 up-facing mouth | yes | **64×64** | `map.bmp` row 9 |
| 11 | 背景草 tuft | no | 96×64 animated | `scenery.bmp` strip 0 |
| 12 | 胜利牌子 flag | no | 96×64 animated | `scenery.bmp` strip 1 |
| 13 | 水流 water | no | 96×64 animated | `scenery.bmp` strip 2 |
| 14 | 树 tree | no | 96×64 animated | `scenery.bmp` strip 3 |

`Scene::show` branches:

```
id == 7        -> putimage 2*WIDTH × HEIGHT from map.bmp
id  8 or 10    -> putimage 2*WIDTH × 2*HEIGHT from map.bmp
id  > 7 else   -> two-pass scenery strip, 3*WIDTH × 2*HEIGHT
id  <= 7 else  -> putimage WIDTH × HEIGHT from map.bmp
```

So id 7 is a wide pipe shaft (world 3 columns). Ids 8 and 10 are the mouths
that `hitMap` also treats as 64×64. Ids 11–14 animate with `scenery_iframe`
cycling `1 → 2 → 1` at `F = TIME * 0.3` frames per tick.

## How a record is stamped

A record is not a single cell. Nested loops stamp `xAmount * yAmount` copies:

```
worldX = xMap + map.x * 32 + j * 32
worldY = yMap + map.y * 32 + k * 32
```

World 1’s opening ground `{0, 9, 1, 15, 1}` is fifteen grass-brick cells
along row 9 (y = 288 px). The fill underneath `{0, 10, 3, 15, 2}` is a 15×2
block of dirt.

World 2 builds stairs as four records that grow in `yAmount`:

```
{67, 9, 2, 1, 1}   one cloud
{68, 8, 2, 1, 2}   two clouds tall
{69, 7, 2, 1, 3}
{70, 6, 2, 1, 4}
```

## Friction by id

Copied from the `switch` in `Scene::createMap`. World 3 uses a tighter set
(ids 1–6 all get `T1`).

```
worlds 1 and 2:
  1, 3, 4, 5, 2  -> u = (V_MAX / T2) / G   ≈ 0.2222
  6              -> u = (V_MAX / T1) / G   ≈ 0.5333
  default        -> u = (V_MAX / T3) / G   ≈ 0.1778

world 3:
  1–6            -> u = (V_MAX / T1) / G   ≈ 0.5333
  2 (clouds)     -> u = (V_MAX / T2) / G   ≈ 0.2222
  default        -> u = (V_MAX / T3) / G   ≈ 0.1778
```

The landing probe in `Role::action` uses whatever tile `hitMap` returned, so
a cloud (id 2) in world 3 is both a safe (non-lethal) landing and a medium
friction surface.

## Sprite sheets that are not tiles

| File | Used for |
| --- | --- |
| `res/mapsky.bmp` | 512×(4×384) looping sky; band `world-1` |
| `res/home.bmp` | 512×(5×384) title / die / clear / credits strips |
| `res/role.bmp` | Cat walk (2 frames × 2 facings) + death + mask row |
| `res/ani.bmp` | Enemy, bullet, bomb, coin, score-pop, food |
| `res/scenery.bmp` | Ids 11–14 |

Hero walk frames pick `hero_iframe` 1 or 2 from
`(-(int)x0 + x) / STEP % 4` with `STEP = 10`. Facing chooses column 0–1
(right) or 3–4 (left). Death uses column 2.

## Authoring a new platform

1. Add a `Map` row to the world array in `createMap`. Stay under
   `MAP_NUMBER = 30` records (not 30 cells — a 37-wide ground is still one
   record).
2. Pick an id that is solid if the cat should stand on it.
3. Let the `switch` assign `u`, or set `u` yourself after the copy.
4. If you need a decorative-only object, use id 11–14 so `hitMap` skips it.
5. Place coins in tile coordinates and food in pixels. Enemies are tile
   coordinates multiplied by 32 in `createEnemy`.

The examples library exposes the same id → friction and id → cell-size tables
in `examples/include/maoliao/friction.hpp` and `collision.hpp`.
