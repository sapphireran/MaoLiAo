# Map format

A world is a short array of `Map` records plus parallel `POINT` /
`Enemy` lists. There is no external map file. `Scene::createMap`,
`createCoin`, `createFood`, and `Role::createEnemy` hard-code each
world.

## `struct Map`

```cpp
struct Map {
    int x, y;       // tile column / row (origin top-left of the world)
    int id;         // tileset row (1-based)
    int xAmount;    // repeat count on X (tile widths)
    int yAmount;    // repeat count on Y (tile heights)
    double u;       // friction, filled after authoring
};
```

`MAP_NUMBER` is 30. World 2's authored list is larger than that in
places; extras are silently dropped by the copy loop. World 3's copy
loop is `<= sizeof` and can read one past the array
([known-issues.md](known-issues.md)).

## Tile ids

Comments in `scene.cpp` (world 1 editor block):

| id | Art | Collision (`hitMap` uses id 1–10) |
| --- | --- | --- |
| 1 | Grass top | Solid |
| 2 | Cloud | Solid (world 3: **safe** even without the star) |
| 3 | Dirt / desert subsoil | Solid |
| 4 | Snow subsoil | Solid |
| 5 | Snow top | Solid |
| 6 | Pipe shaft | Solid, strongest `u` (T1) |
| 7 | Downward pipe extension (world 3) | Solid, drawn 2 tiles wide |
| 8 | Downward pipe mouth | Solid, hit box 2×2 tiles |
| 9 | (reserved / unused in current lists) | — |
| 10 | Upward pipe mouth | Solid, hit box 2×2 tiles |
| 11 | Background grass | Decoration only |
| 12 | Goal sign | Decoration; finish is distance-based |
| 13 | Water | Decoration (animated scenery sheet) |
| 14 | Tree | Decoration (animated) |

`hitMap` iterates while `id > 0 && id < 11`. Ids 11–14 never kill or
block. Goal signs (12) are scenery; walking past `isEnding`'s pixel
threshold is what starts the auto-run.

Special blit sizes:

- id 7: 2×1 tiles from `map.bmp`
- id 8 and 10: 2×2 tiles from `map.bmp`
- id ≥ 11 except 8/10: 3×2 frames from `scenery.bmp`

## World 1 — grassland

Solids (tile xy + size). `u` uses T2 except id 6 (T1).

| x | y | id | xAmt | yAmt | Notes |
| --- | --- | --- | --- | --- | --- |
| 0 | 9 | 1 | 15 | 1 | Start grass |
| 18 | 8 | 1 | 9 | 1 | Raised shelf |
| 27 | 9 | 5 | 37 | 1 | Long snow-top run |
| 67 | 9 | 6 | 4 | 1 | Slippery/high-u patch |
| 74 | 9 | 5 | 1 | 1 | Isolated block |
| 78 | 9 | 1 | 28 | 1 | Home stretch |
| 0 | 10 | 3 | 15 | 2 | Dirt under start |
| 18 | 9 | 3 | 9 | 3 | Dirt under shelf |
| 27 | 10 | 4 | 37 | 2 | Snow fill |
| 67 | 10 | 3 | 4 | 2 | |
| 74 | 10 | 4 | 1 | 2 | |
| 78 | 10 | 3 | 28 | 2 | |
| 10 | 6 | 2 | 4 | 1 | Cloud |
| 40 | 4 | 2 | 3 | 1 | Cloud |
| 62 | 6 | 2 | 1 | 1 | Cloud |
| 65 | 4 | 2 | 5 | 1 | Cloud |
| 36 | 7 | 10 | 1 | 1 | Pipe |
| 45 | 7 | 10 | 1 | 1 | Pipe |
| 80 | 7 | 10 | 1 | 1 | Pipe |
| 90 | 7 | 10 | 1 | 1 | Pipe |

Decor: grass tufts (id 11) at (19,6), (23,6), (32,7), (49,7), (53,7),
(57,7), (93,7). Goal (12) at (101, 7). Water (13) at (15,10), (64,10),
(71,10), (75,10).

Coins (tile cells):

```
(10–13, 5)
(66–69, 3)
(74, 5) (74, 6) (74, 7) (74, 8)
(82,7) (83,8) (84,7) (85,8) (86,7) (87,8) (88,7) (89,8)
```

Food (pixels, not tiles): `(14 * 32, 5 * 32)` — one mushroom.

Enemies `{tileX, tileY, facing}` (`+1` right, `-1` left):

```
(3,8,1) (18,7,-1) (25,7,1) (28,8,-1) (33,8,-1)
(39,8,1) (68,3,1) (66,8,-1) (81,6,1) (92,6,1)
```

Stored as `tile * 32` pixel world positions.

## World 2 — sky / snow islands

Cloud platforms (id 2) at varying heights, snow tops (id 5) with
subsoil (id 4), a rising staircase of 1-wide clouds around x = 67–70,
trees (14) at 4, 9, 83, 89. Goal at (111, 1). One grass tuft at (42, 8).

Coins:

```
(25,2) (26,2) (27,2)
(32,4) (32,5) (32,6) (32,7) (32,8)
(50,6) (53,6) (56,6)
```

Food (pixels): `(39 * WIDTH - WIDTH/3, 3 * HEIGHT + HEIGHT/5)`.

Enemies:

```
(18,8,1) (24,5,1) (28,5,-1) (44,9,1) (97,6,1) (99,4,1)
```

## World 3 — pipe gauntlet

Ten random heights `random(1, 7)` and x slots `i * 10 + 10`. Only the
first **seven** gap columns are instantiated (the last three are
commented out). Each column is four `Map`s:

1. Upper mouth (id 8) at `y = height - 2`
2. Upper shaft (id 7) from y = 0, length `max(0, height - 2)`
3. Lower mouth (id 10) at `y = 4 + height`
4. Lower shaft (id 7) at `y = height + 6`, length `max(0, 6 - height)`

Then a cloud runway `{80, 6, id 2, 25 wide}` and goal `{101, 4, id 12}`.

Coins: seven cells at x = 5,15,…,65 with `random(3, 7)` y.

Food: `{10, 10}` pixels — easy to miss; treat as a known placement quirk.

Enemies (fixed, not random):

```
(24,4,-1) (34,6,1) (37,3,-1) (43,5,-1)
(63,7,1) (67,8,-1) (86,5,1)
```

`random` is `#define random(a,b) (rand()%(b-a)+a)` with no `srand` in
the project, so pipe layouts repeat until the CRT happens to seed
elsewhere.

## Collision box

Actor (hero, enemy, bullet) uses four inset vertices:

```
(x+1, y+1)
(x+WIDTH-1, y+1)
(x+1, y+HEIGHT-1)
(x+WIDTH-1, y+HEIGHT-1)
```

X is converted to world space by subtracting `hero.x0` for the hero
and by adding `hero.x0` when testing enemy screen positions (enemies
already live in world pixels).

Tile AABB:

```
left   = map.x * 32
top    = map.y * 32
right  = left + xAmount * (id is 8 or 10 ? 64 : 32)
bottom = top  + yAmount * (id is 8 or 10 ? 64 : 32)
```

A hit is **any actor vertex inside** the tile box. This is not a full
SAT / swept AABB: a fast bullet can tunnel, and a vertex-only test
misses some edge overlaps where boxes intersect but no actor corner
sits inside the tile.

Coins use a 32×32 box at `tile * 32`. Food uses the sprite size
`3*WIDTH/2+4` by `4*HEIGHT/5` at the stored pixel origin.

## Authoring a new platform

1. Append a `{x, y, id, xAmount, yAmount}` to the world's `Map m[]`.
2. Keep `id` in 1–14. Keep the total count ≤ 30.
3. Place coins as tile `POINT`s; food as pixels (match world 1).
4. Place enemies as tile triples; `createEnemy` multiplies by 32.
5. Extend `isEnding` if the world is longer than 94 / 104 tiles.
6. Rebuild. There is no hot reload.

`examples/05_map_layout` dumps world 1 as a text grid and checks that
every coin/enemy cell sits on or above a solid.
