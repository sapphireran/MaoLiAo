# Map format

A level is an array of `Map` records plus two `POINT` arrays (coins,
food) and an `Enemy` array. Nothing is loaded from disk except the
save world's integer.

```c
struct Map {
    int x, y;        // tile-grid origin (multiply by WIDTH/HEIGHT = 32)
    int id;          // sprite row / collision class
    int xAmount;     // how many tiles wide  (id 8/10 use 2*WIDTH per cell)
    int yAmount;     // how many tiles tall  (id 8/10 use 2*HEIGHT per cell)
    double u;        // friction, filled in after the authored {} list
};
```

`Scene::createMap` copies into `map[MAP_NUMBER]` with `MAP_NUMBER = 30`.
The authored initializer for worlds 1 and 2 is **longer than 30**, so
the last scenery tiles never reach `map[]`. World 3 builds pipes from
`rand()` heights, then a cloud runway and a goal sign.

`Role::hitMap` only collides with `id` in `(0, 11)` — scenery 11–14 is
decoration.

## Tile IDs (`scene.cpp` comments)

| id | Sheet | Collision | Notes |
| --- | --- | --- | --- |
| 1 | `map.bmp` row 0 | yes | Grass top |
| 2 | `map.bmp` row 1 | yes | Cloud; world 3 treats this as safe even before the weapon |
| 3 | `map.bmp` row 2 | yes | Dirt / desert underfill |
| 4 | `map.bmp` row 3 | yes | Snow underfill |
| 5 | `map.bmp` row 4 | yes | Snow top |
| 6 | `map.bmp` row 5 | yes | Pipe shaft (world 1 uses high μ) |
| 7 | `map.bmp` row 6 | yes | Pipe extension; blit is 2×32 wide |
| 8 | `map.bmp` row 7 | yes | Downward pipe mouth; 64×64 hit box |
| 9 | `map.bmp` row 8 | yes | Upward pipe mouth (unused in current arrays) |
| 10 | `map.bmp` row 9 | yes | Upward pipe mouth; 64×64 hit box |
| 11 | `scenery.bmp` | no | Background grass (animated) |
| 12 | `scenery.bmp` | no | Goal sign |
| 13 | `scenery.bmp` | no | Water (animated) |
| 14 | `scenery.bmp` | no | Tree (animated) |

Blit sizes in `Scene::show`:

- id 7: 64×32 from `map.bmp`
- id 8 or 10: 64×64 from `map.bmp`
- id > 7 otherwise: 96×64 from `scenery.bmp` (two-pass transparent)
- else: 32×32 from `map.bmp` at `(0, (id-1)*32)`

## World 1 — grass

Finish line: world X > 94 tiles.

Authored platforms (grid cells, `{x, y, id, xAmount, yAmount}`):

```
# tops
{  0, 9, 1, 15, 1 }   grass
{ 18, 8, 1,  9, 1 }
{ 27, 9, 5, 37, 1 }   snow top (long runway)
{ 67, 9, 6,  4, 1 }   pipe / high friction
{ 74, 9, 5,  1, 1 }
{ 78, 9, 1, 28, 1 }

# underfill
{  0,10, 3, 15, 2 }
{ 18, 9, 3,  9, 3 }
{ 27,10, 4, 37, 2 }
{ 67,10, 3,  4, 2 }
{ 74,10, 4,  1, 2 }
{ 78,10, 3, 28, 2 }

# clouds
{ 10, 6, 2, 4, 1 }
{ 40, 4, 2, 3, 1 }
{ 62, 6, 2, 1, 1 }
{ 65, 4, 2, 5, 1 }

# pipe mouths (id 10)
{ 36, 7,10, 1, 1 }
{ 45, 7,10, 1, 1 }
{ 80, 7,10, 1, 1 }
{ 90, 7,10, 1, 1 }

# grass scenery (id 11) — last two water tiles sit past MAP_NUMBER
{ 19, 6,11, 1, 1 } … { 93, 7,11, 1, 1 }
{101, 7,12, 1, 1 }   goal
{ 15,10,13, 1, 1 } { 64,10,13, 1, 1 } { 71,10,13, 1, 1 } { 75,10,13, 1, 1 }
```

Coins (tile coords):

```
(10–13, 5)   (66–69, 3)   (74, 5–8)
(82,7) (83,8) (84,7) (85,8) (86,7) (87,8) (88,7) (89,8)
```

Food / weapon: one pickup at `(14*32, 5*32)`.

Enemies `{tileX, tileY, turn}`:

```
(3,8,+1) (18,7,-1) (25,7,+1) (28,8,-1) (33,8,-1)
(39,8,+1) (68,3,+1) (66,8,-1) (81,6,+1) (92,6,+1)
```

## World 2 — ice

Finish line: world X > 104 tiles.

Cloud stepping stones, snow tops at y=10, snow underfill at y=11, a
rising staircase at x=67–70, a 4-tall column at (75,6), trees, goal at
`(111, 1)`.

Coins:

```
(25–27, 2)   (32, 4–8)   (50,6) (53,6) (56,6)
```

Food: `(39*32 - 32/3, 3*32 + 32/5)` — intentionally off-grid.

Enemies:

```
(18,8,+1) (24,5,+1) (28,5,-1) (44,9,+1) (97,6,+1) (99,4,+1)
```

## World 3 — pipes (Flappy-style)

Finish line: world X > 94 tiles.

Ten random heights `random(1, 7)` and X positions `i*10 + 10`. Only
indices 0–6 are emitted (7–9 are commented out). Each index becomes
four tiles:

```
upper mouth   { x, height-2,     id 8, 1×1 }
upper shaft   { x, 0,            id 7, 1 × max(0, height-2) }
lower mouth   { x, 4+height,     id 10, 1×1 }
lower shaft   { x, height+6,     id 7, 1 × max(0, 6-height) }
```

Then a cloud runway `{80, 6, id 2, 25×1}` and a goal `{101, 4, id 12}`.

Coins: seven random-height columns at x = 5,15,…,65.
Food: `(10, 10)` in **pixels**, not tiles — almost on the origin.

World 3 collision rule (`hitMap`): any solid other than id 2 kills the
hero unless `isShoot` is already true (weapon = invincibility here).

The copy loop is `while (i <= sizeof(m)/sizeof(m[0]))`, which reads one
past the array. See [known-issues.md](known-issues.md).

## Camera vs tile X

On screen:

```
px = xMap + map.x * 32 + cell * 32
py = map.y * 32 + cell * 32
```

`xMap` is `Hero.x0` (≤ 0). Hit tests convert the sprite back to world
space with `-x0 + spriteX` before comparing to `map.x * 32`.

## How to author a new platform

1. Pick a grid cell `(tx, ty)` for the top-left.
2. Choose `id` from the table (1–10 solid, 11–14 deco).
3. Set `xAmount` / `yAmount` in **tile cells**, not pixels.
4. Keep the total record count ≤ 30 or raise `MAP_NUMBER` in
   `scene.h` **and** every loop that uses it.
5. Place coins as `{tx, ty}` in `createCoin` (tile space).
6. Place food as pixels in `createFood` (world 1 already does this).
7. Place enemies as `{tx, ty, turn}` in `Role::createEnemy`.

`examples/04_level_preview` dumps worlds 1–2 as ASCII so a change to
those tables can be diffed without launching EasyX.
