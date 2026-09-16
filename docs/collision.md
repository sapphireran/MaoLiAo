# Collision

Every gameplay overlap uses the same test: take four inset corners of a 32×32 sprite and ask whether any corner lies inside an axis-aligned box.

## `isHit`

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

`p1` is four vertices. `p2[0]` is the box min, `p2[1]` the box max. This is **vertex-in-AABB**, not a full AABB-vs-AABB test. A large box can sit between the hero’s corners and be missed. For 32×32 vs 32×32 (or larger tile runs) it is usually enough.

The hero corners are inset by 1 px and shifted from camera space into world space:

```
r0 = (-x0 + x + 1,     y + 1)
r1 = (-x0 + x + 31,    y + 1)
r2 = (-x0 + x + 1,     y + 31)
r3 = (-x0 + x + 31,    y + 31)
```

`x0` is `Hero.x0` (camera). Enemies and bullets reuse the same helper, so a bullet’s “corners” are treated as if they were a 32×32 hero-sized box.

## Tiles — `hitMap`

Walk `Scene::map` while `0 < id < 11` and `i < MAP_NUMBER`. Decorative ids 11–14 (grass tufts, goal sign, water, trees) are **not** solid.

Box size:

| id | Pixel size |
| --- | --- |
| 8, 10 (pipe mouths) | `xAmount*64` × `yAmount*64` (double tile) |
| other solid ids | `xAmount*32` × `yAmount*32` |

World 3 special case: if the overlapping tile is not id 2 (cloud) **and** `isShoot == false`, `myHero.died = true`. The function still returns the tile pointer so physics can snap. Clouds remain safe platforms. After eating food, `isShoot` is true and pipe contact no longer kills (the comment calls this 无敌).

`hitMap(..., world)` uses the `world` argument only for that kill test. Several callers pass `1` on purpose (horizontal wall slide, enemy probes, bullets) so world-3 death is not triggered from those paths.

## Coins — `hitCoins`

Each coin is one tile at `(coins[i].x * 32, coins[i].y * 32)`. On hit: +10 score, `setScorePos` for the flash, zero the coin slot. `(0,0)` is also a valid tile, so a coin authored at the origin would be indistinguishable from an empty slot.

## Food — `hitFood`

Food boxes are **not** 32×32:

```
min = (food[i].x, food[i].y)          // already pixels in world 1/2
max = min + (3*WIDTH/2 + 4, 4*HEIGHT/5)  // 52 × 25.6 → 52 × 25
```

Pickup sets `isShoot = true` and plays `music_getWeapon`. Score flash is recorded; score integer does not increase.

## Enemies — `hitEnemy`

Live enemies are 32×32 at `(emy.x, emy.y)` in world pixels. Two uses:

1. **Stomp** — airborne, `vY > 0`, overlap → +5, spawn bomb, clear enemy.
2. **Side/head hit** — `vY <= 0` → hero dies.

Bullets use the same function. A hit awards +5 and clears the enemy.

## Resolution, not physics engines

There is no MTV, no swept AABB, no separate axis. Resolution is:

- **Floor:** snap `y` to tile multiples when a `y+1` probe hits and `vY > 0`.
- **Wall:** undo the last horizontal `H`, or clamp to `XRIGHT`.
- **Pit:** `y > 384` → die.
- **Kill box:** set a bool.

Because the hero can tunnel if `H` is large, the 2.3 px/frame cap keeps overlaps honest.

## Portable helper

`examples/include/maoliao/aabb.h` implements `vertex_in_aabb` with the same inset-corner convention and a full AABB overlap for comparison. `examples/demos/aabb_demo` prints cases that the vertex test hits or misses.
