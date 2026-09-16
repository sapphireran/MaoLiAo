# Collision and combat

Every overlap test in `Role` is the same four-vertex query (`isHit`). There is no swept AABB and no separate “feet vs head” detector except the extra `y + 1` probe used to decide “am I on a floor?”.

## Four-corner test

For a sprite at screen `(x, y)` the homework builds four points **inset by 1 px**, then subtracts the camera origin `xmap = (int)hero.x0` so the test runs in **world** pixels:

```
r[0] = (-xmap + x + 1,           y + 1)
r[1] = (-xmap + x + WIDTH - 1,   y + 1)
r[2] = (-xmap + x + 1,           y + HEIGHT - 1)
r[3] = (-xmap + x + WIDTH - 1,   y + HEIGHT - 1)
```

A target is stored as two corners `(m0, m1)`. A hit is “any hero corner inside that rectangle” (inclusive on all four edges). The target’s own corners are **not** tested against the hero box, so a large hero overlapping a tiny coin still works, but a large tile that only covers the hero’s interior (no corner inside) would miss. Tiles are big; coins are 32×32; this rarely matters.

`examples/src/aabb_lab.cpp` walks the same predicate.

## Tiles (`hitMap`)

`Scene::getMap()` is scanned while `id ∈ [1, 10]` and `i < MAP_NUMBER`. **Ids 11–14 are scenery** (grass tuft, flag, water, tree): they draw, they do not collide.

Ordinary tile box:

```
m0 = (tile.x * 32, tile.y * 32)
m1 = m0 + (tile.xAmount * 32, tile.yAmount * 32)
```

Pipe mouths (`id == 8` or `id == 10`) use **2×** both extents:

```
m1 = m0 + (xAmount * 64, yAmount * 64)
```

Id 7 (pipe shaft) stays 32×32 per cell; `show` blits it 64 px wide anyway, so the visible pipe is wider than its hit box. Documented as a quirk.

### World-3 instadeath

If `world == 3` and the tile is **not** id 2 (cloud) and `hero.isShoot == false`, a map hit sets `died = true`. Eating the weapon / star (`hitFood`) sets `isShoot`, which is also the “invincible against terrain” flag. Clouds remain safe either way.

`hitMap` is also called with a hardcoded `world = 1` from bullets and from the horizontal wall snap, so those paths **never** apply the instadeath rule.

## Coins (`hitCoins`)

Each live coin is one 32×32 world cell (`coins[i].x/y` are tile indices). A hit returns the `POINT*`; `action` then:

1. plays `music_coin`
2. `score += 10`
3. `scene.setScorePos` (floating “+” sprite for a few frames)
4. zeroes the coin (`x = y = 0`; `show` skips `x == 0`)

Coin `(0, *)` would be invisible and unhittable. None of the authored coins sit on column 0.

## Food / weapon (`hitFood`)

Food stores **pixel** coordinates, not tiles, and uses a short fat box:

```
m1 = m0 + (3 * WIDTH / 2 + 4,  4 * HEIGHT / 5)   # 52 × 25
```

A hit plays `music_getWeapon`, records a score-pop (no point award), clears the pickup, and sets `isShoot = true`. After that, `J` can spawn bullets.

| World | Pixel spawn (from `createFood`) |
|---|---|
| 1 | `(14 * 32, 5 * 32) = (448, 160)` |
| 2 | `(39 * 32 - 32/3, 3 * 32 + 32/5) → (1238, 102)` (integer division) |
| 3 | `(10, 10)` — almost the top-left corner |

## Enemies (`hitEnemy`)

Live enemies have `turn != 0`. Box is 32×32 at the enemy’s **world** pixel `(x, y)` (already `tile * 32` from `createEnemy`).

Resolution in `action`:

| Situation | Result |
|---|---|
| `vY > 0` (falling) and overlap | stomp: `score += 5`, bomb sprite, enemy zeroed, hero lives |
| overlap and `vY <= 0` | hero dies, `music_died` |

Enemies patrol by `ENEMY_STEP = 1` px when `(int)(enemy_iframe * 100) % 2 == 0`. They flip `turn` if the next body cell hits a tile **or** the pixel under their front foot has no tile (`hitMap(..., y + 1) == NULL`).

## Bullets

Pool of 30. `setBullet` copies the hero facing. Each `show` frame, `bullteFlying`:

1. Despawn if screen `x` is outside `[-32, 512)`.
2. `hitEnemy` or `hitMap(..., world=1)` or `x > MAX_DISTANCE` (480) → explode.
3. Enemy hit: `score += 5`, `music_boom2`, bomb at the enemy, both slots cleared.
4. Wall / range: `music_boom`, bomb snapped to a tile column, bullet cleared.
5. Else `x += LEHGTH_INTERVAL_BULLET * turn` (`4` px / frame).

Hold-`J` spacing: first shot immediate, then every `TIME_INTERVAL_BULLET = 0.2` s (`shootTimeInterval` accumulates `TIME`).

`MAX_DISTANCE` is a **screen** x test, so you cannot snipe far to the right even if the map continues.

## Bombs

Up to 5 simultaneous. `bomb_iframe += TIME * 10`; at frame 5 the slot is cleared. Drawn 64×64 from `ani.bmp` rows 4–7.

## Score summary

| Event | Points |
|---|---|
| Coin | +10 |
| Stomp | +5 |
| Bullet kills enemy | +5 |
| Food | 0 (enables shoot / world-3 immunity) |

HUD is `Control::showScore` at `(10, 10)` and `showLevel` at `(XSIZE - 90, 10)`.
