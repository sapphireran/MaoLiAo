# Levels

Worlds 1 and 2 are hand-placed arrays in `Scene::createMap`,
`Scene::createCoin`, `Scene::createFood`, and `Role::createEnemy`.
World 3 builds pipes from `rand()`. Coordinates are **tile** units
unless a comment says pixels. One tile is 32×32.

`examples/level_catalog` dumps the same tables the source uses so you
can grep a CSV without opening Visual Studio.

## Tile ids (`createMap` comments)

| id | Sheet | Collision | Notes |
| --- | --- | --- | --- |
| 1 | grass top | yes | world 1 platforms |
| 2 | cloud | yes | safe in world 3 |
| 3 | dirt under grass | yes | stacked under id 1 |
| 4 | snow fill | yes | world 2 underground |
| 5 | snow / mixed top | yes | world 1 mid, world 2 ground |
| 6 | pipe body / ice top | yes | high friction in worlds 1–2 |
| 7 | pipe shaft | yes | world 3 vertical filler |
| 8 | inverted pipe mouth | yes, 2×2 box | world 3 |
| 9 | unused in current arrays | — | mentioned in comments |
| 10 | upright pipe mouth | yes, 2×2 box | worlds 1 and 3 |
| 11 | background grass | no (`id` ≥ 11 skipped in `hitMap`) | scenery |
| 12 | goal sign | no | drawn, not solid |
| 13 | water | no | animated scenery |
| 14 | tree | no | animated scenery |

`hitMap` only tests records with `0 < id < 11`. Goal signs, water, and
trees are decorations. The hero can walk through the flag; `isEnding`
is a distance check, not a sprite overlap.

A `Map` record is `{ x, y, id, xAmount, yAmount, u }`. `u` is filled by
the friction switch, not authored in the array. The record is repeated
as a rectangle of tiles: for each `j in [0, xAmount)` and
`k in [0, yAmount)` a blit happens at `(x+j, y+k)`.

## Ending distances

| World | `isEnding` when world-x exceeds | Flag tile |
| --- | --- | --- |
| 1 | 94 × 32 = 3008 px | `{101, 7, id 12}` |
| 2 | 104 × 32 = 3328 px | `{111, 1, id 12}` |
| 3 | 94 × 32 = 3008 px | `{101, 4, id 12}` |

After `ending`, the hero auto-holds right until `screen-x > 512`.

## World 1 — grassland

Solid walkables (collision `id` 1–10):

| x | y | id | xAmt | yAmt | What you see |
| --- | --- | --- | --- | --- | --- |
| 0 | 9 | 1 | 15 | 1 | starting grass |
| 18 | 8 | 1 | 9 | 1 | raised island |
| 27 | 9 | 5 | 37 | 1 | long mixed top |
| 67 | 9 | 6 | 4 | 1 | sticky strip |
| 74 | 9 | 5 | 1 | 1 | single tile |
| 78 | 9 | 1 | 28 | 1 | run-out to the flag |
| 0 | 10 | 3 | 15 | 2 | dirt under start |
| 18 | 9 | 3 | 9 | 3 | dirt under island |
| 27 | 10 | 4 | 37 | 2 | fill under mixed |
| 67 | 10 | 3 | 4 | 2 | fill |
| 74 | 10 | 4 | 1 | 2 | fill |
| 78 | 10 | 3 | 28 | 2 | fill |
| 10 | 6 | 2 | 4 | 1 | cloud |
| 40 | 4 | 2 | 3 | 1 | cloud |
| 62 | 6 | 2 | 1 | 1 | cloud |
| 65 | 4 | 2 | 5 | 1 | cloud |
| 36 | 7 | 10 | 1 | 1 | pipe |
| 45 | 7 | 10 | 1 | 1 | pipe |
| 80 | 7 | 10 | 1 | 1 | pipe |
| 90 | 7 | 10 | 1 | 1 | pipe |

Decor: grass tufts at x = 19, 23, 32, 49, 53, 57, 93 (all y = 6 or 7,
id 11); water at x = 15, 64, 71, 75 (y = 10, id 13).

Coins (tile xy):

```
(10,5) (11,5) (12,5) (13,5)           cloud four-pack
(66,3) (67,3) (68,3) (69,3)           high four-pack
(74,5) (74,6) (74,7) (74,8)           vertical stack
(82,7) (83,8) (84,7) (85,8)
(86,7) (87,8) (88,7) (89,8)           zigzag over the last grass (8 coins)
```

Twenty coins total.

Food (pixels): `(14 * 32, 5 * 32)` — weapon on the first cloud.

Enemies `{tile-x, tile-y, facing}`:

```
(3,8,+1) (18,7,-1) (25,7,+1) (28,8,-1) (33,8,-1)
(39,8,+1) (68,3,+1) (66,8,-1) (81,6,+1) (92,6,+1)
```

`createEnemy` copies with `i <= count`, which reads one past the
initializer (see [known-quirks.md](known-quirks.md)). The last written
slot is uninitialized stack data.

## World 2 — sky / snow

More vertical. Ground is id 5 on y = 10 with id 4 fill on y = 11.
Floating clouds (id 2) form the parkour:

| x | y | id | xAmt | yAmt |
| --- | --- | --- | --- | --- |
| 18 | 9 | 2 | 4 | 1 |
| 23 | 6 | 2 | 6 | 1 |
| 25 | 3 | 2 | 3 | 1 |
| 31 | 10 | 2 | 2 | 1 |
| 39 | 7 | 2 | 1 | 1 |
| 39 | 4 | 2 | 1 | 1 |
| 96 | 7 | 2 | 2 | 1 |
| 99 | 5 | 2 | 2 | 1 |
| 102 | 3 | 2 | 12 | 1 |
| 67 | 9 | 2 | 1 | 1 |
| 68 | 8 | 2 | 1 | 2 |
| 69 | 7 | 2 | 1 | 3 |
| 70 | 6 | 2 | 1 | 4 |
| 75 | 6 | 2 | 1 | 4 |

Staircase clouds at 67–70 are a 1-wide pyramid. The pair at 70 and 75
is a gap jump.

Ground segments (id 5 / 4): x ranges 0–14, 38–47, singles at 50 / 53 /
56 / 75, 61–70, 82–92.

Trees (id 14) at x = 4, 9, 83, 89 (y = 8). One tuft at (42, 8).

Coins:

```
(25,2) (26,2) (27,2)          top cloud
(32,4) (32,5) (32,6) (32,7) (32,8)   pole
(50,6) (53,6) (56,6)          over the singles
```

Food (pixels): `(39 * 32 - 32/3, 3 * 32 + 32/5)` — tucked on the high
single cloud.

Enemies:

```
(18,8,+1) (24,5,+1) (28,5,-1) (44,9,+1) (97,6,+1) (99,4,+1)
```

## World 3 — pipe gauntlet

Not a fixed map. `createMap` draws 10 candidate columns at

```
x[i] = 10 + 10 * i          // 10, 20, 30, ..., 100
height[i] = rand() % 6 + 1  // 1..6
```

Only columns 0..6 are actually emitted (the 7..9 recipe is commented
out). Each column is four records:

```
{ x, height-2,     id 8, 1, 1 }                 // top mouth
{ x, 0,            id 7, 1, max(0, height-2) }  // shaft down from ceiling
{ x, 4+height,     id 10, 1, 1 }                // bottom mouth
{ x, height+6,     id 7, 1, max(0, 6-height) }  // shaft up from floor
```

Then a cloud runway `{80, 6, id 2, 25, 1}` and the flag
`{101, 4, id 12}`.

`createCoin` picks seven coins at x = 5, 15, …, 65 with
`y = rand() % 4 + 3` (3..6).

Food is `{10, 10}` **pixels**, not tiles — a sprite sitting near the
top-left of the screen. Collecting it sets `isShoot`, which also
disables the "any solid kills you" rule.

Enemies (fixed, not random):

```
(24,4,-1) (34,6,+1) (37,3,-1) (43,5,-1)
(63,7,+1) (67,8,-1) (86,5,+1)
```

`rand()` is never seeded in the 2020 `main`. World 3 therefore repeats
the same pipe heights every process until something else calls
`srand`. `examples/level_catalog` seeds explicitly when you pass
`--seed N` so dumps are reproducible.

## How to author a new strip (worlds 1–2)

1. Add a `Map` line `{ tileX, tileY, id, widthTiles, heightTiles }`.
2. Keep `id` in 1..10 if the hero should stand on it.
3. Stay under `MAP_NUMBER` (30). World 1 authors 32 records (last two
   water tiles drop). World 2 authors 36 (last six drop: the flag at
   `{111, 1, id 12}` and four trees). The **ending still works** because
   `isEnding` is a distance check, not a flag overlap. See quirks.
4. Place coins as tile pairs; food as pixel pairs.
5. Place enemies as tile triples. Keep `ENEMY_TOTE` (30) in mind.

`examples/level_catalog --world 2 --warn` prints how many records were
truncated.

## Score budget if you 100% a world

Assuming every authored coin and enemy is collected (and the
off-by-one extra enemy slot is ignored):

| World | Coins ×10 | Enemies ×5 | Max if all stomped / shot |
| --- | --- | --- | --- |
| 1 | 20 × 10 = 200 | 10 × 5 = 50 | 250 |
| 2 | 11 × 10 = 110 | 6 × 5 = 50 | 160 |
| 3 | 7 × 10 = 70 | 7 × 5 = 35 | 105 |

Food does not add score. The pickup flash still uses `setScorePos`.
