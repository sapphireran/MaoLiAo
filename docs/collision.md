# Collision

Every overlap test is the same four-corner point-in-rect check,
`Role::isHit`.

The moving sprite is shrunk by 1 px on each edge so a cat standing *on* a
tile does not immediately count as inside it:

```
r[0] = (-x0 + x + 1,           y + 1)
r[1] = (-x0 + x + WIDTH - 1,   y + 1)
r[2] = (-x0 + x + 1,           y + HEIGHT - 1)
r[3] = (-x0 + x + WIDTH - 1,   y + HEIGHT - 1)
```

A target AABB is stored as two points: top-left `m[0]` and bottom-right
`m[1]`. A corner hits when

```
m0.x ≤ rx ≤ m1.x  and  m0.y ≤ ry ≤ m1.y
```

Only **one** of the four corners has to land inside. This is not a full
SAT/AABB overlap: a large sprite can straddle a small tile and miss if
every corner stays outside. For 32×32 vs 32×32 (or vs pipes) it is close
enough.

Python: `examples.maoliao_lab.aabb`.

## `hitMap`

Walks `Scene::map` while `0 < id < 11` and `i < MAP_NUMBER`. Scenery
(11–14) is skipped, which is why the goal sign and trees are walk-through.

Target size:

- Default: `xAmount * 32` by `yAmount * 32`
- id 8 or 10 (pipe mouths): `xAmount * 64` by `yAmount * 64`

World 3 extra: if the tile id is not 2 (cloud) and `isShoot` is false, a hit
sets `myHero.died = true` **and still returns the tile**. Horizontal
resolution in `action` calls `hitMap(..., world=1)` on purpose so walking
into a pipe in world 3 does not trigger that death path; falling/standing
tests pass the real `world`.

## `hitCoins` / `hitFood` / `hitEnemy`

Same four corners. Coins are 32×32 at `tile * 32`. Food is
`(3*WIDTH/2 + 4)` × `(4*HEIGHT/5)` = 52×25.6, using the **pixel**
coordinates stored in `food[]` (worlds 1–2 already multiplied by 32; world
3 stored `(10, 10)`). Enemies are 32×32; slots with `turn == 0` are skipped.

A collected coin is zeroed and its cell is handed to `Scene::setScorePos`
for the sparkle animation. Food zeroing also sets `isShoot = true`.

## Stomp vs death

After gravity, if `vY > 0` (falling) an enemy hit is a stomp. Later in the
same function a second `hitEnemy` with `vY <= 0` (rising or grounded) is
death. Same-frame order therefore matters: a falling overlap is handled as
a stomp first, the enemy is cleared, and the death test misses it.

## Bullets

`bullteFlying` (typo kept in the source) reuses `hitEnemy` and
`hitMap(..., 1)`. `MAX_DISTANCE` (480) is compared to the bullet's **screen
x**, not world x, so shots from the left edge travel almost a full window
and shots already near the right die immediately.

## Empty-slot convention

`x == 0 && y == 0` means free for coins, food, bombs, and bullets. Enemies
use `turn == 0`. Do not author a pickup on the origin.
