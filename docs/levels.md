# Levels

`Scene::createMap` / `createCoin` / `createFood` and `Role::createEnemy` are giant initializer lists keyed on `world`.

## Tile IDs

Drawn from `res\map.bmp` (ids 1–10) or `res\scenery.bmp` (ids 11–14). Solid for `hitMap` only if `1 <= id <= 10`.

| id | Art | Solid | Notes |
| ---: | --- | --- | --- |
| 1 | grass top | yes | world 1 start / end strips |
| 2 | cloud | yes | world 2 platforms; **safe** in world 3 even without the star |
| 3 | dirt under grass | yes | stacked under id 1 |
| 4 | snow dirt | yes | under the snow strip |
| 5 | snow top | yes | long mid-stage in world 1 |
| 6 | ice / pipe body (world 1 strip) | yes | high friction (`T1`) |
| 7 | pipe extender | yes | world 3 vertical shafts; drawn 64×32 |
| 8 | inverted pipe mouth | yes | world 3; hit box 64×64 per amount |
| 9 | (unused in current lists) | yes | sheet row exists |
| 10 | upright pipe mouth | yes | world 1 decorations + world 3 floors; 64×64 hit |
| 11 | background grass | no | animated scenery |
| 12 | goal sign | no | near each finish |
| 13 | water | no | animated |
| 14 | tree | no | world 2 |

Friction assignment is in [physics.md](physics.md).

## Ending distances

`Scene::isEnding(distance)` uses **world-space** pixels (`-x0 + hero.x`):

| World | Threshold | Tiles (÷32) |
| ---: | ---: | ---: |
| 1 | 94 × 32 = 3008 | 94 |
| 2 | 104 × 32 = 3328 | 104 |
| 3 | 94 × 32 = 3008 | 94 |

After that the hero auto-runs until `screen x > 512`.

## World 1 — overworld

Ground in three materials: grass, a long snow runway, a short ice patch (`id 6` at x=67, width 4), then grass again. Gaps at tiles 15–17 and 71–73 / 75–77 are water scenery (id 13) with no solid — falling in is death.

Clouds (id 2) at (10,6), (40,4), (62,6), (65,4) are extra floors (and coin perches).

Pipes (id 10) at x = 36, 45, 80, 90 (grid y = 7). Goal sign at (101, 7).

**Coins** (grid): 10–13 @ y=5 (on the first cloud), 66–69 @ y=3, a vertical stack at x=74 y=5..8, then a sawtooth 82–89.

**Weapon**: one food at pixel `(14*32, 5*32)`.

**Enemies** (grid, turn): (3,8,+), (18,7,−), (25,7,+), (28,8,−), (33,8,−), (39,8,+), (68,3,+), (66,8,−), (81,6,+), (92,6,+).

## World 2 — sky / snow

Most of the floor is id 5 (snow) with id 4 dirt under it, broken into islands (0–14, 38–47, singles at 50/53/56, 61–70, 75, 82–92). Cloud stepping stones climb from (18,9) up to (25,3) and a late staircase 67–70. Goal sign at (111, 1) — high.

Trees (id 14) at x = 4, 9, 83, 89.

**Coins**: 25–27 @ y=2, a vertical column at x=32 y=4..8, then 50/53/56 @ y=6.

**Weapon**: `(39*32 - 32/3, 3*32 + 32/5)` ≈ (1237, 102).

**Enemies**: (18,8,+), (24,5,+), (28,5,−), (44,9,+), (97,6,+), (99,4,+).

Sky strip: `yBg = -YSIZE` (second band of `mapsky.bmp`).

## World 3 — pipe gauntlet

`createMap` rolls `height[i] = random(1, 7)` and `x[i] = 10 + 10*i` for i = 0..9, then **emits only columns 0..6** (7–9 are commented out). Each column is four tiles:

```
inverted mouth  id 8 at (x, height-2)
upper shaft     id 7 at (x, 0)      height max(0, height-2)
upright mouth   id 10 at (x, 4+height)
lower shaft     id 7 at (x, height+6)  height max(0, 6-height)
```

A long cloud runway `{80, 6, id 2, 25 wide}` and a goal sign at (101, 4) finish the stage.

`createCoin` places seven coins at x = 5,15,...,65 with independent random y in `[3, 6]`.

**Weapon**: `(10, 10)` pixels — almost at the top-left of the world. Easy to miss; without it, every pipe is lethal.

**Enemies**: (24,4,−), (34,6,+), (37,3,−), (43,5,−), (63,7,+), (67,8,−), (86,5,+).

Jump is always allowed. Combined with random pipe gaps this is the “仿 flappy bird” stage mentioned in `git log`.

`random(a,b)` is `rand()%(b-a)+a`. **`srand` is never called**, so world 3 layouts are the same every process until something else seeds `rand`.

## Authoring a new strip (mental checklist)

1. Keep `MAP_NUMBER` (30) in mind. World 3 already uses ~4×7 + 2 entries.
2. Set `xAmount`/`yAmount` so the AABB matches the `putimage` loops in `Scene::show`.
3. Ids ≥ 11 never block. Do not use them as floors.
4. Re-tune `isEnding` if you lengthen the stage.
5. Enemy grid cells are multiplied by 32 in `createEnemy`; coin cells are multiplied at draw/hit time; food is already pixels. Do not mix those units.

`examples/05_level_friction` rebuilds the `u` switch. `examples/06_side_scroller_sim` ships a **tiny** authored track (not the full 100-tile worlds) so the camera and ending flags can be unit-tested.
