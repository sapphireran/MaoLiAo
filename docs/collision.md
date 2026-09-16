# Collision

Every overlap test in 猫里奥 is the same idea: take the hero (or bullet, or enemy probe) as a 32×32 box, shrink it by 1 px on each side, and ask whether **any of the four corners** sits inside a target axis-aligned rectangle.

```cpp
// MaoLiAo/role.cpp
bool Role::isHit(POINT* p1, POINT* p2)
{
    for (int i = 0; i < 4; i++)
    {
        if (p1[i].x >= p2[0].x && p1[i].y >= p2[0].y &&
            p1[i].x <= p2[1].x && p1[i].y <= p2[1].y)
            return true;
    }
    return false;
}
```

`p1` is the four inset corners. `p2[0]` is the target's top-left, `p2[1]` the bottom-right. This is **not** a full AABB vs AABB test: a large box can sit on top of a small box without any of the small box's corners being inside the large box, and the function will miss. For a 32×32 actor against tiles that are at least 32 px wide it is usually enough.

The portable copy is `examples/maoliao_lib/collision.py`.

## Hero corners

`hitMap`, `hitCoins`, `hitFood`, and `hitEnemy` all build the same four points. `xmap` is `(int)myHero.x0`, the camera origin:

```
( -xmap + x + 1,          y + 1 )
( -xmap + x + WIDTH - 1,  y + 1 )
( -xmap + x + 1,          y + HEIGHT - 1 )
( -xmap + x + WIDTH - 1,  y + HEIGHT - 1 )
```

The 1 px inset avoids flickering when the sprite is flush with a tile edge. Coordinates are in **world space** for tiles / coins / enemies (`xmap` is subtracted from the screen-space sprite). Food is stored in pixels already and compared in the same space.

## Map tiles

`hitMap` walks `Scene::map` while `id` is in `1 … 10` and `i < MAP_NUMBER`. IDs 11–14 (grass tufts, the victory sign, water, trees) are scenery and are **not** solid.

Default tile box:

```
top-left  = (tile.x * 32, tile.y * 32)
size      = (tile.xAmount * 32, tile.yAmount * 32)
```

IDs **8** and **10** (the two pipe mouths) double both axes:

```
size = (tile.xAmount * 64, tile.yAmount * 64)
```

That matches `Scene::show`, which blits those IDs as 2×2 cells.

A hit returns `&map[i]`. World 3 then applies the Flappy rule **before** returning:

```
if (world == 3 && map[i].id != 2 && myHero.isShoot == false)
    myHero.died = true;
```

So in world 3:

- clouds (id 2) are safe platforms
- any other solid tile kills, unless the weapon / star pickup has set `isShoot`
- the death assignment happens even on a landing probe (`y + 1`) and on a bullet's `hitMap(..., 1)` it does **not** happen, because bullets pass `world = 1`

Horizontal blocking after a run step calls `hitMap(x, y, scene, 1)` on purpose, so walking into a world-3 pipe does not use the lethal branch of that particular call. Jump probes pass the real `world`, so vertical contact with a pipe still kills.

## Coins

Coins live in tile coordinates. Box is one cell:

```
(coin.x * 32, coin.y * 32) … + (32, 32)
```

A hit awards +10, plays `music_coin`, records a sparkle through `Scene::setScorePos`, and zeroes the coin. Because unused coins are `(0, 0)`, a coin authored on tile (0, 0) would be invisible and uncollectable. None of the shipped tables use that cell.

## Food (weapon / star)

Food boxes are pixel-based and slightly larger than a tile. The C++ uses
integer macros, so the size is `(52, 25)` not `(52, 25.6)`:

```
(food.x, food.y) … + (3 * WIDTH / 2 + 4, 4 * HEIGHT / 5)
                 … + (52, 25)
```

A hit sets `isShoot = true`, plays `music_getWeapon`, and zeroes the food. It does not add score. World 1 places the pickup on the cloud at tile (14, 5) in pixels `(448, 160)`. World 2 nudges it off the floating cloud. World 3 drops a 10×10 px pickup near the origin — easy to miss in the pipe field.

## Enemies

An enemy is live while `turn != 0`. Box is 32×32 at the enemy's **world** pixel position (`emy.x`, `emy.y` — already multiplied by 32 when spawned).

Hero contact:

- `vY > 0` (falling) → stomp: +5, bomb, enemy cleared
- `vY <= 0` (grounded or rising) → `died = true`

Bullet contact uses the same `hitEnemy` with the bullet's screen `x` and the same camera correction, then +5 and a bomb.

## Bullets

`bullteFlying` (the typo is in the source) steps `x += 4 * turn` (`LEHGTH_INTERVAL_BULLET`). It dies when:

- it leaves the framebuffer (`x > XSIZE` or `x < -WIDTH`)
- `hitEnemy` succeeds
- `hitMap(..., world=1)` succeeds
- `x > MAX_DISTANCE` (480)

`MAX_DISTANCE` is compared to the **screen** x, so a shot fired while the camera has scrolled still expires around pixel 480 of the window, not 480 world pixels from the muzzle. The comment in `role.cpp` already notes that the right edge of the map will also detonate a shot.

On death, wall hits snap the bomb x to the tile grid using `x0` and the shot's facing; enemy hits use the enemy's pixel position.

## Enemy patrol probes

Each live enemy steps `ENEMY_STEP` (1 px) in `turn` when `(int)(enemy_iframe * 100) % 2 == 0`. It then probes:

1. `hitMap` at its current screen position — wall
2. `hitMap` one body width ahead and 1 px down — floor

A wall hit **or** a missing floor flips `turn`. The floor probe is why enemies reverse at ledges instead of walking into the void.

## What this test does not do

- No swept AABB. Fast motion can skip a thin tile. `TIME` is 10 ms and `V_MAX` is modest, so it rarely shows up except on world 3's random pipes.
- No separate “head bump” vs “feet” resolution beyond the `vY > 0` landing check. A rising hero who overlaps a tile has `vY` zeroed and `y` snapped, which can glue the cat under a ceiling.
- Decorative IDs are not tested, so the victory sign and water are walk-through. Water is not a hazard; falling off the bottom of the screen is.
