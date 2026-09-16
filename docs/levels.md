# Levels

Worlds are integers `1`, `2`, `3` stored in the global `world` (`main.cpp`) and copied into `Scene::world`. Each constructor calls `createMap`, `createCoin`, `createFood`, and `Role::createEnemy`.

Tile coordinates are in **map cells** of 32×32 unless noted. A row is `{ x, y, id, xAmount, yAmount }` plus a computed `u`.

## Tile id legend

From comments in `Scene::createMap` and the blit paths in `Scene::show`:

| id | Name | Collides | Draw |
| --- | --- | --- | --- |
| 1 | Grass brick (top) | yes | 32×32 from `map.bmp` row 0 |
| 2 | Cloud | yes | 32×32 |
| 3 | Dirt under grass | yes | 32×32 |
| 4 | Snow underground | yes | 32×32 |
| 5 | Snow surface | yes | 32×32 |
| 6 | Pipe shaft / slick strip | yes | 32×32 |
| 7 | Pipe extension (world 3) | yes | 64×32 |
| 8 | Downward pipe mouth | yes (64×64 box) | 64×64 |
| 9 | (reserved in comments) | — | — |
| 10 | Upward pipe mouth | yes (64×64 box) | 64×64 |
| 11 | Background grass | no | 96×64 scenery frames |
| 12 | Victory sign | no | scenery |
| 13 | Water | no | scenery (animated) |
| 14 | Tree | no | scenery (animated) |

`map.bmp` is a vertical strip: source `y0 = (id - 1) * 32`. Ids ≥ 8 use wider blits; ids > 10 switch to `scenery.bmp` with a 3-frame sway (`scenery_iframe`).

## Clear distances

`Scene::isEnding(distance)` with `distance = -x0 + hero.x`:

| World | Distance | Cells (÷32) |
| --- | --- | --- |
| 1 | `94 * WIDTH` | 94 |
| 2 | `104 * WIDTH` | 104 |
| 3 | `94 * WIDTH` | 94 |

After that the hero auto-runs until `hero.x > 512`.

Sky row: `yBg = -(world - 1) * 384`, so `mapsky.bmp` is a 4-screen-tall strip (worlds 1–3 plus spare).

## World 1 — grassland

Theme: grass tops, dirt, a few clouds, pipes, water pits, a flag at cell 101.

**Ground / fill (colliding):**

| x | y | id | xAmt | yAmt | Notes |
| --- | --- | --- | --- | --- | --- |
| 0 | 9 | 1 | 15 | 1 | Starting lawn |
| 18 | 8 | 1 | 9 | 1 | Raised island |
| 27 | 9 | 5 | 37 | 1 | Long mid stretch (snow-top graphic on grass world) |
| 67 | 9 | 6 | 4 | 1 | High-friction strip |
| 74 | 9 | 5 | 1 | 1 | Single cell |
| 78 | 9 | 1 | 28 | 1 | Run to the flag |
| 0 | 10 | 3 | 15 | 2 | Dirt under start |
| 18 | 9 | 3 | 9 | 3 | Dirt under island |
| 27 | 10 | 4 | 37 | 2 | Fill |
| 67 | 10 | 3 | 4 | 2 | Fill |
| 74 | 10 | 4 | 1 | 2 | Fill |
| 78 | 10 | 3 | 28 | 2 | Fill |
| 10 | 6 | 2 | 4 | 1 | Cloud platform |
| 40 | 4 | 2 | 3 | 1 | High clouds |
| 62 | 6 | 2 | 1 | 1 | Cloud |
| 65 | 4 | 2 | 5 | 1 | Cloud runway (coins sit on y=3) |
| 36 | 7 | 10 | 1 | 1 | Pipe |
| 45 | 7 | 10 | 1 | 1 | Pipe |
| 80 | 7 | 10 | 1 | 1 | Pipe |
| 90 | 7 | 10 | 1 | 1 | Pipe |

**Decoration:** grass tufts at (19,6), (23,6), (32,7), (49,7), (53,7), (57,7), (93,7); flag (101,7); water at (15,10), (64,10), (71,10), (75,10).

**Coins (tile cells):** (10–13, 5) on the first cloud; (66–69, 3) on the high cloud; a vertical stack at x=74, y=5..8; a zigzag 82–89 on y=7/8.

**Food:** one pickup at `(14 * 32, 5 * 32)` — pixel space, sitting above the early lawn.

**Enemies** (`{cellX, cellY, facing}` then `* 32` for pixels):

```text
(3,8,+1) (18,7,-1) (25,7,+1) (28,8,-1) (33,8,-1)
(39,8,+1) (68,3,+1) (66,8,-1) (81,6,+1) (92,6,+1)
```

`MAP_NUMBER` is 30. World 1’s initializer lists **32** map rows. The last water tiles can be dropped or overflow; see [known-issues.md](known-issues.md).

## World 2 — snow / stepped clouds

Theme: snow crust (id 5) over snow fill (id 4), stair-step clouds, trees, a high flag.

**Clouds / stairs (excerpt):** (18,9), (23,6 w=6), (25,3 w=3), (31,10 w=2), stacked 1-wide columns at x=39, late clouds at 96 / 99 / 102, and a 4-step staircase at x=67–70 plus a matching column at (75,6).

**Snow ground:** segments at y=10 (surface) and y=11 (fill) covering 0–14, 38–47, singles at 50 / 53 / 56, 61–70, 75, 82–92.

**Decoration:** grass (42,8); flag (111,1); trees at (4,8), (9,8), (83,8), (89,8).

**Coins:** (25–27, 2) on the high cloud; a vertical column at x=32, y=4..8; three coins at (50,6), (53,6), (56,6) above the isolated snow posts.

**Food:** `(39 * 32 - 32/3, 3 * 32 + 32/5)` ≈ (1237, 102) — tucked on the tall cloud column.

**Enemies:** (18,8,+1), (24,5,+1), (28,5,-1), (44,9,+1), (97,6,+1), (99,4,+1).

World 2’s map array is **also longer than 30** entries. The `while (id > 0 && id < 15 && i < MAP_NUMBER)` stop prevents a hard overrun but silently drops trailing tiles (likely trees / flag). Treat 30 as a hard budget when editing.

## World 3 — pipe gauntlet

Built at runtime:

```text
for i in 0..9:
    height[i] = random(1, 7)   // 1..6 inclusive (rand() % 6 + 1)
    x[i]      = i * 10 + 10    // 10, 20, ..., 100
```

For each of the first **seven** columns (`x[0]`..`x[6]`) four tiles are emitted:

1. Upper mouth: `(x, height-2, id=8, 1, 1)`
2. Upper shaft: `(x, 0, id=7, 1, max(0, height-2))`
3. Lower mouth: `(x, 4+height, id=10, 1, 1)`
4. Lower shaft: `(x, height+6, id=7, 1, max(0, 6-height))`

Columns 7–9 exist in comments but are disabled. A long cloud runway `{80, 6, id=2, 25, 1}` and a flag `{101, 4, id=12}` finish the level.

The gap between upper and lower mouths is about four cells — flap through it. Touching any non-cloud collider without food kills.

**Coins:** seven cells at x = 5,15,…,65 with independent random y in `[3, 6]`.

**Food:** `{10, 10}` in **pixel** space (almost at the origin). Easy to miss; picking it up is the star / gun and also pipe immunity.

**Enemies:** (24,4,-1), (34,6,+1), (37,3,-1), (43,5,-1), (63,7,+1), (67,8,-1), (86,5,+1).

The copy loop is `while (i <= sizeof(m)/sizeof(m[0]))` — one past the last element. See known issues.

`random(a,b)` is `rand()%(b-a)+a`. `srand` is never called, so world 3 layouts repeat across runs on a given CRT.

## Capacity constants

| Constant | Value | Used for |
| --- | --- | --- |
| `MAP_NUMBER` | 30 | `Scene::map` |
| `COINS_NUMBER` | 70 | plenty for current layouts |
| `FOOD_NUMBER` | 5 | one live pickup per world |
| `SCORE_NUMBER` | 5 | concurrent collect sparks |
| `ENEMY_TOTE` | 30 | enemy slots |
| `BULLET_NUMBER` | 30 | live shots |
| `BOMB_NUMBER` | 5 | explosions |

When you add tiles, count the initializer **and** stay under 30, or raise `MAP_NUMBER` in `scene.h`.
