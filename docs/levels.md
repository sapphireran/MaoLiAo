# Levels

Tile records are `{ x, y, id, xAmount, yAmount }` in **tile units** (32 px). `u` is filled by the friction switch in `createMap`. Caps:

| Pool | Cap | Notes |
|---|---|---|
| `Map map[MAP_NUMBER]` | **30** | worlds 1–2 author more than 30; extras never copy |
| `POINT coins[COINS_NUMBER]` | 70 | plenty; loops use `<= sizeof` (one garbage write) |
| `POINT food[FOOD_NUMBER]` | 5 | one pickup per world |
| `Enemy myEnemy[ENEMY_TOTE]` | 30 | authored 6–10 |

Ending trigger (`Scene::isEnding`) uses the hero’s world x `(-x0 + x)`:

| World | Distance | Pixels |
|---|---|---|
| 1 | `> 94 * WIDTH` | `> 3008` |
| 2 | `> 104 * WIDTH` | `> 3328` |
| 3 | `> 94 * WIDTH` | `> 3008` |

After that the hero auto-walks right until `x > 512`.

`examples/src/world_atlas.cpp` prints the stored (clipped) tables and checks counts.

## Tile ids

| id | Draw | Collide? | Typical `u` (worlds 1–2) |
|---|---|---|---|
| 1 | grass top | yes | T2 |
| 2 | cloud | yes (safe in world 3) | T2 |
| 3 | grass fill | yes | T2 |
| 4 | snow fill | yes | T2 |
| 5 | snow top | yes | T2 |
| 6 | slick slab | yes | T1 (grippy / short stop) |
| 7 | pipe shaft | yes (32×32 box, 64 px blit) | T3 |
| 8 | inverted pipe mouth | yes, **2×** box | T3 |
| 9 | (sheet row; unused in data) | — | — |
| 10 | upright pipe mouth | yes, **2×** box | T3 |
| 11 | swaying grass | no | — |
| 12 | goal sign | no | — |
| 13 | water | no | — |
| 14 | tree | no | — |

## World 1 — grassland

32 authored tiles, **30 stored**. The last two water decorations (`x=71` and `x=75`) are dropped by `i < MAP_NUMBER`.

**Ground (id 1 / 5 / 6) + fill (id 3 / 4)**

| x | y | id | w | h | What |
|---|---|---|---|---|---|
| 0 | 9 | 1 | 15 | 1 | grass top |
| 18 | 8 | 1 | 9 | 1 | raised grass |
| 27 | 9 | 5 | 37 | 1 | long snow-top run |
| 67 | 9 | 6 | 4 | 1 | slick T1 slab |
| 74 | 9 | 5 | 1 | 1 | single snow cell |
| 78 | 9 | 1 | 28 | 1 | grass to the flag |
| 0 | 10 | 3 | 15 | 2 | dirt under first pad |
| 18 | 9 | 3 | 9 | 3 | dirt under raised pad |
| 27 | 10 | 4 | 37 | 2 | snow fill |
| 67 | 10 | 3 | 4 | 2 | dirt under slick slab |
| 74 | 10 | 4 | 1 | 2 | snow fill |
| 78 | 10 | 3 | 28 | 2 | dirt to the end |

**Clouds (id 2)** — `(10,6)×4`, `(40,4)×3`, `(62,6)×1`, `(65,4)×5`

**Pipes (id 10)** — `(36,7)`, `(45,7)`, `(80,7)`, `(90,7)`

**Scenery** — grass id 11 at 19/23/32/49/53/57/93; flag id 12 at `(101,7)`; water id 13 at 15, 64, **71 (clipped)**, **75 (clipped)**

**Coins (tile)** — four on the first cloud `(10–13, 5)`; four on `(66–69, 3)`; a vertical stack at `x=74`, `y=5..8`; a zigzag `82..89`

**Enemies (tile, facing)** — `(3,8,+1) (18,7,-1) (25,7,+1) (28,8,-1) (33,8,-1) (39,8,+1) (68,3,+1) (66,8,-1) (81,6,+1) (92,6,+1)`

**Food** — `(448, 160)` (on the first cloud line)

## World 2 — snow / stairs

36 authored tiles, **30 stored**. Clipped: grass tuft `(42,8,11)`, **goal sign `(111,1,12)`**, four trees. The clear is still the distance test at 104 tiles, so the missing flag is cosmetic — but the sign never draws.

**Clouds** — floating pads at x 18, 23, 25, 31, 39 (two heights), 96, 99, and a long runway `(102,3)×12`

**Snow floor** — tops id 5 at y=10 (chunks 0–14, 38–47, singles at 50/53/56, 61–70, 75, 82–92); fill id 4 at y=11, height 2

**Stair / pillar** — `(67,9)`, `(68,8)h2`, `(69,7)h3`, `(70,6)h4`, `(75,6)h4`

**Coins** — `(25–27,2)`; vertical `(32,4..8)`; `(50,6)(53,6)(56,6)`

**Enemies** — `(18,8,+1) (24,5,+1) (28,5,-1) (44,9,+1) (97,6,+1) (99,4,+1)`

**Food** — `(1238, 102)` (integer `39*32 - 32/3`, `3*32 + 32/5`)

## World 3 — pipe field

Not a hand-placed map. `createMap(3)`:

```
height[i] = random(1, 7)    # rand() % 6 + 1  →  1..6
x[i]      = i * 10 + 10     # 10, 20, …, 100
```

Only groups `i = 0..6` are compiled in (7–9 are commented). Each group emits four tiles:

```
{ x, height-2,     8, 1, 1 }                 # inverted mouth
{ x, 0,            7, 1, max(0, height-2) }  # shaft from the ceiling
{ x, 4+height,    10, 1, 1 }                 # upright mouth
{ x, height+6,     7, 1, max(0, 6-height) }  # shaft toward the floor
```

Then a cloud runway `{ 80, 6, 2, 25, 1 }` and a flag `{ 101, 4, 12, 1, 1 }`.

7×4 + 2 = **30** tiles — fills `MAP_NUMBER` exactly. The copy loop is `i <= sizeof(m)/sizeof(m[0])` (31 iterations): one read/write past both arrays. See [quirks.md](quirks.md).

**Coins** — seven, x = 5,15,…,65, y = `random(3,7)` ∈ {3,4,5,6}

**Enemies** — `(24,4,-1) (34,6,+1) (37,3,-1) (43,5,-1) (63,7,+1) (67,8,-1) (86,5,+1)`

**Food** — `(10, 10)`

Rules unique to this world: infinite jump; any non-cloud tile kills unless `isShoot`.

`examples/src/pipe_field.cpp` rebuilds one deterministic field (seeded `rand`) and checks the 30-slot occupancy.

## ASCII sketch (world 1, stored tiles only)

Column is tile x, row is tile y. `#` = solid (id 1–10), `c` = cloud, `P` = pipe mouth, `F` = flag. Not to scale for pipe 2× boxes.

```
y\x    0        10        20        30        40        50        60        70        80        90        100
 4                         ccc                           ccccc
 6           cccc                                        c              P
 7                                   P        P                                P         P  #grass   F
 8                   #########
 9     ###############         #####################################~~~~#  ############################
10     (fill under the tops; water scenery at 15 and 64 only — 71/75 clipped)
```

The `~~~~` slab at x=67 is id 6 (T1 friction).
