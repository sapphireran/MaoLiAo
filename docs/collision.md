# Collision

Every query in `Role` uses the same idea: build four **inset** corners of a 32×32 sprite, then test whether any corner sits inside a target AABB (`Role::isHit`).

## Hero / sprite box

`xmap = (int)myHero.x0` converts screen X to world X (`worldX = -xmap + screenX`).

```
r[0] = (worldX + 1,           y + 1)
r[1] = (worldX + WIDTH - 1,   y + 1)
r[2] = (worldX + 1,           y + HEIGHT - 1)
r[3] = (worldX + WIDTH - 1,   y + HEIGHT - 1)
```

The 1 px inset avoids getting stuck on seams.

`isHit(p1, p2)` is **not** a full SAT test. It only asks whether each of `p1`’s four points is inside `p2`’s rectangle (`p2[0]` = min, `p2[1]` = max). A tall thin sprite that overlaps a tile without a corner inside that tile will miss. In practice tiles are large and the hero is 32×32, so it is close to AABB-vs-AABB.

## Tiles (`hitMap`)

Walk `Scene::getMap()` while `0 < id < 11` and `i < MAP_NUMBER`.
Ids 11–14 are scenery (grass tufts, flag, water, trees) and are **not** solid.

Tile world box:

```
min = (map.x * 32, map.y * 32)
if id is 8 or 10:   // pipe mouths, 2×2 art
    max = min + (xAmount * 64, yAmount * 64)
else
    max = min + (xAmount * 32, yAmount * 32)
```

World 3 special case: if the overlapping tile is **not** id 2 (cloud) **and** `isShoot == false`, set `died`. The star/weapon pickup sets `isShoot`, which is also the “don’t die on pipes” flag — the in-game text calls it 无敌金币 / 星星.

`hitMap` is reused for:

| Call site | World arg | Purpose |
| --- | --- | --- |
| jump / land (`y + 1`) | current `world` | ground + world-3 death |
| walk (`x, y`) | **1** | walls without world-3 death |
| enemy AI (ahead / underfoot) | **1** | turn around |
| bullets | **1** | explode on wall |

Forcing world `1` on those last three is why bullets and walkers do not “die” when they graze a pipe.

## Coins (`hitCoins`)

Each live coin is one 32×32 cell at `(coins[i].x * 32, coins[i].y * 32)`.
On hit: play coin SFX, `score += 10`, `Scene::setScorePos` (floating +10 anim), then zero the coin slot.

`createCoin` copies initializer lists with `while (i <= sizeof/sizeof)` — one extra iteration reads off the end of the local array (quirks).

## Food / weapon (`hitFood`)

Food AABBs are **not** grid-aligned the same way. `createFood` stores **pixel** coordinates. The hit box is:

```
min = (food.x, food.y)
max = min + (3*32/2 + 4, 4*32/5) = min + (52, 25)
```

On hit: weapon SFX, `isShoot = true`, hide the pickup. This is the only way to enable `J` and (in world 3) survive non-cloud tiles.

## Enemies (`hitEnemy`)

Live enemies have `turn != 0`. Box is 32×32 at `(enemy.x, enemy.y)` in **world** pixels (not grid). `createEnemy` multiplies the authored grid cells by 32.

Resolution order in `action`:

1. If falling (`vY > 0`) and overlapping an enemy → stomp: +5, bomb sprite, clear enemy.
2. Later, if overlapping an enemy **and** `vY <= 0` → hero dies.

So a jump that clips an enemy on the way **up** is lethal; on the way **down** it is a stomp. Side bumps while grounded (`vY == 0`) are lethal.

Enemy patrol: every other tick of `enemy_iframe * 100`, step `turn * ENEMY_STEP` (1 px). Reverse if the body overlaps a tile **or** the cell ahead-and-down is empty (edge of a platform).

## Bullets (`bullteFlying`)

Spawn at hero screen X (offset by facing) with `turn = hero.turn`. Each show-frame:

- despawn if screen X is outside `[-32, 512)`
- `hitEnemy` or `hitMap(..., world=1)` or `x > MAX_DISTANCE` (480) → explode
- else `x += LEHGTH_INTERVAL_BULLET * turn` (`LEHGTH` is the original spelling; value 4)

Enemy hit: +5, `music_boom2`, bomb at the enemy.
Wall / max range: `music_boom`, bomb snapped to the tile grid.

Fire rate: first frame of a press always shoots; while held, a shot every `TIME_INTERVAL_BULLET` (0.2 s). `isShoot` must already be true.

`MAX_DISTANCE` is compared against **screen** X, not world X, so you cannot snipe far off the right side even after the camera has scrolled.

## Example programs

- `examples/02_aabb_hit` — inset corners, pipe 2× scale, scenery ids ignored, stomp vs side
- `examples/06_side_scroller_sim` — collect a coin, stomp, side-hit death
