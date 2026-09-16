# Levels

Three worlds. Each one is a set of brace-initialized arrays, not a map file.
Coordinates below are **tiles** unless a sentence says pixels. The screen is
16 tiles wide and 12 tiles tall (512/32 × 384/32).

## Shared rules

- Hero spawn is `(X0, X0)` = (64, 64) pixels, i.e. tile (2, 2). `Y0` is
  defined as `3*HEIGHT` but the constructor assigns `myHero.y = X0`.
- Ending auto-walk starts when `-(int)x0 + x` exceeds the per-world distance
  in `Scene::isEnding`.
- Flag tiles (id 12) are scenery. Crossing the distance threshold, not
  touching the flag, is what sets `Hero::ending`.
- `passed` becomes true when the auto-walking sprite’s screen `x` exceeds
  `XSIZE` (512).

| World | Ending distance | Pixels | Flag record |
| --- | --- | --- | --- |
| 1 | 94 tiles | 3008 | `{101, 7, 12, 1, 1}` |
| 2 | 104 tiles | 3328 | `{111, 1, 12, 1, 1}` |
| 3 | 94 tiles | 3008 | `{101, 4, 12, 1, 1}` |

## World 1 — grass course

Ground is a broken grass strip with dirt fill, a long snow-capped mid section,
a sticky pipe-top (id 6) at tiles 67–70, a one-tile snow island at 74, then
grass again to the flag.

### Platforms (id 1 / 5 / 6 caps, id 3 / 4 fill)

| x | y | id | w | h | Notes |
| --- | --- | --- | --- | --- | --- |
| 0 | 9 | 1 | 15 | 1 | Opening grass |
| 18 | 8 | 1 | 9 | 1 | Raised island |
| 27 | 9 | 5 | 37 | 1 | Long snow cap |
| 67 | 9 | 6 | 4 | 1 | Sticky pipe top |
| 74 | 9 | 5 | 1 | 1 | Single snow tile |
| 78 | 9 | 1 | 28 | 1 | Run-out to the flag |
| 0 | 10 | 3 | 15 | 2 | Dirt under the opener |
| 18 | 9 | 3 | 9 | 3 | Dirt under the island |
| 27 | 10 | 4 | 37 | 2 | Snow fill |
| 67 | 10 | 3 | 4 | 2 | Dirt under the pipe |
| 74 | 10 | 4 | 1 | 2 | Snow fill under the island |
| 78 | 10 | 3 | 28 | 2 | Dirt under the run-out |

Gaps at tile columns 15–17, 71–73, and 75–77 are pits (id 13 water sits in
three of those at row 10, decoration only).

### Clouds (id 2)

`(10,6)×4`, `(40,4)×3`, `(62,6)×1`, `(65,4)×5`.

### Pipes / tufts / water / flag

Up-pipes (id 10) at `(36,7)`, `(45,7)`, `(80,7)`, `(90,7)`.
Grass tufts (id 11) at 19, 23, 32, 49, 53, 57, 93 along rows 6–7.
Water (id 13) at 15, 64, 71, 75 on row 10.
Flag at `(101, 7)`.

### Coins

```
(10–13, 5)          on the first cloud
(66–69, 3)          above the pipe / cloud
(74, 5–8)           vertical stack on the snow island
(82,7) (83,8) … (89,8)  zigzag on the run-out
```

20 coins × 10 points = 200 if you collect them all.

### Flower

One pickup at **pixel** `(14*32, 5*32)` = (448, 160), above the end of the
opening grass.

### Enemies `{tileX, tileY, turn}`

```
(3,8,+1) (18,7,-1) (25,7,+1) (28,8,-1) (33,8,-1)
(39,8,+1) (68,3,+1) (66,8,-1) (81,6,+1) (92,6,+1)
```

Ten patrols. The `(68,3)` one sits on the high cloud near the pipe.

## World 2 — snow and stairs

The floor is a snow cap (id 5) over snow fill (id 4) with missing columns
that become pits. Clouds (id 2) are the real platforms.

### Ground strips

Caps at y = 10: `[0,15)`, `[38,48)`, `{50}`, `{53}`, `{56}`, `[61,71)`,
`{75}`, `[82,93)`. Matching fill at y = 11, height 2.

### Clouds

```
(18,9)×4   (23,6)×6   (25,3)×3   (31,10)×2
(39,7)×1   (39,4)×1   (96,7)×2   (99,5)×2   (102,3)×12
```

The last cloud is the flag runway. A 4-step staircase climbs from (67,9) to
(70,6), then a matching 4-tall column at (75,6).

### Scenery

One tuft at `(42,8)`, flag at `(111,1)`, trees (id 14) at 4, 9, 83, 89 on
row 8.

### Coins

```
(25–27, 2)     top cloud
(32, 4–8)      vertical stack
(50,6) (53,6) (56,6)   over the isolated snow tiles
```

11 coins × 10 = 110.

### Flower

Pixel `(39*32 - 32/3, 3*32 + 32/5)` ≈ (1237, 102), near the high single
cloud at (39, 4).

### Enemies

```
(18,8,+1) (24,5,+1) (28,5,-1) (44,9,+1) (97,6,+1) (99,4,+1)
```

## World 3 — pipe gauntlet

This world is generated at construction time. Ten candidate columns:

```
x[i]      = i * 10 + 10          // 10, 20, …, 100
height[i] = random(1, 7)         // 1..6 inclusive (rand()%(7-1)+1)
```

Only the first **seven** columns are emitted (the last three are commented
out). Each column is four records:

```
top mouth     { x, height-2,     id 8,  1×1 }
top shaft     { x, 0,            id 7,  1 × max(0, height-2) }
bottom mouth  { x, 4+height,     id 10, 1×1 }
bottom shaft  { x, height+6,     id 7,  1 × max(0, 6-height) }
```

The playable gap is four tiles tall, starting at row `height` and ending
before row `4+height`. Because id 8/10 collide as 64×64, the effective
opening is tighter than the authored four-tile hole.

After the pipes:

```
{80, 6, 2, 25, 1}    long cloud runway
{101, 4, 12, 1, 1}   flag
```

Coins use a second random height table `random(3, 7)` at x = 5, 15, 25, …,
65 (seven coins). Food is the odd record `{10, 10}` — those are stored as
pixels, so the flower sits at (10, 10), the top-left of the screen.

Enemies are hand-placed, not random:

```
(24,4,-1) (34,6,+1) (37,3,-1) (43,5,-1)
(63,7,+1) (67,8,-1) (86,5,+1)
```

World 3 movement: `CMD_UP` always relaunches, and any solid that is not a
cloud kills you unless `isShoot` is already true. The intended line is
therefore “flap through the gaps, land on the cloud, walk to the flag.”

`random` is `#define random(a,b) (rand()%(b-a)+a)` and `createMap` never
calls `srand`, so a process that does not seed the RNG will produce the
same pipe field every launch (typically `rand()`’s default seed of 1).

## Capacity leftover

World 2’s map array is the fullest. `MAP_NUMBER` is 30; world 2 writes 29
records before the `id > 0` stop. Adding a 30th solid without raising the
constant silently drops it. World 3’s loop is `i <= sizeof(m)/sizeof(m[0])`,
which walks one past the array — see [known-issues.md](known-issues.md).

## Portable rebuild

`examples/include/maoliao/levels.hpp` stores the ending distances, the
world 1/2 coin counts, and the world 3 pipe formula. `demo_pipes.cpp`
prints a seeded gauntlet and checks that every gap is four tiles and that
mouths never invert (`bottom.y - top.y` stays positive).
