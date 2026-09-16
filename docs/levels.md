# Levels

Worlds are not data files. `Scene::createMap`, `createCoin`, `createFood`, and `Role::createEnemy` each switch on `world` and copy from a stack array. The tile buffer is `Map map[30]`. Anything past the 30th entry is dropped. World 1 authors 32 tiles and world 2 authors 36 — the last rows never appear in play. World 3 authors 30 on purpose (the extra pipe clusters are commented out).

## Tile ids

From the comments in `scene.cpp` and the blit paths in `Scene::show`:

| `id` | Art | Collides | Typical `u` (worlds 1–2) |
| --- | --- | --- | --- |
| 1 | Grass surface | yes | `T2` |
| 2 | Cloud | yes | `T2` |
| 3 | Dirt / grass underground | yes | `T2` |
| 4 | Snow underground | yes | `T2` |
| 5 | Snow surface | yes | `T2` |
| 6 | Pipe shaft (also used as a slick? no — **high** friction) | yes | `T1` |
| 7 | Pipe shaft, drawn 64 px wide | yes, 32 px hit | `T3` default |
| 8 | Downward pipe mouth (world 3) | yes, 64×64 | `T3` |
| 9 | (reserved in comments; unused in the live arrays) | — | — |
| 10 | Upward pipe mouth | yes, 64×64 | `T3` |
| 11 | Background grass | no | copied, unused |
| 12 | Victory sign | no | copied, unused |
| 13 | Water (animated scenery) | no | copied, unused |
| 14 | Tree (animated scenery) | no | copied, unused |

`show()` special-cases `id == 7` (two tiles wide from `map.bmp`) and `id > 7` (either a 64×64 pipe mouth from `map.bmp`, or a 96×64 frame from `scenery.bmp` indexed by `id - 11`). Water and trees therefore animate; the victory sign is a still frame in that same strip.

Each `Map` record is `{ tileX, tileY, id, xAmount, yAmount }` plus `u` filled in by the switch. A platform that is 15 tiles wide and 1 tile tall is one record, not fifteen.

## Finish line

`Scene::isEnding(distance)` compares world-space pixels (`hero.x - (int)hero.x0`) to a multiple of `WIDTH`:

| World | Threshold | Pixels |
| --- | --- | --- |
| 1 | `94 * 32` | 3008 |
| 2 | `104 * 32` | 3328 |
| 3 | `94 * 32` | 3008 |

After `ending` is set the hero auto-runs until `x > 512`, then `passed` flips. The victory sign is authored near those columns (world 1 at tile 101, world 2 at tile 111, world 3 at tile 101) but the sign itself does not collide and is not the trigger.

## World 1 — grass

Surface band around row 9, dirt under it, a few cloud steps, four upward pipes, coin clusters on the left clouds and on the late staircase, water scenery in the pits, victory sign at `(101, 7)`.

```
tileX  tileY  id  xAmt  yAmt   notes
    0      9   1    15     1   grass
   18      8   1     9     1   raised grass
   27      9   5    37     1   long snow-colored cap (art id 5)
   67      9   6     4     1   high-friction pipe tops
   74      9   5     1     1   single cap
   78      9   1    28     1   grass to the sign
    0     10   3    15     2   dirt
   18      9   3     9     3
   27     10   4    37     2
   67     10   3     4     2
   74     10   4     1     2
   78     10   3    28     2
   10      6   2     4     1   clouds
   40      4   2     3     1
   62      6   2     1     1
   65      4   2     5     1
   36      7  10     1     1   pipes
   45      7  10     1     1
   80      7  10     1     1
   90      7  10     1     1
   19      6  11     1     1   grass tufts (no hit)
  ...
  101      7  12     1     1   sign
   15     10  13     1     1   water (fits)
   64     10  13     1     1   water (fits)
   71     10  13     1     1   DROPPED (31st)
   75     10  13     1     1   DROPPED (32nd)
```

Coins (tiles): `(10–13, 5)`, `(66–69, 3)`, vertical stack at `x=74` rows 5–8, zigzag `82–89`. Food: one weapon at pixel `(448, 160)`. Enemies (tiles, then `* 32` in `createEnemy`): ten walkers from column 3 through 92.

## World 2 — snow / clouds

A broken snow floor (`id` 5 over `id` 4) with cloud stepping stones, a rising staircase at columns 67–70, and a long cloud runway into the sign at `(111, 1)`.

The 30-slot cap drops the background tuft, the victory sign, and all four trees. The **colliding** platforms still fit (the staircase is the 30th record). The finish-line check does not need the sign.

Food sits on the high cloud at roughly `(39 * 32 - 32/3, 3 * 32 + 32/5)`. Six enemies.

## World 3 — pipe corridor

Ten candidate gap heights `random(1, 7)` and x positions `10, 20, …, 100`. Only the first **seven** clusters are compiled in (indices 0–6). Each cluster is four records:

```
{ x, height - 2,      8, 1, 1 }                 // top mouth
{ x, 0,               7, 1, max(0, height-2) }  // top shaft
{ x, 4 + height,     10, 1, 1 }                 // bottom mouth
{ x, height + 6,      7, 1, max(0, 6-height) }  // bottom shaft
```

Then a cloud runway `{ 80, 6, 2, 25, 1 }` and a sign `{ 101, 4, 12, 1, 1 }`.

`random(a,b)` is `rand()%(b-a)+a`, so heights are `1..6`. There is no `srand` in the project; the sequence is whatever the CRT default seed produces for that process.

The copy loop is `while (i <= sizeof(m)/sizeof(m[0]))` — **inclusive**, so it writes one `Map` past the initialiser (and past `map[29]` if the array is exactly 30). That is undefined behaviour. The portable example uses a strict `<` and prints the intended 30 records.

Coins: seven tiles at `x = 5,15,…,65` with independent random rows `3..6`. Food: `{10, 10}` pixels. Seven enemies at fixed tiles.

Rules unique to this world (enforced in `Role`, not in `Scene`):

- `CMD_UP` always applies, even in air.
- Any colliding tile except clouds kills unless `isShoot` is set.

## Capacity cheat sheet

| Buffer | Capacity | World 1 authored | World 2 authored | World 3 authored |
| --- | --- | --- | --- | --- |
| `map[30]` | 30 | 32 (2 water dropped) | 36 (sign + props dropped) | 30 + off-by-one write |
| `coins[70]` | 70 | 19 + OOB copy | 11 + OOB copy | 7 + OOB copy |
| `food[5]` | 5 | 1 + OOB copy | 1 + OOB copy | 1 + OOB copy |
| `myEnemy[30]` | 30 | 10 + OOB copy | 6 + OOB copy | 7 + OOB copy |

`examples/src/map_layout.cpp` dumps the three authored tables, flags dropped rows, and checks finish-line constants. `examples/python/dump_world_maps.py` prints the same tables as ASCII.
