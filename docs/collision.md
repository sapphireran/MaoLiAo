# Collision

Every hit-test in `role.cpp` uses the same four-corner rule.

## `isHit`

`p1` is four points (hero or projectile inset by 1 px). `p2` is a rectangle given as **top-left** `p2[0]` and **bottom-right** `p2[1]`.

A hit is true if **any** of the four points satisfies:

```
p2[0].x ≤ px ≤ p2[1].x  and  p2[0].y ≤ py ≤ p2[1].y
```

This is a point-in-AABB test, not a full AABB-vs-AABB test. A body that overlaps a tile with its centre but whose four inset corners miss the rectangle will not register. The 1 px inset (`x+1`, `y+1`, `x+WIDTH-1`, `y+HEIGHT-1`) shrinks the 32×32 sprite to 30×30 sample points.

World coordinates for the hero corners subtract `Hero.x0` (the camera origin) so they can be compared to map tiles stored in world space.

```
r[0] = (-x0 + x + 1,     y + 1)
r[1] = (-x0 + x + WIDTH-1, y + 1)
r[2] = (-x0 + x + 1,     y + HEIGHT-1)
r[3] = (-x0 + x + WIDTH-1, y + HEIGHT-1)
```

## `hitMap`

Walks `Scene::map[]` while `0 < id < 11` and `i < MAP_NUMBER`. Ids **11–14** (grass tufts, goal sign, water, trees) are decorative: they draw but never collide.

Tile AABB:

- Default: `[x*32, y*32]` → `[(x+xAmount)*32, (y+yAmount)*32]`
- Ids **8 and 10** (pipe mouths): each “amount” unit is **2×** tile size (`2*WIDTH`, `2*HEIGHT`)

Special case: if `world == 3` **and** the tile id is not 2 **and** `!isShoot`, a map hit sets `Hero.died = true`. Clouds (id 2) remain safe landing pads, including the long cloud runway at the end of world 3. Eating the weapon mushroom (`isShoot = true`) disables this instant-death rule.

Several call sites pass `world=1` on purpose so that side-nudge / enemy-patrol / bullet hits do not apply the world-3 death rule:

| Caller | `world` argument | Why |
| --- | --- | --- |
| Jump landing `hitMap(x, y+1, …, world)` | current `world` | World 3 pipes kill on touch |
| Grounded check `hitMap(x, y+1, …, world)` | current `world` | same |
| After horizontal integrate | **1** | Wall stop without dying from a side brush in world 3? (also used in worlds 1–2) |
| Enemy patrol | **1** | Enemies should turn, not kill the hero |
| `bullteFlying` | **1** | Bullets explode on walls |

## Coins

`hitCoins` uses a 32×32 AABB at `(coin.x * WIDTH, coin.y * HEIGHT)`. A collected coin is zeroed (`x=y=0`) and its old tile is passed to `Scene::setScorePos` for a 4-frame sparkle. Score **+10**.

The create loops use `i <= sizeof(array)/sizeof(array[0])`, which reads one past the static array. On typical stack layouts that extra slot is leftover memory; the tools treat only the authored coordinates as canonical. See [known-quirks.md](known-quirks.md).

## Food (weapon mushroom)

`hitFood` uses a **pixel** rectangle of size `(3*WIDTH/2 + 4) × (4*HEIGHT/5)` = `52 × 25.6` (the `int` y extent is 25). World 1 places food at `{14*32, 5*32}`. World 2 at `{39*32 - WIDTH/3, 3*32 + HEIGHT/5}`. World 3 at `{10, 10}` — that is near the top-left of the **world**, not a tile index.

Pickup sets `isShoot = true` and plays `music_getWeapon`. There is no extra life or size-up in this version (the comment in `createFood` says that was future work).

## Enemies

`hitEnemy` skips slots with `turn == 0` (dead or unused). AABB is 32×32 at the enemy’s world pixel origin.

Resolution in `Role::action`:

- Falling onto an enemy (`isFly && vY > 0`): stomp, +5, bomb, clear the slot
- Overlap with `vY <= 0` (standing or rising): `died = true`

Bullets use the same `hitEnemy`. A hit awards +5, clears the enemy, and spawns a bomb.

## Bullets

`bullteFlying` (name is spelled that way in the source):

1. Off-screen (`x > XSIZE` or `x < -WIDTH`): despawn
2. Else if enemy or map or `x > MAX_DISTANCE` (480): explode
3. Else `x += LEHGTH_INTERVAL_BULLET * turn` with `LEHGTH_INTERVAL_BULLET = 4`

Fire rate: first tap fires immediately; while the key is held, a shot is emitted when `shootTimeInterval` wraps past `TIME_INTERVAL_BULLET` (0.2 s). `isShoot` must already be true.

Bombs are purely visual: up to `BOMB_NUMBER` (5) sparkles on `ani.bmp` rows 4–6.

## Authoring rule of thumb

If you add a tile that should **block**, use `id` in `1..10`. If it should only **decorate**, use `11..14`. If it should kill on touch in world 3, it must be a colliding id other than 2, and the hero must not have the mushroom.
