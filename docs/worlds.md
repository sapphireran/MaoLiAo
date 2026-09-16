# Worlds

Tiles are authored as `{ tileX, tileY, id, xAmount, yAmount }`.
Pixel origin of a tile is `(tileX * 32, tileY * 32)`.

`MAP_NUMBER = 30`. Worlds 1 and 2 copy with
`while (id ∈ (0, 15) && i < 30)`, so **extra authored rows never load**.
World 3 uses `i <= sizeof(m)/sizeof(m[0])` and is a separate overflow
(see [quirks.md](quirks.md)).

Catalog JSON lives in `examples/catalog/worlds/`. `kit.worlds.load_world(n)`
can return the **runtime** cap (`cap=True`, default) or the **authored** list.

## Tile legend

| id | Draw | Collides | Friction T |
| --- | --- | --- | --- |
| 1 | Grass top | yes | T2 |
| 2 | Cloud | yes | T2 |
| 3 | Dirt / grass fill | yes | T2 |
| 4 | Snow fill | yes | T2 |
| 5 | Snow top | yes | T2 |
| 6 | Ice top | yes | T1 |
| 7 | Pipe shaft | yes | T3 |
| 8 | Downward pipe mouth | yes, 2×2 | T3 |
| 10 | Upward pipe mouth | yes, 2×2 | T3 |
| 11 | Grass tuft (ani sheet) | no | — |
| 12 | Goal sign | no | — |
| 13 | Water | no | — |
| 14 | Tree | no | — |

## World 1 — grassland

Runtime: 30 / 32 tiles. Authored water at tile x=71 and x=75 is dropped.
The first two water tiles (x=15, x=64) still load.

| Piece | Tiles |
| --- | --- |
| Ground tops | `[0,15)×y9`, `[18,27)×y8`, snow `[27,64)×y9`, ice `[67,71)×y9`, snow 1-wide at x=74, grass `[78,106)×y9` |
| Fills | matching dirt / snow under those tops |
| Clouds | `[10,14)×y6`, `[40,43)×y4`, 1-wide at (62,6), `[65,70)×y4` |
| Pipes | (36,7), (45,7), (80,7), (90,7) — id 10 |
| Goal (scenery) | (101,7) id 12 |
| Coins | 20: four on the first cloud, four on the high ice cloud, a 4-high stack at x=74, eight zigzag at x=82–89 |
| Flower | pixel `(14*32, 5*32)` = (448, 160), above the first cloud |
| Enemies | 10: (3,8), (18,7), (25,7), (28,8), (33,8), (39,8), (68,3), (66,8), (81,6), (92,6) |

First walkable gap: end of the opening grass (x=15 tiles = 480 px) to the
raised shelf at x=18 (576 px). Gap width **96 px**. A mid-speed jump clears it.

Ending tripwire: world-x `> 3008` (tile 94). Goal art sits at tile 101.

Max score if you collect every coin and remove every enemy: `20*10 + 10*5 = 250`.

## World 2 — snow / clouds

Runtime: 30 / 36 tiles. **Dropped:** background grass at (42,8), the goal
sign at (111,1), and all four trees. The finish still triggers on distance
(`> 3328` px) even without the sign.

Authored platforms (first 30 all load): nine cloud shelves, eight snow tops
plus fills, and a five-step cloud staircase at x=67–70 plus a 4-high pillar at x=75.

| Piece | Notes |
| --- | --- |
| Opening snow | `[0,15)` at y=10 |
| Cloud stack | (18,9) 4-wide, (23,6) 6-wide, (25,3) 3-wide, (31,10) 2-wide, poles at (39,7) and (39,4) |
| Late clouds | (96,7), (99,5), runway `(102,3)` 12-wide |
| Flower | pixel `(39*32 - 32/3, 3*32 + 32/5)` ≈ (1237.33, 102.4) |
| Coins | 11: three on the high cloud, a 5-high stack at x=32, three over the 1-wide snow pillars |
| Enemies | 6 |

Max score: `11*10 + 6*5 = 140`.

## World 3 — pipe gauntlet

Flappy-style. `rand() % 6 + 1` picks ten heights (only seven columns are
compiled in). There is **no `srand` in the V2.0 sources**, so the sequence
is whatever the CRT default is (often seed 1 on MSVC, not portable).

Column `i` (i = 0..6) uses `x = 10 + 10*i` and `h ∈ [1, 6]`:

| Part | Record |
| --- | --- |
| Top mouth | `{x, h-2, id 8, 1, 1}` |
| Top shaft | `{x, 0, id 7, 1, max(0, h-2)}` |
| Bottom mouth | `{x, 4+h, id 10, 1, 1}` |
| Bottom shaft | `{x, h+6, id 7, 1, max(0, 6-h)}` |

The vertical gap between the 2-tall mouths is **4 tiles = 128 px**.
Hero is 32 px. Air-jump is on, so the gap is about spacing, not one jump.

After the pipes: a 25-wide cloud at `(80, 6)` and a goal at `(101, 4)`.
That is **exactly 30** records. The loop still runs one extra time.

Coins: seven, at `((5+10*i), rand(3,6)+3)` — another `rand` stream.
Flower is authored as `{10, 10}` **pixels**, not tiles, so it sits on the
top-left sky and is easy to miss.

Touching any solid that is not a cloud kills you until the flower is eaten.
Enemies: 7, listed in `examples/catalog/worlds/world3.json`.

Ending: `> 3008` px. Max score about `7*10 + 7*5 = 105` if every enemy is removed.

## Catalog vs source

`examples/bin/export_catalog.py` is a check that the JSON matches the
literals in `MaoLiAo/scene.cpp` and `MaoLiAo/role.cpp`. If you edit a map
in C++, re-run the exporter or the suite will fail.
