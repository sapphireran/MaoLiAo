# Levels

Three worlds share one `Scene` / `Role` pair. The world index selects hard-coded tables in `createMap`, `createCoin`, `createFood`, and `createEnemy`. There is no external map file.

Tile coordinates are in cells. Pixel position is `cell * 32`. `Map` also stores how many cells the brush covers (`xAmount`, `yAmount`) and a friction `u` (see [physics.md](physics.md)).

## Tile IDs

Comments in `scene.cpp` and the blit switch in `Scene::show` agree on this atlas:

| ID | Name | Solid? | Blit |
| --- | --- | --- | --- |
| 1 | Grass top | yes | 32×32 from `map.bmp` |
| 2 | Cloud | yes | 32×32 |
| 3 | Dirt / grass underground | yes | 32×32 |
| 4 | Snow underground | yes | 32×32 |
| 5 | Snow top | yes | 32×32 |
| 6 | High-friction strip (comment: pipe middle) | yes | 32×32 |
| 7 | Pipe shaft (world 3) | yes | 64×32 |
| 8 | Inverted pipe mouth | yes | 64×64 (collision also 2×) |
| 9 | Unused in the shipped tables | yes if present | 32×32 |
| 10 | Upright pipe mouth | yes | 64×64 (collision also 2×) |
| 11 | Background grass | no | 96×64 from `scenery.bmp` |
| 12 | Victory sign | no | scenery sheet |
| 13 | Water | no | scenery sheet, animated |
| 14 | Tree | no | scenery sheet, animated |

`hitMap` only treats IDs `1 … 10` as solid. IDs 11–14 are drawn and ignored.

## Capacity vs authored size

`MAP_NUMBER` is 30. World 1 authors **32** tiles. World 2 authors **36**. The copy loop stops at `i < MAP_NUMBER`, so the extra rows never land in `map[]`.

World 1 therefore **drops** the last two water decorations (`{71,10,13}` and `{75,10,13}`).

World 2 copies the first 30 rows (clouds, snow tops, underground, stair clouds) and **drops** the grass tuft, the victory sign, and the four trees.

World 3 authors exactly 30 tiles (7 pipe groups × 4 + cloud runway + victory sign) and then uses `i <= sizeof(m)/sizeof(m[0])`, which walks one element **past** the array. That is undefined behavior. The examples rebuild the intended tables without the overrun.

`examples/levels/dump_layouts.py` prints the tiles that actually survive the `MAP_NUMBER` cap, and the tiles the source asked for.

## World 1 — grassland

Ending when world-x > `94 * 32` (3008 px). Hero then auto-walks until screen `x > 512`.

**Ground**

| x | y | id | w | h | Notes |
| --- | --- | --- | --- | --- | --- |
| 0 | 9 | 1 | 15 | 1 | Starting grass |
| 18 | 8 | 1 | 9 | 1 | Raised island |
| 27 | 9 | 5 | 37 | 1 | Long snow top |
| 67 | 9 | 6 | 4 | 1 | High-friction strip |
| 74 | 9 | 5 | 1 | 1 | Single snow cell |
| 78 | 9 | 1 | 28 | 1 | Runway to the sign |

Underground rows (id 3 / 4) sit one cell below those tops. Clouds sit at `(10,6)` w=4, `(40,4)` w=3, `(62,6)` w=1, `(65,4)` w=5. Four upright pipes at x = 36, 45, 80, 90 (y = 7). Victory sign at `(101, 7)`.

**Coins** (tile cells): a row of four on the first cloud `(10–13, 5)`; four on the high cloud `(66–69, 3)`; a vertical stack at x=74, y=5…8; a sawtooth `(82,7) (83,8) … (89,8)`.

**Food:** one weapon at pixel `(14*32, 5*32) = (448, 160)` — the right edge of the first cloud.

**Enemies** `{tileX, tileY, facing}` with `+1` right, `-1` left:

```
(3,8, +), (18,7, -), (25,7, +), (28,8, -), (33,8, -),
(39,8, +), (68,3, +), (66,8, -), (81,6, +), (92,6, +)
```

`createEnemy` uses `i <= count`, so it also writes a garbage 11th slot from off the stack array. The examples only keep the ten authored rows.

## World 2 — sky / snow

Ending when world-x > `104 * 32` (3328 px).

Floating clouds form the real floor: a staircase of id-2 cells around x=18–31, paired pillars at x=39, a late pair at x=96–102, and a 4-high stair at x=67–75. Snow tops (id 5) and snow underground (id 4) fill y=10–12 in broken segments, so falling between islands is death.

**Coins:** three on the high cloud `(25–27, 2)`; a vertical stack at x=32, y=4…8; three floating at `(50,6) (53,6) (56,6)`.

**Food:** pixel `(39*32 - 32/3, 3*32 + 32/5) ≈ (1237, 102)` near the upper pillar.

**Enemies:**

```
(18,8, +), (24,5, +), (28,5, -), (44,9, +), (97,6, +), (99,4, +)
```

## World 3 — pipe field

Ending when world-x > `94 * 32` (3008 px), same as world 1.

On construct, ten random heights `random(1, 7)` and ten x positions `i*10 + 10` are rolled. Only the first **seven** columns are used. Each column is four tiles:

```
inverted mouth  id 8 at (x, height-2)
upper shaft     id 7 at (x, 0)      height max(0, height-2)
upright mouth   id 10 at (x, 4+height)
lower shaft     id 7 at (x, height+6) height max(0, 6-height)
```

A cloud runway `{80, 6, id 2, w=25}` and a victory sign `{101, 4, id 12}` finish the stage.

Rules that make it Flappy-like:

- `CMD_UP` always relaunches (`Role::action` special-cases `world == 3`)
- any solid tile except id 2 kills unless `isShoot` is set
- coins are seven random-height cells at x = 5, 15, …, 65
- food is a 10×10 px pickup at `(10, 10)`

`random(a, b)` is `rand() % (b - a) + a`. `srand` is never called, so the pipe field is deterministic per process on most runtimes.

## Camera length vs ending

The hero locks at screen x = 192. World-x is `-x0 + x`. `isEnding` compares that world-x to the thresholds above. After `ending` is set, `CMD_RIGHT` is forced and the sprite is allowed past `XRIGHT` until it walks off the right of the 512 px window.

## Regenerating the tables

```bash
python3 examples/levels/dump_layouts.py
python3 examples/levels/dump_layouts.py --check-fixtures
python3 examples/levels/test_worlds.py
```

Checked-in ASCII snapshots live in `examples/levels/fixtures/world{1,2,3}.txt`. The tests assert tile counts, ending distances, enemy facings, coin values, the `MAP_NUMBER` truncation documented here, and that those fixtures still match the dump.
