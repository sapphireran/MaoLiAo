# Levels

Maps are hardcoded C arrays in `Scene::createMap`, `createCoin`, `createFood`, and `Role::createEnemy`. There is no external editor file. Tile records are:

```text
{ x, y, id, xAmount, yAmount [, u filled in by switch] }
```

`x,y` are **tile** coordinates. Pixel origin of a record is `(x*32, y*32)`. `xAmount`/`yAmount` repeat the same sprite.

## Tile ids

From comments in `scene.cpp`:

| id | Art | Solid? | Friction band |
| --- | --- | --- | --- |
| 1 | Grass top | yes | T2 |
| 2 | Cloud | yes | T2 (safe in world 3) |
| 3 | Dirt under grass | yes | T2 |
| 4 | Snow subsurface | yes | T2 |
| 5 | Snow top | yes | T2 |
| 6 | Pipe mid / “ice” strip on world 1 | yes | T1 |
| 7 | Pipe shaft (world 3) | yes (id < 11) | T3 default / T1 in w3 switch |
| 8 | Inverted pipe mouth | yes, 64×64 box | T3 / T1 in w3 |
| 9 | (unused in current tables) | — | — |
| 10 | Upward pipe mouth | yes, 64×64 box | T3 / T1 in w3 |
| 11 | Background grass | no | — |
| 12 | Goal sign | no | — |
| 13 | Water | no | — |
| 14 | Tree | no | — |

`hitMap` only treats `id` in `(0, 11)`. Ids 11–14 are scenery blits.

World 3’s friction switch maps 1–6 to T1 and id 2 to T2; pipes 7/8/10 fall through to T3.

## Capacity

`MAP_NUMBER` is 30. Several tables are longer. The copy loop stops at 30, so trailing records never appear in-game.

| World | Authored map records | Copied | Dropped |
| --- | --- | --- | --- |
| 1 | 32 | 30 | last two water tiles (id 13 at 71 and 75) |
| 2 | 36 | 30 | goal sign + four trees + extra cloud stair bits depending on order |
| 3 | 30 (7 pipe pairs × 4 + cloud runway + sign) | 30 | none, but the loop is `i <= count` (OOB read) |

The world 2 loop comment is already `//???存在问题`.

Enemy / coin / food fill loops use `while (i <= sizeof(arr)/sizeof(arr[0]))`, which reads one past the local array. That is undefined behavior. The portable tables in `examples/` use the correct count.

## World 1 — grassland

Solid runs (tile X, Y, id, w, h):

```
ground tops:  (0,9,1,15,1) (18,8,1,9,1) (27,9,5,37,1) (67,9,6,4,1)
              (74,9,5,1,1) (78,9,1,28,1)
dirt / snow:  same X, Y+1, ids 3/4, extra height 2–3
clouds:       (10,6,2,4,1) (40,4,2,3,1) (62,6,2,1,1) (65,4,2,5,1)
pipes:        (36,7,10,1,1) (45,7,10,1,1) (80,7,10,1,1) (90,7,10,1,1)
goal sign:    (101,7,12,1,1)
```

Coins (tile):

```
10–13 @ y=5
66–69 @ y=3
74 @ y=5,6,7,8
82,84,86,88 @ y=7
83,85,87,89 @ y=8
```

Food: one pickup at pixel `(14*32, 5*32)` = (448, 160).

Enemies `{tileX, tileY, facing}`:

```
(3,8,+1) (18,7,-1) (25,7,+1) (28,8,-1) (33,8,-1)
(39,8,+1) (68,3,+1) (66,8,-1) (81,6,+1) (92,6,+1)
```

Ending distance: hero world X > 94 tiles.

## World 2 — sky

Cloud platforms scattered from x=18 to a long runway at `(102,3,2,12,1)`. Snow ground in broken islands. A staircase of stacked clouds at x=67–70 and a matching pillar at x=75. Trees at 4, 9, 83, 89. Goal sign authored at `(111,1,12,1,1)` — likely past the 30-slot copy, so the sign may not draw.

Coins:

```
25–27 @ y=2
32 @ y=4..8
50, 53, 56 @ y=6
```

Food: `(39*32 - 32/3, 3*32 + 32/5)` = (1238, 102) with integer division.

Enemies:

```
(18,8,+1) (24,5,+1) (28,5,-1) (44,9,+1) (97,6,+1) (99,4,+1)
```

Ending: 104 tiles.

## World 3 — pipes

Ten random heights `h_i ∈ [1,6]` and X positions `x_i = 10 + 10 i`. Only the first **seven** pairs are compiled in (the last three are commented out). Each pair is four records: upper mouth (id 8), upper shaft (id 7), lower mouth (id 10), lower shaft (id 7). Gap geometry:

```
top mouth y = h - 2
top shaft y = 0, height = max(0, h-2)
bottom mouth y = 4 + h
bottom shaft y = h+6, height = max(0, 6-h)
```

Then a cloud runway `{80,6,2,25,1}` and sign `{101,4,12,1,1}`.

Coins: seven random-height tiles at x = 5,15,…,65, y ∈ [3,6].

Food: `{10,10}` **pixels** (almost the origin). Easy to miss; looks like a leftover.

Enemies:

```
(24,4,-1) (34,6,+1) (37,3,-1) (43,5,-1) (63,7,+1) (67,8,-1) (86,5,+1)
```

Jump is always allowed. Touching any solid except clouds kills unless `isShoot`.

`rand()` is not seeded in the repo, so pipe layouts are often the same until something else calls `srand`.

## ASCII preview

`examples/demos/level_preview` prints a coarse occupancy grid for worlds 1–2 (deterministic) and a sample world 3 with a fixed seed. `examples/data/` stores the same tables as CSV for grepping.
