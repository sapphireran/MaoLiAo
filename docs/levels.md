# Levels

Maps are authored as arrays of `Map { x, y, id, xAmount, yAmount, u }` in `Scene::createMap`.
Coordinates are **tile units** (multiply by 32 for pixels). `xAmount` / `yAmount` tile the same
id into a rectangle. Collision ids 8 and 10 (pipe mouths) double that box to 64×64.

`MAP_NUMBER` is 30. Worlds 1 and 2 list more than 30 entries; the copy loop stops at 30 and the
trailing scenery never appears. World 3 is authored to fit.

## Tile IDs (`map.bmp` rows)

| id | Art | Collision | Friction time |
| --- | --- | --- | --- |
| 1 | Grass top | yes | T2 (mid) |
| 2 | Cloud / floating platform | yes | T2 |
| 3 | Dirt under grass | yes | T2 |
| 4 | Snow underground | yes | T2 |
| 5 | Snow top | yes | T2 |
| 6 | Pipe shaft | yes | T1 (high) |
| 7 | Pipe extension (world 3) | yes | T3 (low) unless grouped with 1–6 |
| 8 | Inverted pipe mouth | yes (2×2) | T3 default |
| 9 | (unused in v2 layouts) | — | — |
| 10 | Upright pipe mouth | yes (2×2) | T3 default |
| 11 | Background grass | no (`id >= 11`) | — |
| 12 | Goal sign | no | — |
| 13 | Water | no | — |
| 14 | Tree | no | — |

`hitMap` only tests `id` in `(0, 11)`. Scenery never blocks. World 3 then treats a hit as death
when `id != 2` and the hero has not picked up the weapon.

Friction assignment in worlds 1–2:

```text
1, 3, 4, 5, 2 → u = (V_MAX / T2) / G
6             → u = (V_MAX / T1) / G
else          → u = (V_MAX / T3) / G
```

World 3 puts 1 and 3–6 on T1 (the pipe field is meant to feel sticky) and keeps id 2 on T2.

## World 1 — grassland

Ground strips (tops + matching underground):

- `[0, 15)` at y=9 grass / y=10 dirt
- `[18, 27)` raised one tile
- `[27, 64)` snow-top mix at y=9
- `[67, 71)` pipe-shaft feel (id 6)
- lonely tile at 74
- `[78, 106)` grass to the goal

Clouds at (10,6)×4, (40,4)×3, (62,6), (65,4)×5. Upright pipes at 36, 45, 80, 90. Goal sign at
(101, 7). Water decorations at 15, 64, 71, 75 — the last two are past the 30-tile cap.

**Enemies** (tile x, y, facing): (3,8,+), (18,7,−), (25,7,+), (28,8,−), (33,8,−), (39,8,+),
(68,3,+), (66,8,−), (81,6,+), (92,6,+).

**Coins:** cloud row 10–13 y=5; roof 66–69 y=3; pole 74 y=5..8; zigzag 82–89.

**Weapon:** (14, 5) in pixels `14*32, 5*32`.

**End walk** starts at world X > 94×32 = 3008.

## World 2 — floating snow

Long ground at y=10 with holes (gaps at 15–38, 48, 51, 54–56, 57–61, 71–75, 76–82). Cloud
staircases around x=67–75. High cloud shelf at (102, 3)×12 leading to the sign at (111, 1).

**Enemies:** (18,8,+), (24,5,+), (28,5,−), (44,9,+), (97,6,+), (99,4,+).

**Coins:** 25–27 y=2; vertical stack at x=32 y=4..8; three hovering over the isolated ground
tiles at 50, 53, 56 y=6.

**Weapon:** visually nudged to `(39*32 - 32/3, 3*32 + 32/5)`.

**End walk** at 104 tiles (3328 px). The authored map is 36 tiles; the 30-slot pool keeps the
solids and the cloud staircase, then drops background grass, the goal sign, and the trees. The
stage still clears because `isEnding` is a distance check, not a sign-touch.

## World 3 — Flappy pipes

Ten random heights `random(1,7)` and x = `i*10 + 10` are generated, but only the first **seven**
pipe groups are compiled in. Each group is four tiles: inverted mouth, upper shaft, lower mouth,
lower shaft, with a 4-tile gap. A long id-2 cloud runway at (80, 6)×25 and a sign at (101, 4)
finish the stage.

Coins sit at x = 5, 15, …, 65 with random y in `[3, 7)`. The weapon is at pixel (10, 10) — almost
under the spawn, so world 3 is intended to unlock shooting immediately if the player walks over it.

Mid-air jumps are always legal. Touching a pipe without the weapon is death; id 2 is safe.

**End walk** at 94 tiles, same as world 1.

## Sky strip

`mapsky.bmp` is loaded at `512 × (4*384)`. `yBg = -(world-1) * YSIZE` selects band 0, 1, or 2.
Horizontal wrap uses two blits of the same bitmap when `xBg <= -width`.

## Pools vs authored counts

| | W1 | W2 | W3 | Pool |
| --- | --- | --- | --- | --- |
| Map entries authored | 32 | 36 | 30 | 30 |
| Enemies | 10 | 6 | 7 | 30 |
| Coins | 20 | 11 | 7 | 70 |
| Food | 1 | 1 | 1 | 5 |

`examples/levels/world_catalog.cpp` encodes these tables and checks friction, ending distances,
and the 30-tile cap.
