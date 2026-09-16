# 10 — Levels

Three worlds. Layouts are literal arrays in `Scene::createMap`, `createCoin`, `createFood`, and `Role::createEnemy`. Coordinates are **tile columns / rows** unless noted.

The screen is 16 tiles wide (`512/32`) and 12 tiles tall (`384/32`). Row 0 is the top of the window.

## Shared clear rule

```text
distance = -hero.x0 + hero.x     // world-space x of the cat
if Scene::isEnding(distance): hero.ending = true
if hero.x > XSIZE:              hero.passed = true
```

| World | `isEnding` distance | Victory sign column |
| --- | --- | --- |
| 1 | 94 tiles (3008 px) | 101 |
| 2 | 104 tiles (3328 px) | 111 |
| 3 | 94 tiles (3008 px) | 101 |

## World 1 — grassland

Ground runs (surface + underground filler):

| x | y | id | xAmt | yAmt | Role |
| --- | --- | --- | --- | --- | --- |
| 0 | 9 | 1 | 15 | 1 | Grass from the left |
| 0 | 10 | 3 | 15 | 2 | Dirt under it |
| 18 | 8 | 1 | 9 | 1 | Raised island |
| 18 | 9 | 3 | 9 | 3 | Dirt |
| 27 | 9 | 5 | 37 | 1 | Long snow-looking mid stretch |
| 27 | 10 | 4 | 37 | 2 | Snow dirt |
| 67 | 9 | 6 | 4 | 1 | Sticky pipe-mid floor |
| 67 | 10 | 3 | 4 | 2 | Dirt |
| 74 | 9 | 5 | 1 | 1 | One-tile step |
| 74 | 10 | 4 | 1 | 2 | Dirt |
| 78 | 9 | 1 | 28 | 1 | Home stretch |
| 78 | 10 | 3 | 28 | 2 | Dirt |

Clouds (id 2) at `(10,6)×4`, `(40,4)×3`, `(62,6)×1`, `(65,4)×5`.

Upward pipes (id 10) at columns 36, 45, 80, 90 (row 7).

Scenery grass (id 11) at 19/23/32/49/53/57/93. Water (id 13) in the pits at columns 15, 64, 71, 75 row 10.

Coins: a row on the first cloud `(10–13, 5)`, a row on the high cloud `(66–69, 3)`, a vertical stack on the one-tile step `(74, 5–8)`, then a zigzag `(82–89)`.

Mushroom: world pixel `(448, 160)` — on the way to the first raised island.

Enemies: ten patrols, including one on the high cloud at `(68, 3)`.

Pits: the gap after column 15, the 67–78 broken floor (pipe / single tile / gap), and anything below row 12.

## World 2 — snow platforms

Walkway fragments at row 10 (id 5) with id 4 filler at row 11: `[0,15)`, `[38,48)`, singles at 50, 53, 56, `[61,71)`, single 75, `[82,93)`.

Floating clouds (id 2) make the real path: `(18,9)×4`, `(23,6)×6`, `(25,3)×3`, `(31,10)×2`, singles at `(39,7)` and `(39,4)`, then late clouds at 96/99/102.

A rising staircase: `(67,9)`, `(68,8)×2 tall`, `(69,7)×3`, `(70,6)×4`, and a matching pillar at `(75,6)×4`.

Trees (id 14) at 4, 9, 83, 89 row 8. One grass tuft at `(42,8)`. Victory sign at `(111,1)`.

Coins: top cloud `(25–27, 2)`, a vertical shaft at column 32 rows 4–8, and three coins above the singles at 50/53/56 row 6.

Mushroom: roughly `(1237, 102)`, on the high single cloud at column 39.

Enemies: six, including two on the late clouds.

This stage is longer (`104` tiles to trigger ending) and uses air time more than world 1.

## World 3 — pipe gauntlet

On construct, ten random heights `random(1, 7)` and ten x positions `10, 20, …, 100`. Only the first **seven** pairs are emitted (the last three are commented out).

Each pair:

```text
(x, height-2, id 8)           // top mouth
(x, 0,        id 7, yAmt = max(0, height-2))   // top shaft
(x, 4+height, id 10)          // bottom mouth
(x, height+6, id 7, yAmt = max(0, 6-height))   // bottom shaft
```

Gap between the mouths is four tiles (`4+height` vs `height-2` → 6 tile centers, 4 tile hole). Heights 1–6 move that hole up and down.

Landing strip: cloud run `{80, 6, id 2, 25 tiles}` and a victory sign at `(101, 4)`.

Coins: seven, x = 5,15,…,65, y = `random(3,7)` — independent of the pipe heights, so they can sit inside a pipe.

Mushroom: `(10, 10)` px. Food hit box is 52×25, so it is a sliver at the top-left of the window.

Enemies: seven mid-air patrols. They do not use the world-3 death rule (probes pass `world = 1`).

**Feel:** hold jump to flutter, treat every pipe as lethal, race to the cloud runway at column 80.

## Editing a layout

1. Keep `MAP_NUMBER` (30) in mind. World 3 already uses 7×4 + 2 = 30 slots.
2. Set `u` via the same `id → Tn` switch, or friction will default to T3.
3. Place coins / food / enemies in the matching `create*` function — they are not derived from the map.
4. If you lengthen a stage, raise the `isEnding` threshold and move the id-12 sign.

`examples/07_world3_pipes` prints a text dump of one randomized gauntlet. `examples/06_level_end` checks the three distance thresholds.
