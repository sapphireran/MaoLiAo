# Levels

Tiles are `{x, y, id, xAmount, yAmount}` in **cell** units (32 px), plus a
friction `u` filled from `id`. Arrays are copied into `Scene::map[MAP_NUMBER]`
with `MAP_NUMBER = 30`, so any record after index 29 is discarded.

Python dumps: `examples/data/world1.json`, `world2.json`, and
`examples.run_lab ascii-map`.

## Tile ids (`map.bmp` / `scenery.bmp`)

| id | Sheet | Solid? (`id < 11`) | Typical use |
| --- | --- | --- | --- |
| 1 | `map.bmp` row 0 | yes | Grass top |
| 2 | row 1 | yes | Cloud / snow platform |
| 3 | row 2 | yes | Dirt under grass |
| 4 | row 3 | yes | Snow underground |
| 5 | row 4 | yes | Snow top |
| 6 | row 5 | yes | Pipe body / icy top in world 1 |
| 7 | row 6, drawn 64×32 | yes | Vertical pipe shaft (world 3) |
| 8 | row 7, drawn 64×64 | yes | Downward pipe mouth (world 3) |
| 9 | row 8 | yes | (unused in the three shipped maps) |
| 10 | row 9, drawn 64×64 | yes | Upward pipe mouth |
| 11 | `scenery.bmp` | no | Background grass |
| 12 | scenery | no | Goal sign |
| 13 | scenery | no | Water (animated) |
| 14 | scenery | no | Tree (animated) |

Collision treats id 8 and 10 as **2×2** cells regardless of `xAmount`/`yAmount`.
Id 7 is blitted two tiles wide. Ids ≥ 11 use the 3×2 scenery frames with
`SRCAND`/`SRCPAINT`.

`createMap` friction (worlds 1–2):

- ids 1, 3, 4, 5, 2 → `T2` (normal)
- id 6 → `T1` (high)
- else → `T3`

World 3 special-cases ids 1, 3, 4, 5, **and 6** as `T1`.

## World 1 — grassland

Pass when world-x > 94 tiles. 32 records authored, **30 stored** (last two
water decorations at cells 71 and 75 never land in `map[]`).

Ground, left to right:

- Grass top `[0,9] ×15`, dirt `[0,10] ×15×2`
- Gap, then platform `[18,8] ×9` with dirt `[18,9] ×9×3`
- Long snow-top `[27,9] ×37` with snow-under `[27,10] ×37×2`
- High-friction strip `[67,9] ×4` (id 6)
- Single snow tile `[74,9]`, then grass `[78,9] ×28`
- Clouds at `[10,6]×4`, `[40,4]×3`, `[62,6]×1`, `[65,4]×5`
- Up-pipes at x = 36, 45, 80, 90 (id 10)
- Goal sign at `[101,7]` (id 12, **not solid**, auto-run starts at tile 94)
- Water at x = 15, 64, 71, 75 — only 15 and 64 survive the 30-slot cap

Coins (tile cells): four on the first cloud (10–13, y=5), four on the high
cloud (66–69, y=3), a vertical stack on the single tile x=74, then a
zigzag 82–89.

Weapon: one food at pixel `(14*32, 5*32)` = (448, 160), on the first cloud.

Enemies (tile, facing): `(3,8,+)` `(18,7,-)` `(25,7,+)` `(28,8,-)` `(33,8,-)`
`(39,8,+)` `(68,3,+)` `(66,8,-)` `(81,6,+)` `(92,6,+)`.

## World 2 — snow

Pass when world-x > 104 tiles. **36 records authored, 30 stored.** The copy
loop therefore drops:

```
{ 42,8,11,1,1 }           // grass tuft
{ 111,1,12,1,1 }          // goal sign — never drawn
{ 4,8,14 }, {9,8,14},
{ 83,8,14 }, {89,8,14}    // four trees
```

What *does* load: cloud crumbs, a 15-wide snow floor, mid-level stepping
stones, a staircase at x=67–70 (`yAmount` 1..4), a matching 4-tall pillar at
x=75, and the long snow runway from x=82.

Coins: `(25–27,2)`, a drop column at x=32 y=4..8, then `(50,6)(53,6)(56,6)`.

Weapon: pixel `(39*WIDTH - WIDTH/3, 3*HEIGHT + HEIGHT/5)`. `WIDTH` and
`HEIGHT` are ints, so this is `(1248 - 10, 96 + 6)` = `(1238, 102)`.

Enemies: `(18,8,+)` `(24,5,+)` `(28,5,-)` `(44,9,+)` `(97,6,+)` `(99,4,+)`.

Because the goal sign is missing, “finishing” world 2 is purely the
`isEnding` distance plus walking off the right of the window.

## World 3 — pipe gauntlet

`createMap` draws 7 of the 10 planned pipe columns (the last three are
commented out so the array fits in 30 slots), then a cloud runway
`{80,6,2,25,1}` and a sign `{101,4,12,1,1}`.

Each column `i` in `0..6`:

```
x      = i * 10 + 10          // 10, 20, 30, 40, 50, 60, 70
height = random(1, 7)         // 1..6 inclusive (rand()%(7-1)+1)

top mouth     (x, height-2, id 8)
top shaft     (x, 0,        id 7, yAmount = max(0, height-2))
bottom mouth  (x, 4+height, id 10)
bottom shaft  (x, height+6, id 7, yAmount = max(0, 6-height))
```

`random(a,b)` is `rand()%(b-a)+a` with **no `srand`**, so the pipe roll is
the CRT's default seed every process.

Coins: seven cells `(5, h0) … (65, h6)` with `h = random(3,7)`.

Weapon: `{10, 10}` **pixels**, i.e. almost on the origin — easy to miss, and
the only thing that sets `isShoot`. Without `isShoot`, `hitMap` for world 3
kills on any solid id other than 2 (the cloud). That is why grabbing the
weapon is also an invulnerability flag for pipes.

Enemies: `(24,4,-)` `(34,6,+)` `(37,3,-)` `(43,5,-)` `(63,7,+)` `(67,8,-)`
`(86,5,+)`.

Jump is unrestricted. Combined with pipe-touch death this is the “Flappy
Bird 关卡” mentioned in git history (`0f1f392`).

## Scoring

| Event | `score +=` |
| --- | --- |
| Coin | 10 |
| Stomp enemy | 5 |
| Bullet hits enemy | 5 |
| Weapon | 0 (sets `isShoot`) |

HUD is `Control::showScore` / `showLevel` in the Cooper font, top-left and
top-right.

## Lives and rebuilds

`LIFE` is 5. Death rebuilds `Role(world)` and `Scene(world)` in place
(world 3 therefore **re-rolls** pipes). World-up does the same with
`world++`. Game over and “passed all” reset `world = 1` and `life = LIFE`
and bounce through `gameStart()`.
