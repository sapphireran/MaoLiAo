# Map format

Levels are **not** loaded from disk. `Scene::createMap`, `Scene::createCoin`, `Scene::createFood`, and `Role::createEnemy` each contain a `switch (world)` with C++ array literals. The JSON files under `examples/levels/` are a documented export of those literals plus two extra design files that the C++ game does not load yet.

## `Map` record

```cpp
struct Map {
    int x;        // tile column
    int y;        // tile row (0 at the top of the window)
    int id;       // sprite row in map.bmp / scenery.bmp
    int xAmount;  // repeat count, X
    int yAmount;  // repeat count, Y
    double u;     // derived friction, written by createMap
};
```

Brace order in the sources is `{ x, y, id, xAmount, yAmount }` (and sometimes a trailing unused comma). `u` is never authored; it is computed from `id` (see [physics.md](physics.md)).

Screen pixel of the top-left of cell `(j, k)` inside a record:

```text
px = xMap + map.x * 32 + j * 32
py = yMap + map.y * 32 + k * 32
```

`yMap` stays 0. `xMap` is the camera origin (`hero.x0`).

## Tile ids

Comments in `scene.cpp` (world 1) are the canonical legend:

| id | Name | Solid? | Blit | Notes |
| --- | --- | --- | --- | --- |
| 1 | grass surface | yes | 32×32 from `map.bmp` | mid friction |
| 2 | cloud | yes | 32×32 | mid friction; **safe** in world 3 |
| 3 | dirt / grass underground | yes | 32×32 | mid friction |
| 4 | snow underground | yes | 32×32 | mid friction |
| 5 | snow surface (also used as grass-to-snow in world 1) | yes | 32×32 | mid friction |
| 6 | pipe shaft / slick strip | yes | 32×32 | **high** friction in worlds 1–2 |
| 7 | downward pipe extension | yes | **64×32** | world 3 shafts |
| 8 | inverted pipe mouth | yes | **64×64** (`2×2` cells) | world 3 ceiling pipes |
| 9 | (unused in shipped maps) | yes | 32×32 | reserved “upward mouth” in the comment block |
| 10 | upward pipe mouth | yes | **64×64** | worlds 1 and 3 |
| 11 | background grass | no | 96×64 from `scenery.bmp` | animated |
| 12 | goal sign | no | scenery | drawn, not a collider — clear is distance-based |
| 13 | water | no | scenery | animated |
| 14 | tree | no | scenery | animated |

`hitMap` only considers `0 < id < 11`. Ids 8 and 10 multiply both extents by 2, matching the 64×64 blit.

World 3’s kill rule uses the same solids, except id 2.

## Authoring units

| Thing | Unit in the C++ literal | Stored as |
| --- | --- | --- |
| `Map` x,y | tiles | tiles |
| coins | tiles | tiles (`POINT`) |
| enemies | tiles | **pixels** (`x * 32`, `y * 32`) |
| food world 1 | `{14*WIDTH, 5*HEIGHT}` | pixels |
| food world 2 | `{39*WIDTH - WIDTH/3, 3*HEIGHT + HEIGHT/5}` | pixels |
| food world 3 | `{10, 10}` | pixels |

The JSON schema uses `"unit": "tiles"` or `"unit": "pixels"` on each pickup so the toolkit does not guess.

Enemy `turn` is `+1` (right) or `-1` (left). `0` is unused.

## World 1 — grassland

Ending: map-x > 94 tiles (3008 px). 32 authored tiles, 20 coins, 1 food, 10 enemies.

Notable geometry:

- Ground bands at y=9 (surface) and y=10 (dirt), with a raised island at `(18, 8)` spanning 9 tiles.
- A long mixed strip starting at x=27 (`id` 5 over `id` 4) and a 4-tile slick patch (`id` 6) at x=67.
- Clouds at y=6 and y=4.
- Upward pipes (`id` 10) at x=36, 45, 80, 90.
- Goal sign at `(101, 7)`. Water decorations sit in the pits.

The copy loop keeps the first 30 records. The last two water tiles (`(71,10)` and `(75,10)`) do not fit in `map[30]`.

## World 2 — snow

Ending: map-x > 104 tiles (3328 px). 36 authored tiles, 11 coins, 1 food, 6 enemies.

- Snow surface (`id` 5) on y=10 with matching underground (`id` 4) on y=11.
- Floating clouds used as platforms, including a 12-tile run at `(102, 3)` that carries you to the sign at `(111, 1)`.
- A rising staircase of `id` 2 at x=67..70 and a matching pillar at x=75.
- Trees (`id` 14) at x=4, 9, 83, 89 — these are past slot 30, so they are **not installed**.

## World 3 — pipe gauntlet

Ending: map-x > 94 tiles. Pipes are generated at runtime:

```cpp
height[i] = random(1, 7);   // rand() % 6 + 1  →  1..6
x[i]      = i * 10 + 10;    // 10, 20, ..., 100
```

Only **i = 0..6** are compiled in (28 pipe pieces). Then a cloud runway `{80, 6, 2, 25, 1}` and a sign `{101, 4, 12, 1}` are appended (30 records). The install loop is `while (i <= sizeof(m)/sizeof(m[0]))`, one past the end — see [known-quirks.md](known-quirks.md).

Each pipe column:

```text
{ x, height-2,           8, 1, 1 }                 // inverted mouth
{ x, 0,                  7, 1, max(0, height-2) }  // top shaft
{ x, 4+height,          10, 1, 1 }                 // upward mouth
{ x, height+6,           7, 1, max(0, 6-height) }  // bottom shaft
```

Gap between the inverted mouth and the upward mouth is 4 tiles, which is just larger than the 3.16-tile jump. That is why world 3 grants infinite jumps.

`examples/levels/world-3-flappy.json` freezes one legal height roll so the toolkit can render and validate it. The running game will not match that snapshot unless you seed `rand`.

## JSON schema (examples only)

See `examples/levels/schema.md` and the files next to it. Required fields:

```json
{
  "id": "world-1",
  "world": 1,
  "name": "Grassland",
  "ending_tiles": 94,
  "friction_profile": "overworld",
  "tiles": [ { "x": 0, "y": 9, "id": 1, "x_amount": 15, "y_amount": 1 } ],
  "coins": [ { "x": 10, "y": 5 } ],
  "food":  [ { "x": 448, "y": 160, "unit": "pixels" } ],
  "enemies": [ { "x": 3, "y": 8, "turn": 1 } ]
}
```

`friction_profile` is `overworld` (worlds 1–2 table) or `pipes` (world 3 table).

## Camera-aware placement

Because the hero locks at screen-x=192, anything you want the player to stand on must exist in **map** tiles, not screen tiles. A platform at tile x=40 is 1280 px into the level; it appears when `x0 ≈ 192 - 1280`.

The goal sign is decoration. The actual clear trigger is `ending_tiles * 32`. Place the sign a little to the right of that line so the auto-run walks past it.

## Toolkit

```bash
python3 examples/toolkit/maoliao_levels.py validate examples/levels/*.json
python3 examples/toolkit/maoliao_levels.py render examples/levels/world-2-snow.json
python3 examples/toolkit/maoliao_levels.py export-cpp examples/levels/example-world-4-canyon.json
python3 examples/toolkit/maoliao_levels.py legend
```

`validate` checks array caps, id range, ending distance, overlapping solids, and whether a 101 px jump can reach the next solid from each authored platform (heuristic, not a full pathfinder).
