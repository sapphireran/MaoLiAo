# Collision

Every overlap test in 猫里奥 is the same idea: take the hero (or bullet)
rectangle, shrink it by one pixel on each side, and ask whether any of the
four remaining vertices sits inside a target AABB.

## Vertex inset

```
r[0] = (-xmap + x + 1,           y + 1)
r[1] = (-xmap + x + WIDTH - 1,   y + 1)
r[2] = (-xmap + x + 1,           y + HEIGHT - 1)
r[3] = (-xmap + x + WIDTH - 1,   y + HEIGHT - 1)
```

`xmap` is `(int)Hero::x0`, the camera origin. Subtracting it converts a
screen-space sprite into world space so it can be compared with tiles that
were authored in world pixels (`tileX * 32`).

The 1 px inset is why the cat can graze a wall without sticking as often as a
tight 32×32 box would. It also means a 1 px-deep overlap on a single corner
is enough to count as a hit.

## `isHit`

```
for each vertex p of the hero:
    if p.x ∈ [box.left, box.right] and p.y ∈ [box.top, box.bottom]:
        return true
return false
```

This is **not** a full AABB vs AABB test. A large hero that completely
contains a small box without putting a vertex inside it would miss. In
practice every target is at least 32×32 and the hero is 32×32, so a real
overlap almost always drives a vertex inside. Food is the awkward case:
the flower hit box is `3*WIDTH/2+4` by `4*HEIGHT/5` (52×25.6 → 52×25),
still wider than the inset hero.

## Map tiles (`hitMap`)

Only records with `0 < id < 11` collide. IDs 11–14 (grass tuft, flag, water,
tree) are scenery.

The tile AABB is:

```
left   = map.x * WIDTH
top    = map.y * HEIGHT
right  = left + map.xAmount * cellW
bottom = top  + map.yAmount * cellH
```

`cellW` / `cellH` are 32×32 except for pipe mouths **id 8 and id 10**, which
use 64×64 because `Scene::show` blits those from a 2×2 tile in `map.bmp`.

World 3 special case: if the colliding tile is not a cloud (id 2) and the
hero has not eaten the flower (`isShoot == false`), `hitMap` sets
`Hero::died = true`. Clouds stay safe landing pads; pipes kill on touch
unless you are “invincible” via the flower. The comment in `role.cpp` calls
the flower a star — the sprite is the food / weapon pickup.

Callers pass different `world` values on purpose:

| Call site | `world` argument | Why |
| --- | --- | --- |
| Vertical probe `hitMap(x, y+1, …, world)` | current world | world 3 death-on-touch |
| Horizontal wall `hitMap(x, y, …, 1)` | forced 1 | sliding a pipe should not kill |
| Enemy foot / nose probes | forced 1 | patrols should not die |
| Bullet `hitMap` | forced 1 | shots explode, they do not use the death flag |

## Coins

Coin boxes are exactly one tile:

```
[coins[i].x * 32, coins[i].y * 32] .. + (32, 32)
```

A hit awards 10 points, records the tile in `Scene::score[]` so the next few
frames can play the “+” sparkle, then zeroes the coin slot.

## Food / flower

Food is stored in **pixels**, not tiles (world 1: `{14*32, 5*32}`). The box
is the sprite size `52 × 25`. A hit sets `Hero::isShoot = true` and clears
the slot. There is no extra life or size change — the comment in
`createFood` says that was planned and never shipped.

## Enemies

Active enemies are those with `turn != 0`. Box is 32×32 at the stored pixel
position. Stomp versus death is not decided in `hitEnemy`; the caller looks
at `vY`:

- `vY > 0` during the airborne update: stomp, +5, bomb sprite.
- `vY <= 0` at the end of `action`: death.

A jump that lands on an enemy in the same tick can therefore stomp and then
also see `vY == 0` after the landing snap. The stomp branch runs first and
clears the enemy, so the later `hitEnemy` misses.

## Bullets

`bullteFlying` (the typo is in the source) reuses `hitEnemy` and `hitMap`.
Priority:

1. Off-screen → discard, no bomb.
2. Enemy or tile or `x > MAX_DISTANCE` (480) → bomb sprite, discard.
3. Else `x += 4 * turn`.

Hitting an enemy is +5 and `music_boom2`. Hitting a tile or max range is
`music_boom` and no score.

## What this is not

- No swept volumes. Fast objects can tunnel. Bullets move 4 px/tick and tiles
  are 32 px, so they do not tunnel. A 100 Hz jump is also well under a tile
  per tick (~4 px at launch).
- No separate floor / wall / ceiling normals. Landing vs head-bump is
  inferred from `vY` after the fact.
- No one-way platforms. Clouds (id 2) are solid from every side.

The portable reconstruction is `examples/include/maoliao/collision.hpp` and
the demo `examples/src/demo_collision.cpp`.
