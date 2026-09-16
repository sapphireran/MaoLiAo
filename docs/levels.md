# Levels

Worlds are not data files. `Scene::createMap`, `Scene::createCoin`, `Scene::createFood`, and `Role::createEnemy` each switch on `world` and copy C++ aggregate literals into fixed arrays:

| Array | Cap | Authoring units |
| --- | --- | --- |
| `Map map[30]` | `MAP_NUMBER` | tiles (`x`, `y`, `xAmount`, `yAmount`) plus `id` and friction `u` |
| `POINT coins[70]` | `COINS_NUMBER` | tiles |
| `POINT food[5]` | `FOOD_NUMBER` | pixels on worlds 1–2; leftover `{10,10}` on world 3 |
| `Enemy myEnemy[30]` | `ENEMY_TOTE` | tiles, then `* WIDTH/HEIGHT` at spawn |

`examples/maoliao_sim/worlds.py` is a faithful extract of the literals so ASCII maps and tests do not re-parse C++.

## Tile record

```
{ x, y, id, xAmount, yAmount }     // u is filled by createMap
```

`x` / `y` are the top-left tile. The block occupies `xAmount` × `yAmount` tiles of 32×32 px, except:

- `id == 7` is drawn 64×32 (pipe shaft).
- `id == 8` or `10` is drawn and collided as 64×64 (pipe mouth).

Only `1 <= id < 11` participates in `hitMap`. IDs 11–14 are scenery (`scenery.bmp` frames).

## Tile IDs (comments in `scene.cpp`)

| id | Art | Collision | Typical `u` (worlds 1–2) |
| --- | --- | --- | --- |
| 1 | Grass brick | yes | mid (`T2`) |
| 2 | Cloud | yes | mid |
| 3 | Grass / desert underground | yes | mid |
| 4 | Snow underground | yes | mid |
| 5 | Snow / desert surface | yes | mid |
| 6 | Pipe middle (also used as an icy strip on world 1) | yes | high (`T1`) |
| 7 | Vertical pipe body (world 3) | yes | low (`T3`) unless world 3’s solid rule |
| 8 | Downward pipe mouth (world 3) | yes, 2×2 | low / world-3 high |
| 9 | Unused in current literals | — | — |
| 10 | Upward pipe mouth | yes, 2×2 | low |
| 11 | Background grass | no | — |
| 12 | Goal sign | no | — |
| 13 | Water | no | — |
| 14 | Tree | no | — |

World 3 overrides friction: ids 1 and 3–6 use `T1` (high). Id 2 (cloud) stays `T2`. Everything else uses `T3`.

World 3 also special-cases collision in `Role::hitMap`: any solid that is not id 2 kills the hero unless `isShoot` is already true (the “star / invincible mushroom” comment in the source).

## World 1 — grassland

Length trigger: 94 tiles (3008 px). Goal sign at tile `(101, 7)`.

Solid floors (id, then underground filler):

- Grass from x 0–14 at y 9, underground id 3 for two tiles.
- Raised grass 18–26 at y 8.
- Long snow-surface strip 27–63 at y 9 (id 5) over id 4 fill.
- Short high-friction strip 67–70 at y 9 (id 6).
- One-tile snow 74,9.
- Closing grass 78–105 at y 9.

Clouds (id 2): `(10,6)×4`, `(40,4)×3`, `(62,6)×1`, `(65,4)×5`.

Pipes (id 10): `(36,7)`, `(45,7)`, `(80,7)`, `(90,7)`.

Coins sit on the first cloud, the high cloud at x 66–69, a vertical stack at x 74, and a zigzag over the late grass.

One weapon mushroom at pixel `(14*32, 5*32) = (448, 160)`.

Ten enemies; several start on the opening grass and the late platforms.

Water scenery (id 13) marks the pits at x 15, 64, 71, 75.

## World 2 — sky islands

Length trigger: 104 tiles (3328 px). Goal sign authored at `(111, 1)` — but it sits past the `MAP_NUMBER` (30) copy cap, so V2.0 never draws it. Trees and the x=42 grass tuft are in the same dropped tail. Clear the stage by walking far enough for `isEnding`, not by touching the sign.

The floor is broken snow (id 5) with underground id 4. Cloud stepping stones climb toward a 12-tile cloud runway at `(102, 3)`. A staircase of 1-tile clouds at x 67–70 and a matching pillar at x 75 form a gap you can fall through.

Coins: three on the high cloud at y 2, a vertical column at x 32, and three hanging over the isolated floor tiles at x 50 / 53 / 56.

Weapon mushroom at `(39*32 - 32/3, 3*32 + 32/5) ≈ (1238, 102)`.

Six enemies, including a pair on the mid clouds and two near the goal runway.

Trees (id 14) at x 4, 9, 83, 89.

## World 3 — pipe gauntlet

Generated at runtime. Ten candidate columns at `x[i] = 10 + 10*i`. Each column draws a random height `h = random(1, 7)` (i.e. `rand() % 6 + 1`) and four tiles:

```
{ x, h-2, id 8, 1, 1 }              // downward mouth
{ x, 0,   id 7, 1, max(0, h-2) }    // upper shaft
{ x, 4+h, id 10, 1, 1 }             // upward mouth
{ x, h+6, id 7, 1, max(0, 6-h) }    // lower shaft
```

Only the first seven columns are live; columns 7–9 are commented out. After the pipes, a 25-tile cloud at `(80, 6)` and a goal sign at `(101, 4)` finish the stage.

Coins: seven points at x = 5, 15, …, 65 with independent random y in `[3, 6]`.

Food is `{10, 10}` pixels — almost under the spawn, easy to miss, and probably leftover.

Enemies are a fixed seven-point list, not randomized.

Infinite jump is on. Touching any non-cloud solid kills you unless you already picked up the weapon.

`createMap`’s loop is `while (i <= sizeof(m)/sizeof(m[0]))`, one step past the end of `m`. That is a real overrun; see [known-issues.md](known-issues.md).

## How to edit a world

1. Open `MaoLiAo/scene.cpp` (`createMap` / `createCoin` / `createFood`) or `MaoLiAo/role.cpp` (`createEnemy`).
2. Keep `MAP_NUMBER` (30) in mind. World 1 already uses 32 `Map` initializers — the `while (id > 0 && id < 15 && i < MAP_NUMBER)` copy **silently drops** the last two water tiles.
3. After changing literals, copy the same tuples into `examples/maoliao_sim/worlds.py` and re-run `python3 examples/tests/test_all.py` plus `python3 examples/levels/ascii_map.py`.
4. Ending distance is independent of the goal-sign tile. If you lengthen a stage, update `Scene::isEnding` and `examples/maoliao_sim/worlds.py` together.

## Scoring

| Event | Points |
| --- | --- |
| Coin | +10 |
| Stomp enemy (`vY > 0`) | +5 |
| Bullet hits enemy | +5 |
| Weapon mushroom | 0 (enables `J`) |

There is no time bonus and no 1-up. Score is a `Role` field and resets when the object is reconstructed.

## ASCII preview

```
python3 examples/levels/ascii_map.py --world 1
python3 examples/levels/ascii_map.py --world 2
python3 examples/levels/ascii_map.py --world 3 --seed 2020
```

Legend is printed by the script. World 3 needs `--seed` because the C++ `random` macro is unseeded `rand()` in the real game.
