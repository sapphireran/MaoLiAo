# 06 — Roles and combat

`Role` is the actor system: one hero, up to 30 enemies, 30 bullets, and 5 explosion sprites.

## Hero

```text
x, y           screen pixels (ints, derived from xx, yy)
xx, yy         screen pixels (doubles, integration accumulators)
x0             camera / world origin (≤ 0 while scrolling)
vX, vY         feel-space velocities
turn           +1 face right, -1 face left
direction      this-frame input vector (x used to set turn)
isFly          in air
isShoot        mushroom collected — J is live; also world-3 pipe immunity
died           touched enemy / pit / (world 3 solid)
ending         past isEnding threshold, auto-walk
passed         screen x > XSIZE
```

Constructor spawn is `(X0, X0)` = (64, 64), not `(X0, Y0)`. `Y0` (96) is unused. `isFly` starts true so the cat falls onto the first floor.

Walk cycle: `rolePos = -x0 + x`, then

```text
if rolePos/STEP % 2 == 0 and % 4 != 0 → frame 2
if rolePos/STEP % 4 == 0             → frame 1
```

`role.bmp` layout used by `show()`:

| Source x | Source y | Pose |
| --- | --- | --- |
| 0 / 32 | 0 (color) + 32 (mask) | walk frames, facing right |
| 96 / 128 | same | walk frames, facing left |
| 64 | same | death |

## Enemies

`createEnemy` writes world-pixel positions (`tile * WIDTH/HEIGHT`) and an initial `turn`.

World 1 (10): columns 3, 18, 25, 28, 33, 39, 68, 66, 81, 92.

World 2 (6): 18, 24, 28, 44, 97, 99.

World 3 (7): 24, 34, 37, 43, 63, 67, 86 — they still patrol even among lethal pipes.

Patrol runs only when `(int)(enemy_iframe * 100) % 2 == 0`. `enemy_iframe` advances by `TIME * 5` in `show`, so the gate is a crude subframe. Each step is `ENEMY_STEP` (1 px) in `turn`. Reverse on wall or missing floor (see [04-collision.md](04-collision.md)).

Art: `ani.bmp` rows 0 (color) / 1 (mask), 2 frames.

## Mushrooms and shooting

`isShoot` starts false. `hitFood` flips it and plays `music_getWeapon`.

While `CMD_SHOOT` and `isShoot`:

- First frame of a hold (`shootButtonDown == false`): fire immediately.
- Subsequent frames: fire when `shootTimeInterval` is 0, then accumulate `TIME` until it exceeds `TIME_INTERVAL_BULLET` (0.2 s) and wrap to 0.

Spawn x is `hero.x + WIDTH/2` facing right, or `hero.x - WIDTH` facing left. Y matches the cat.

`setBullet` takes the first slot with `(0,0)`.

## Bullets

Updated inside `show()` via `bullteFlying`:

```text
x += LEHGTH_INTERVAL_BULLET * turn    // 4 px per drawn frame
```

Death conditions and scoring are in [04-collision.md](04-collision.md). Wall / max-distance explosions play `music_boom`; enemy explosions play `music_boom2` and add 5 points.

`MAX_DISTANCE` (480) is compared to **screen** x. A bullet fired while the camera has scrolled still dies around the right side of the window, not 480 world pixels from the muzzle.

## Explosions

`setBomb` records a world-pixel point. `show` plays 4 frames (`TIME * 10`) at 64×64, offset by half a tile so the burst is centered. Rows 4 (color) / 6 (mask) of `ani.bmp`.

## Score

| Event | Points | Code |
| --- | --- | --- |
| Coin | +10 | `Role::action` after `hitCoins` |
| Stomp | +5 | falling `hitEnemy` |
| Bullet vs enemy | +5 | `bullteFlying` |

Score is a `Role` member. Reconstructing `Role` (death, clear, restart) zeros it. It is never written to `gameRecord.dat`.

## World 3 interactions

- Jump bit is accepted even when `isFly` is already true.
- `hitMap(..., world)` with the real world index on the vertical probe can set `died` on pipe contact.
- Horizontal `hitMap(..., 1)` will not. You can be pushed by a pipe side without dying, then die on the next downward probe.
- `isShoot` (mushroom) suppresses pipe death. The world-3 mushroom sits at `(10, 10)` px — almost off the usable path.

## MCI aliases opened by `Role`

`死亡1`, `跳`, `金币`, `踩敌人`, `吃到武器`, `子弹`, `子弹撞墙`, `子弹打到敌人`. All `open`ed in the constructor with `from 0` on each play so overlapping events restart the sting.
