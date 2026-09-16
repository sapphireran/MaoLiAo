# 04 — Collision

Every hit test in `role.cpp` is the same idea: take the actor’s 32×32 box, shrink it by 1 px on each side, and ask whether **any of the four corners** lies inside the target’s axis-aligned rectangle.

```cpp
bool Role::isHit(POINT* p1, POINT* p2)
{
    for (int i = 0; i < 4; i++)
        if (p1[i].x >= p2[0].x && p1[i].y >= p2[0].y
         && p1[i].x <= p2[1].x && p1[i].y <= p2[1].y)
            return true;
    return false;
}
```

`p1` is four corners. `p2` is `{topLeft, bottomRight}`.

This is **not** a full AABB-vs-AABB test. A large actor can overlap a thin tile without any corner landing inside it. For a 32×32 cat versus tiles that are at least 32 px wide, it is good enough. World-3 pipes are 64×64 (`id` 8 and 10 use `2*WIDTH` / `2*HEIGHT`), so the same helper still works.

## Hero corners in world space

```text
xmap = (int)hero.x0
r[0] = (-xmap + x + 1,           y + 1)
r[1] = (-xmap + x + WIDTH - 1,   y + 1)
r[2] = (-xmap + x + 1,           y + HEIGHT - 1)
r[3] = (-xmap + x + WIDTH - 1,   y + HEIGHT - 1)
```

`x,y` are the **screen** coordinates of the thing being tested (hero, bullet, or an enemy’s screen projection). Adding `-x0` converts them to world pixels so they can be compared with tile and coin world boxes.

## Tiles — `hitMap`

Walks `Scene::map[]` while `id ∈ (0, 11)` and `i < MAP_NUMBER`. IDs 11–14 are scenery (grass tufts, victory sign, water, trees) and are **not solid**.

Tile box:

```text
m0 = (map.x * WIDTH, map.y * HEIGHT)
m1 = m0 + (xAmount * cellW, yAmount * cellH)
```

`cellW/cellH` are `WIDTH/HEIGHT` except for `id == 8` or `id == 10` (pipe mouths), which use `2*WIDTH` / `2*HEIGHT`.

Special case — world 3:

```cpp
if (world == 3 && map.id != 2 && !hero.isShoot)
    hero.died = true;
```

Clouds (`id == 2`) stay friendly. Any other solid kills unless the mushroom already set `isShoot`. Several `hitMap` call sites pass `world = 1` **on purpose** so side bumps, enemy patrol probes, and bullet-vs-wall tests do not trigger that death rule.

## Coins — `hitCoins`

Each live coin is one 32×32 world cell at `(coins[i].x * WIDTH, coins[i].y * HEIGHT)`. A hit returns the `POINT*` so `action` can:

1. play `music_coin`
2. `score += 10`
3. `setScorePos` (sparkle)
4. zero the coin

## Food — `hitFood`

Mushrooms are stored in **pixels**, not tiles (`createFood` already multiplies by `WIDTH`/`HEIGHT` in worlds 1–2). The box is slightly larger than a tile:

```text
size = (3*WIDTH/2 + 4,  4*HEIGHT/5) = (52, 25)
```

A hit sets `isShoot = true` and removes the mushroom. No score.

## Enemies — `hitEnemy`

Live enemies have `turn != 0`. Box is 32×32 at the enemy’s **world** pixel. Used three ways:

| Caller | Extra rule | Result |
| --- | --- | --- |
| Falling hero (`vY > 0`) | stomp | +5, bomb FX, enemy cleared |
| Standing / rising hero (`vY <= 0`) | touch | death |
| Bullet | any overlap | +5, bomb FX, enemy + bullet cleared |

## Bullets vs world

`bullteFlying` (sic) also calls `hitMap(bullet.x, bullet.y, scene, 1)` and dies on:

- enemy overlap
- solid tile
- `bullet.x > MAX_DISTANCE` (480)
- `bullet.x` outside `[-WIDTH, XSIZE]`

Explosions are 4-frame FX in `bombs[]`. They are cosmetic; they do not deal damage.

## Probe used by enemy AI

Each patrol tick tests two points:

```text
(x1, y1) = screen of enemy origin          → if solid, reverse
(x2, y2) = one tile ahead, one pixel down  → if air, reverse
```

That is “turn at a wall or at a ledge.”

## What is *not* tested

- Hero vs scenery IDs 11–14
- Bullet vs coin / food
- Enemy vs enemy
- Two bullets
- Crouch (`CMD_DOWN` is parsed and ignored)

The portable four-corner helper and a few fixtures live in `examples/02_collision`.
