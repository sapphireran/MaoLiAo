# Physics and combat

All motion is Euler-integrated with `Inertia::move` on a 0.01 s tick. The numbers below are the `define.h` macros; `fixtures/physics.json` is the machine-readable twin.

## Horizontal

Each tick:

1. `a = 0`. Left subtracts `A_ROLE` (20). Right — or the ending auto-walk — adds `A_ROLE`.
2. If velocity and `a` have opposite signs (braking) and the hero is standing on a tile, add friction acceleration `a1 = k * G * map.u`.
3. `k` is a facing fudge: `+|v|/v - 3` when moving right, `+|v|/v + 3` when moving left. For any non-zero `vX` that is `−2` or `+4`. It is not a physical term; it just makes left-vs-right stopping feel different.
4. `H = Inertia::move(vX, TIME, a + a1) * UNREAL_HEIGHT / REAL_HEIGHT`.
5. If `vX` flipped sign this tick, snap `vX` to 0.
6. If `|vX|` exceeds `V_MAX` (8), clamp.

`map.u` is filled when the level is built:

```text
u = (V_MAX / Tn) / G
```

| Surface | Worlds 1–2 `Tn` | World 3 `Tn` | `u` |
| --- | --- | --- | --- |
| IDs 1, 3, 4, 5 (ground) | T2 = 1.2 | T1 = 0.5 | 0.222… / 0.533… |
| ID 2 (cloud) | T2 = 1.2 | T2 = 1.2 | 0.222… |
| ID 6 (pipe body) | T1 = 0.5 | T1 = 0.5 | 0.533… |
| Anything else (pipes mouths, default) | T3 = 1.5 | T3 = 1.5 | 0.177… |

Airborne (`map == NULL`) sets `a1 = 0`, so there is no air friction.

Screen clamp: `x < 0` stops you. `x > XRIGHT` (192) while not ending scrolls the world via `x0` instead of letting the sprite leave the lock window. Colliding with a tile after the horizontal step rewinds `x` by `H` and zeroes `vX`, except when `x > XRIGHT` already — then `x` is forced to `XRIGHT`.

Pixel speed at the cap, after the jump scale:

```text
8 * 101 / 3.5 ≈ 230.9 “meter-units”/s
* TIME 0.01 ≈ 2.31 px/tick
```

That is why a 32 px tile takes more than one frame to cross even at `V_MAX`.

## Jump

Grounded jump (worlds 1–2) or any-tick jump (world 3):

```text
vY = -sqrt(2 * G * REAL_HEIGHT) = -sqrt(210) ≈ -14.491
isFly = true
```

Each airborne tick:

```text
yy -= -Inertia::move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT
y = (int)yy
```

`Inertia::move` applies `+G` to `vY`, so the hero decelerates upward and then falls. After the move, `hitMap(x, y+1)` is tested. Any hit zeroes `vY` and snaps `y` to the tile grid (`(y + HEIGHT/2) / HEIGHT * HEIGHT`). If `vY` was positive (falling) the hero is grounded (`isFly = false`). If `vY` was negative the jump is cancelled — a low ceiling eats the jump.

Leaving a platform with no tile under `y+1` sets `isFly` again (walk-off).

Peak theoretical height in “meters” is `REAL_HEIGHT` (3.5). In pixels that is `UNREAL_HEIGHT` (101), a little over three tiles. World 3’s pipe gap is four tiles (`(4+h) - (h-2) = 6` between mouth origins, minus the 2-tile-tall mouths), which is why the infinite-jump rule exists.

## Death and stomps

| Event | Condition | Result |
| --- | --- | --- |
| Fall out | `y > YSIZE` while falling | `died`, play `music_died` |
| Side-hit enemy | `hitEnemy` and `vY <= 0` | `died` |
| Stomp | `hitEnemy` while falling (`vY > 0`) | enemy cleared, +5, bomb sprite |
| World 3 tile | `hitMap` with `world==3`, tile id ≠ 2, `isShoot==false` | `died` (even standing on pipes) |
| Flower | `hitFood` | `isShoot = true`; world 3 solids become safe |

`hitMap` is also called with a hard-coded `world=1` in a few places (horizontal bump, enemy walk, bullets). Those calls never take the world-3 instant-death branch. Only the vertical / standing probes in `Role::action` pass the real `world`.

## Enemies

`createEnemy` stores tile coordinates × 32. Each even `enemy_iframe*100` tick they step `ENEMY_STEP` (1 px) in `turn` (±1). They reverse if the body overlaps terrain **or** the tile one body-width ahead at `y+1` is empty (edge detect). `turn == 0` means “dead / unused.”

They are not simulated when off-screen; they still exist in world space and will walk into the camera.

## Flower and bullets

World 1 flower is at pixel `(14*32, 5*32) = (448, 160)` — on the first cloud. World 2 is `(39*32 - 32/3, 3*32 + 32/5)` which, with integer division, is `(1237, 102)` on the stacked clouds at x=39. World 3’s `{10, 10}` is a 10×10 px blob in the top-left of *world* space, so it almost never overlaps the 32×32 hero unless you stand at the origin.

`J` is ignored until `isShoot`. The first press fires immediately; holding fires again every `TIME_INTERVAL_BULLET` (0.2 s). Spawn x is `hero.x + WIDTH/2` facing right, or `hero.x - WIDTH` facing left. Each tick the bullet moves `LEHGTH_INTERVAL_BULLET * turn` (4 px). It explodes when:

- it hits an enemy (+5, enemy cleared),
- it hits a tile with `id` in `(0, 11)`,
- `x > MAX_DISTANCE` (480),
- or it leaves `[-32, 512]`.

Up to 30 bullets and 5 simultaneous explosion sprites. Explosions last 5 animation frames at `TIME*10` per frame (~0.5 s).

## Ending auto-walk

`Scene::isEnding(-(int)x0 + x)` uses the *world* x of the sprite. After the threshold, `ending` is sticky: jump is blocked in worlds 1–2, left is ignored, and right acceleration is forced. The hero walks off the right edge of the window (`x > 512`) to set `passed`. There is no flag-pole collision — the victory sign (ID 12) is scenery and is not even loaded on world 2 (see [known quirks](known-quirks.md)).
