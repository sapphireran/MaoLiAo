# Levels

Tiles, coins, food, and enemies are **C arrays**, not external maps. Capacities: `MAP_NUMBER = 30`, `COINS_NUMBER = 70`, `FOOD_NUMBER = 5`, `ENEMY_TOTE = 30`.

World 1 and 2 **define more than 30 map records**. `createMap` copies only while `i < MAP_NUMBER`, so the tail is dropped. `examples/map_layout` prints both the authored list and what fits in 30 slots.

## Tile ids

Comments in `Scene::createMap` (world 1):

| id | Art | Collision | Notes |
| ---: | --- | --- | --- |
| 1 | grass top | yes | `map.bmp` row 0 |
| 2 | cloud | yes | walkable; world 3 treats id 2 as the safe cloud runway |
| 3 | dirt under grass | yes | |
| 4 | snow fill | yes | |
| 5 | snow / grass top | yes | world 1 mid strip uses 5 |
| 6 | pipe shaft / ice strip | yes | world 1 `x=67` uses 6 → T1 friction |
| 7 | pipe extender | yes | world 3; drawn 64×32 |
| 8 | downward pipe mouth | yes | 64×64 hit box |
| 9 | (unused in current arrays) | — | |
| 10 | upward pipe mouth | yes | 64×64 hit box |
| 11 | background grass | **no** | `id < 11` is the hit filter |
| 12 | goal sign | **no** | scenery sheet |
| 13 | water | **no** | animated scenery |
| 14 | tree | **no** | animated scenery |

`Role::hitMap` only tests `0 < id < 11`. Decorations never block or kill.

World 3 extra rule: if `world == 3` and the tile id is **not** 2 and `isShoot == false`, any overlap sets `died`. The weapon pickup is therefore also a pipe-pass. Horizontal wall resolution calls `hitMap(..., 1)` so that death rule is skipped there; vertical / general hits use the real `world`.

## Finish line

`Scene::isEnding(distance)` with `distance = -x0 + x` (world x of the hero):

| World | distance > | pixels |
| ---: | ---: | ---: |
| 1 | 94 × 32 | 3008 |
| 2 | 104 × 32 | 3328 |
| 3 | 94 × 32 | 3008 |

Then `ending` auto-holds right, and `x > 512` sets `passed`.

## World 1 — grassland

Rough left-to-right:

- Ground: grass 0–14, gap, platform 18–26, long mixed strip 27–63, short ice 67–70, 1-tile 74, grass 78–105
- Fill under those tops (id 3/4)
- Clouds at y=6 / y=4
- Pipe mouths (10) at 36, 45, 80, 90
- Goal sign at tile (101, 7)
- Water decorations (several past slot 30 never spawn)

Coins (tile xy): `(10–13, 5)`, `(66–69, 3)`, vertical stack at `74`, zigzag `82–89`.

Food (pixels): `(14*32, 5*32)` — the shoot unlock.

Enemies (tile xy, facing):  
`(3,8,+1) (18,7,-1) (25,7,+1) (28,8,-1) (33,8,-1) (39,8,+1) (68,3,+1) (66,8,-1) (81,6,+1) (92,6,+1)`

## World 2 — snow / stairs

- Cloud islands (id 2) including a staircase `67–70` and a tall column at `75`
- Snow tops (5) and snow fill (4) with 1-tile gaps (50, 53, 56, 75)
- Goal at `(111, 1)` — needs the 104-tile finish distance
- Trees (14) are authored but several sit past `MAP_NUMBER`

Coins: top of the high cloud `(25–27, 2)`, vertical `(32, 4–8)`, three over the gap tiles `(50,53,56)` at y=6.

Food: `(39*32 - 32/3, 3*32 + 32/5)` — on the stacked clouds at x=39.

Enemies: `(18,8) (24,5) (28,5) (44,9) (97,6) (99,4)`.

## World 3 — pipes

Seven pipe pairs (the last three authored groups are commented out). For `i = 0..6`:

```text
x[i] = 10 + 10*i
height[i] = random(1, 7)     // 1..6 inclusive
```

Each pair: upper mouth (8) + shaft (7), gap, lower mouth (10) + shaft (7). Then a cloud runway `{80, 6, id 2, 25 tiles}` and a goal `{101, 4, id 12}`.

Coins: seven tiles at `x = 5,15,…,65` and `y = random(3, 7)`.

Food: `{10, 10}` — **pixels**, not tiles. That is near the top-left of the screen, easy to miss.

Enemies: `(24,4) (34,6) (37,3) (43,5) (63,7) (67,8) (86,5)` with mixed facing.

`rand()` is not seeded in the repo, so pipe gaps repeat until something else calls `srand`.

## Authoring a change

1. Edit the `Map m[]` / `POINT p[]` / `Enemy emy[]` in `scene.cpp` or `role.cpp`.
2. Keep `id` in `1..14` and stay under 30 **colliding + decorating** records you actually need, or raise `MAP_NUMBER` in `scene.h`.
3. Re-run `examples/map_layout` if you copy the tables into `examples/common/map_data.cpp` (the example tables are a snapshot for docs; the game arrays are the source of truth).
4. Goal `id 12` is scenery only — the real exit is `isEnding` + walking off the right of the window.
