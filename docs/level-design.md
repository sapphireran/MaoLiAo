# Level design

Tiles are `{x, y, id, xAmount, yAmount}` in **tile** units (32 px). `Scene::show` repeats the source cell across that rectangle. Collision uses the same rectangle, except IDs 8 and 10 (pipe mouths) which collide as `2*xAmount` × `2*yAmount` tiles.

`MAP_NUMBER` is 30. `createMap` copies while `id ∈ (0, 15)` **and** `i < 30`. Extra rows in the C++ initializer are authored, visible in `fixtures/levels/`, and **dropped at runtime**.

## Tile catalog

From the comments in `Scene::createMap` plus the blit branches in `Scene::show`. Full table: `fixtures/tiles.json`.

| ID | Name | Sheet | Collides | Default friction band (W1–2) |
| --- | --- | --- | --- | --- |
| 1 | Grass brick | `map.bmp` row 0 | yes | medium (`T2`) |
| 2 | Cloud | `map.bmp` row 1 | yes | medium |
| 3 | Dirt / grass under | `map.bmp` row 2 | yes | medium |
| 4 | Snow under | `map.bmp` row 3 | yes | medium |
| 5 | Snow surface | `map.bmp` row 4 | yes | medium |
| 6 | Pipe body | `map.bmp` row 5 | yes | high (`T1`) |
| 7 | Pipe shaft (world 3, 64×32 blit) | `map.bmp` row 6 | yes | low (`T3`) |
| 8 | Downward pipe mouth (64×64) | `map.bmp` row 7 | yes | low |
| 9 | (reserved in comments; unused) | | | |
| 10 | Upward pipe mouth (64×64) | `map.bmp` row 9 | yes | low |
| 11 | Background grass | `scenery.bmp` | no | — |
| 12 | Victory sign | `scenery.bmp` | no | — |
| 13 | Water | `scenery.bmp` | no | — |
| 14 | Tree | `scenery.bmp` | no | — |

IDs ≥ 11 animate with `scenery_iframe` (two frames). Water and trees are decoration; pits still kill via `y > YSIZE`.

## World 1 — overworld

32 authored tiles, 30 loaded. Dropped: the last two water decorations at tiles (71,10) and (75,10). Water at (15,10) and (64,10) survives.

Ground plan (surface only):

```text
x=0..14     grass (id 1) at y=9, dirt under
x=18..26    grass at y=8 (one tile higher island)
x=27..63    snow surface (id 5) at y=9
x=67..70    pipe body (id 6) at y=9   # high friction
x=74        snow single
x=78..105   grass at y=9
```

Clouds: (10,6)×4, (40,4)×3, (62,6)×1, (65,4)×5. Pipes mouths at 36, 45, 80, 90 (id 10). Flag at (101,7). Ending at world-x > 94 tiles — the hero auto-walks past the flag.

Coins (tile xy): cloud 10–13 y=5; snow stairs 66–69 y=3; column 74 y=5..8; zigzag 82–89.

Enemies (tile xy, facing): (3,8,+), (18,7,−), (25,7,+), (28,8,−), (33,8,−), (39,8,+), (68,3,+), (66,8,−), (81,6,+), (92,6,+).

Flower: pixel (448, 160) = tile (14, 5), the right edge of the first cloud.

## World 2 — snow / clouds

36 authored tiles, 30 loaded. **Dropped at runtime:** background grass (42,8), victory sign (111,1), trees at 4, 9, 83, 89. The level still ends at 104 tiles because `isEnding` is a distance check, not a flag collision. The sign was never reachable as geometry.

Playable solids that *do* load:

- Snow floor fragments at y=10: `[0,15)`, `[38,48)`, singles at 50, 53, 56, `[61,71)`, 75, `[82,93)`.
- Cloud platforms: (18,9)×4, (23,6)×6, (25,3)×3, (31,10)×2, stacked (39,7) and (39,4), late (96,7)×2, (99,5)×2, (102,3)×12.
- Cloud staircase: (67,9), (68,8)×2 tall, (69,7)×3, (70,6)×4, (75,6)×4.

Coins: top cloud 25–27 y=2; vertical well at x=32 y=4..8; singles above the 50/53/56 pillars.

Enemies: (18,8,+), (24,5,+), (28,5,−), (44,9,+), (97,6,+), (99,4,+).

Flower: pixels (1237, 102), on the x=39 cloud stack.

## World 3 — pipe run

Procedural. `Scene::createMap` draws 10 candidate columns `x[i] = i*10+10` and `height[i] = rand()%(7-1)+1` → **1..6**, then only uses **i = 0..6** (the i=7..9 groups are commented out). Each column emits four tiles:

```text
{ x, height-2,     id 8,  1, 1 }               # down mouth
{ x, 0,            id 7,  1, max(0, height-2) } # upper shaft
{ x, 4+height,     id 10, 1, 1 }               # up mouth
{ x, height+6,     id 7,  1, max(0, 6-height) } # lower shaft
```

Plus a 25-tile cloud runway at (80,6) and a flag at (101,4). That is 7×4+2 = **30** tiles — exactly `MAP_NUMBER`.

The copy loop is `while (i <= sizeof(m)/sizeof(m[0]))`, which is one iteration too far (see [known quirks](known-quirks.md)).

Coins: seven points `{10*k+5, rand()%(7-3)+3}` for k=0..6, so x=5,15,…,65 and y ∈ 3..6.

Enemies are *not* random: (24,4,−), (34,6,+), (37,3,−), (43,5,−), (63,7,+), (67,8,−), (86,5,+).

Rules + one deterministic example: `fixtures/levels/world-3.json`. `tools/validate_fixtures.py` rebuilds the example tiles from the height table so the ASCII map stays honest.

## Capacity table

| Array | Cap | W1 used (authored) | W2 | W3 |
| --- | --- | --- | --- | --- |
| `Map map[30]` | 30 | 32 authored / 30 live | 36 / 30 | 30 + off-by-one write |
| `POINT coins[70]` | 70 | 20 | 11 | 7 |
| `POINT food[5]` | 5 | 1 | 1 | 1 |
| `Enemy[30]` | 30 | 10 | 6 | 7 |

Every `create*` loop is `while (i <= sizeof(arr)/sizeof(*arr))`, so it writes **one past** the initializer (coins/enemies/food land in the next slot as leftover stack memory; see quirks).

## Editing a level

There is no editor. Change the `Map m[]` / `POINT p[]` / `Enemy emy[]` literals, then refresh the JSON:

1. Edit `MaoLiAo/scene.cpp` / `role.cpp`.
2. Mirror the numbers in `fixtures/levels/world-N.json` (or let the extractor fail and paste its diff).
3. `python3 tools/validate_fixtures.py` — it will rewrite nothing; it only diffs. Update `fixtures/expected/world-N.ascii.txt` by running `python3 tools/render_level.py --world N --write-expected` after you intend the new map.

Keep `MAP_NUMBER` in mind: the 31st tile is fiction.
