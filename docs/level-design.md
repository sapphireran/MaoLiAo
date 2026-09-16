# Level design

A world is three parallel tables filled in `Scene::createMap`, `Scene::createCoin`, `Scene::createFood`, plus `Role::createEnemy`. There is no external map file at runtime. The JSON under `examples/data/` is a **mirror** of those tables for offline tools.

## `struct Map`

```cpp
struct Map {
    int x;        // tile column (world origin)
    int y;        // tile row (0 = top of the 384 px window)
    int id;       // sprite + behaviour (see tile-catalog.md)
    int xAmount;  // repeat count horizontally
    int yAmount;  // repeat count vertically
    double u;     // friction; filled after the table, not authored
};
```

`y` is in tiles of 32 px. The window is `YSIZE/HEIGHT = 12` tiles tall (0..11). World 1’s main floor sits at `y = 9` with underground filler at `y = 10`.

`MAP_NUMBER` is **30**. `createMap` copies until `id` is outside `1..14` **or** `i == 30`. World 1’s C array has **32** records; the last two water tiles (`{71,10,13}` and `{75,10,13}`) **never load**. World 2’s array has **36** records; trees and the goal sign after index 29 never load. The lint example flags this.

World 3 uses a different loop (`i <= sizeof(m)/sizeof(m[0])`) which **over-reads** by one and can write a 31st slot if the stack junk has a plausible `id` — another reason the offline generator exists.

## World 1 — grassland

Authored as a long walk to the right. Ending distance: **94 tiles**.

Solid bands (id 1 grass top, id 5 snow-looking top, id 6 high-friction top; underground 3/4):

| x | y | id | xAmount | yAmount | Notes |
| --- | --- | --- | --- | --- | --- |
| 0 | 9 | 1 | 15 | 1 | Starting floor |
| 18 | 8 | 1 | 9 | 1 | Raised island |
| 27 | 9 | 5 | 37 | 1 | Long mid section |
| 67 | 9 | 6 | 4 | 1 | High-friction strip |
| 74 | 9 | 5 | 1 | 1 | Single tile |
| 78 | 9 | 1 | 28 | 1 | Runway to the goal |

Clouds (id 2) at `(10,6)×4`, `(40,4)×3`, `(62,6)×1`, `(65,4)×5`.

Upward pipe mouths (id 10) at columns 36, 45, 80, 90 (all `y=7`).

Goal sign id 12 at `(101, 7)` — past the 94-tile ending trigger, so the hero is already auto-walking when it appears.

Coins: a row on the first cloud `(10–13, 5)`, a row on the high cloud `(66–69, 3)`, a vertical stack at column 74 rows 5–8, then a zigzag on the last floor `(82–89)`.

Enemies (tile coords, `turn` +1 right / −1 left):

`(3,8,+1), (18,7,−1), (25,7,+1), (28,8,−1), (33,8,−1), (39,8,+1), (68,3,+1), (66,8,−1), (81,6,+1), (92,6,+1)`

Food: one mushroom at pixel `(14*32, 5*32)` — on the first cloud, next to the four coins.

## World 2 — snow / platforms

Ending distance: **104 tiles**. Heavier use of id 5 (snow surface) and id 4 underground, plus floating id 2 platforms.

A staircase of 1-wide clouds at columns 67–70 (`y` 9,8,7,6 with `yAmount` 1,2,3,4) and a matching pillar at `(75,6)` with `yAmount` 4.

Goal sign authored at `(111, 1)` but that record is **index 31**, past `MAP_NUMBER`, so it does not appear in a faithful 30-slot copy. The lint tool reports it as truncated.

Enemies: `(18,8), (24,5), (28,5), (44,9), (97,6), (99,4)`.

Food: pixel `(39*WIDTH - WIDTH/3, 3*HEIGHT + HEIGHT/5)` ≈ `(1237, 102)`.

## World 3 — pipe gauntlet

Not a hand-placed tile list. `createMap` does:

```
for i in 0..9:
    height[i] = random(1, 7)     // 1..6 inclusive (C `rand()%(b-a)+a`)
    x[i]      = i * 10 + 10      // 10, 20, …, 100
```

Only **i = 0..6** are actually emitted (the table for 7, 8, 9 is commented out). Each used column emits four records:

1. Upper mouth: `{x, height-2, id 8, 1, 1}`
2. Upper body: `{x, 0, id 7, 1, max(0, height-2)}`
3. Lower mouth: `{x, 4+height, id 10, 1, 1}`
4. Lower body: `{x, height+6, id 7, 1, max(0, 6-height)}`

Then a cloud runway `{80, 6, id 2, 25, 1}` and a goal `{101, 4, id 12, 1, 1}`.

`random(a,b)` is `#define random(a,b) (rand()%(b-a)+a)` and **`srand` is never called**, so the first world-3 layout of a process is whatever the CRT default seed produces. Reconstructing `Scene` after death draws a **new** layout if `rand` has advanced (it has, from previous world-3 construction and any other `rand` use — only world 3 calls it).

Coins: seven tiles at x = 5,15,…,65 with `y = random(3,7)` (3..6).

Enemies: seven authored walkers, not random.

## How to add a platform (checklist)

1. Keep `id` in `1..10` if it must collide.
2. Stay under 30 **total** map records per world, including underground fillers.
3. Put pickups in `createCoin` / `createFood` with the correct units (tiles vs pixels).
4. Put walkers in `createEnemy` as tile coords; they are multiplied by 32 at spawn.
5. If the stage should end later, raise the `isEnding` distance **and** extend a solid floor so the auto-walk has ground.
6. Re-export `examples/data/worlds.json` and run `python3 examples/python/cli.py lint`.

## ASCII preview

```bash
python3 examples/python/cli.py preview --world 1
python3 examples/python/cli.py preview --world 3 --seed 1
```

Legend is printed with the map. The preview uses the same 30-slot cap as the game.
