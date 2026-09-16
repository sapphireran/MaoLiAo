# Levels

Three worlds share one `Map` struct and one 32×32 tile grid. Coordinates in `Scene::createMap` / `createCoin` / `Role::createEnemy` are **tile indices** unless noted. Food in worlds 1–2 is authored in **pixels**.

```
struct Map {
    int x, y;       // top-left tile
    int id;         // sheet row / behavior
    int xAmount;    // tiles wide  (id 8 and 10 collide as 2× wide)
    int yAmount;    // tiles tall  (id 8 and 10 collide as 2× tall)
    double u;       // friction, filled in createMap
};
```

`MAP_NUMBER = 30` is the runtime array length. The authored lists for worlds 1 and 2 are longer; `createMap` copies until `id` is out of `1..14` **or** `i == MAP_NUMBER`. Surplus tiles never appear. The level dumper reports the overflow.

Portable copies of the arrays live in `examples/lib/worlds.hpp`.

## Tile ids

Drawn from `res/map.bmp` (ids 1–10) or `res/scenery.bmp` (ids 11–14).

| Id | Sheet | Draw size | Collides | Notes |
| ---: | --- | --- | --- | --- |
| 1 | map row 0 | 32×32 | yes | Grass surface |
| 2 | map row 1 | 32×32 | yes | Cloud; safe in world 3 |
| 3 | map row 2 | 32×32 | yes | Dirt under grass |
| 4 | map row 3 | 32×32 | yes | Snow subsurface |
| 5 | map row 4 | 32×32 | yes | Snow surface |
| 6 | map row 5 | 32×32 | yes | High-friction ice / pipe-top |
| 7 | map row 6 | 64×32 | yes | Pipe shaft; world 3 |
| 8 | map row 7 | 64×64 | yes, 2×2 | Inverted pipe mouth |
| 9 | map row 8 | 64×64 | yes, 2×2 | Unused in the shipped maps |
| 10 | map row 9 | 64×64 | yes, 2×2 | Upward pipe mouth |
| 11 | scenery | 96×64 animated | no | Background grass |
| 12 | scenery | 96×64 animated | no | Goal plaque |
| 13 | scenery | 96×64 animated | no | Water |
| 14 | scenery | 96×64 animated | no | Tree |

`hitMap` only walks entries with `0 < id < 11`. Goal plaques therefore do not block; the stage ends by distance, not by touching the sign.

Ids 8 and 10 inflate the AABB to `xAmount * 64` by `yAmount * 64` even when `xAmount` is 1.

## World 1 — grassland

A tutorial lane: dirt under grass, a few floating clouds, four pipe mouths, a short ice patch (`id 6` at x=67), waterfalls in the pits, a goal sign at tile 101.

| Kind | Count authored | Count that fit in `MAP_NUMBER` / tables |
| --- | ---: | ---: |
| Map stamps | 32 | 30 (last two water tiles dropped) |
| Coins | 20 | 20 |
| Enemies | 10 | 10 |
| Food | 1 | pixel `(14*32, 5*32) = (448, 160)` |

Coin clusters:

- Cloud top: tiles `(10..13, 5)`
- High ice cloud: `(66..69, 3)`
- Vertical stack on the lonely snow block: `(74, 5..8)`
- Zigzag over the last grass: `(82,7) (83,8) … (89,8)`

Enemies (tile x, y, initial `turn`):

```
(3,8,+1) (18,7,-1) (25,7,+1) (28,8,-1) (33,8,-1)
(39,8,+1) (68,3,+1) (66,8,-1) (81,6,+1) (92,6,+1)
```

Ending rail: world-x > 94 tiles (3008 px).

## World 2 — snow platforms

Floating clouds, broken snow ground, a 1-wide staircase at x=67–70, trees, goal sign at `(111, 1)`.

| Kind | Authored | Stored |
| --- | ---: | ---: |
| Map stamps | 36 | 30 (six decorations / stairs overflow) |
| Coins | 11 | 11 |
| Enemies | 6 | 6 |
| Food | 1 | pixel `(39*32 - 32/3, 3*32 + 32/5) ≈ (1237, 102)` |

Coins sit on the high cloud `(25..27, 2)`, a vertical climb at x=32, and three sky coins over the 1-tile snow pillars.

Ending rail: 104 tiles (3328 px).

Because six stamps are dropped, the dumped ASCII map and the running game can disagree on trees and the far staircase. That is a shipped limitation, not a dumper bug. See [known-issues.md](known-issues.md).

## World 3 — pipe gauntlet (Flappy-style)

`createMap` rolls `height[i] = rand() % 6 + 1` (`random(1,7)`) for i in `0..9`, then places pipe **pairs** at `x[i] = 10 + 10*i`. Pairs 7–9 are commented out, so seven gaps ship.

Each pair is four stamps:

```
{ x, height-2,     id 8,  1, 1 }          // inverted mouth
{ x, 0,            id 7,  1, max(0, h-2)} // shaft from the ceiling
{ x, 4+height,     id 10, 1, 1 }          // upward mouth
{ x, height+6,     id 7,  1, max(0, 6-h)} // shaft toward the floor
```

Then a long cloud runway `{80, 6, id 2, 25, 1}` and a goal `{101, 4, id 12}`.

7 pairs × 4 + 2 = **30** stamps — this is the only world that fills `MAP_NUMBER` exactly.

`Role::action` lets the player jump every frame. `hitMap(..., world == 3)` sets `died` on any solid that is not a cloud, unless `isShoot` (weapon/star) is already on.

There is no `srand` in the project. On MSVC the first `rand()` sequence is typically seed 1, so a given Visual Studio build produces the same pipes every launch. Another CRT will place them differently. The example generator can replay a seed.

Coins: seven tiles `(5+10*i, random(3,7))`. Food is the suspicious pair `{10, 10}` — those look like **tile** numbers stored in a field that worlds 1–2 treat as **pixels**, so the pickup sits near the origin and is easy to miss.

Enemies are fixed tile positions, not tied to the random heights, so a short pipe can leave a walker floating until it turns around.

Ending rail: 94 tiles.

## How to author a stamp

A ground slab from tile column 0 to 14, row 9, grass, one tile tall:

```
{ 0, 9, 1, 15, 1 }
```

`xAmount = 15` draws fifteen 32 px tiles. Collision is the same rectangle.

A 4×1 cloud at `(10, 6)`:

```
{ 10, 6, 2, 4, 1 }
```

Keep the total number of stamps per world at 30 or fewer, or extend `MAP_NUMBER` in both `scene.h` and any example copy.

## Scoring from the level

| Event | Score |
| --- | ---: |
| Coin | +10 |
| Stomp enemy | +5 |
| Bullet hits enemy | +5 |

Food does not add score; it only sets `isShoot` and plays `吃到武器.mp3`.

## Save file vs world index

`gameRecord.dat` stores the integer `world` (1–3). It does not store score, lives, or world-3 RNG. Loading a save starts that world’s constructor from scratch. Details in [controls-and-save.md](controls-and-save.md).
