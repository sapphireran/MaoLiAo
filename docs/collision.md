# Collision

Every hit test shares one idea: take the moving sprite’s **four corners**, shrink them by 1 px, and ask whether any corner sits inside the target’s axis-aligned box.

```text
r[0] = (x - xmap + 1,           y + 1)
r[1] = (x - xmap + WIDTH - 1,   y + 1)
r[2] = (x - xmap + 1,           y + HEIGHT - 1)
r[3] = (x - xmap + WIDTH - 1,   y + HEIGHT - 1)
```

`xmap` is `(int)hero.x0` so tests run in **world** space. `isHit(p1, p2)` is a point-in-rect for those four points against `p2[0]..p2[1]` (top-left / bottom-right). It is **not** a full rectangle-vs-rectangle test: a large box can overlap a tile without any of its inset corners landing inside. For 32×32 actors versus 32×N tiles this is usually good enough.

A portable clone of the test lives in `examples/aabb_collision.cpp`.

## Solid tiles (`hitMap`)

`Scene::map[]` entries with `id` in `(0, 11)` are treated as colliders. Ids 11+ are scenery (grass tufts, flag, water, trees) and are skipped.

Box size:

- Default: `xAmount * 32` by `yAmount * 32`.
- Ids **8** and **10** (pipe mouths): each amount cell is **64×64** (`2 * WIDTH` by `2 * HEIGHT`). That matches how `Scene::show` blits those ids.

World 3 extra rule, inside `hitMap` when the `world` argument is 3:

- If the tile is **not** id 2 (cloud) **and** the hero does not have `isShoot` (the food / star power-up), set `died = true`.
- Clouds remain safe landing pads.

Most in-game `hitMap` calls pass the real `world`. Horizontal wall resolution in `Role::action` passes `world = 1` on purpose so walking into a pipe in world 3 does not instantly kill during the X pass — the lethal test still happens on the vertical / generic queries.

## Coins and food

Coins are stored as tile coordinates. Their box is exactly one 32×32 cell.

Food (the weapon pickup) is stored in **pixels** already. The box is `3*WIDTH/2 + 4` by `4*HEIGHT/5` (52×25.6, truncated in integer blit to 52×25). Collecting it sets `hero.isShoot = true`. World 3 also uses that flag as “touching pipes does not kill.”

On collect, the point is zeroed and `Scene::setScorePos` records a short “+score” spark in the `score[]` overlay list (max 5 concurrent).

## Enemies

An enemy is alive when `turn != 0`. Box is 32×32 at `(enemy.x, enemy.y)` in world pixels.

- If the hero is falling (`vY > 0`) and the boxes overlap: stomp. +5 score, spawn a bomb sprite, zero the enemy.
- If the boxes overlap and `vY <= 0`: the hero dies.

Enemies reverse `turn` when the cell they occupy is solid **or** the cell one body-width ahead and one pixel down is empty (walked off a ledge). They do not have their own gravity.

## Bullets

`bullteFlying` (typo in the original) steps `x` by `LEHGTH_INTERVAL_BULLET * turn` (4 px per show-tick, facing `±1`).

A bullet dies and spawns a bomb when:

- it hits an enemy (+5 score, enemy cleared), or
- `hitMap` with `world = 1` reports a tile, or
- `x > MAX_DISTANCE` (480), or
- it leaves the screen (`x > 512` or `x < -32`).

Note: `MAX_DISTANCE` is compared to the bullet’s **screen** `x`, not world x, so shots cannot travel past the right half of the window.

Fire rate: first tap shoots immediately. While `J` is held, shots repeat every `TIME_INTERVAL_BULLET` (0.2 s).

## Resolution order in `Role::action`

1. Jump / gravity / stomp / fall-off-screen.
2. Horizontal accel + friction + integrate.
3. Ending / passed flags.
4. If `hitMap(x, y, scene, 1)`: clamp to `XRIGHT` or undo the step; zero `vX`.
5. Speed cap.
6. Shoot.
7. Clamp `XLEFT`; if past `XRIGHT` and not ending, push `x0`.
8. Enemy patrol.
9. Coins, food, side-hit death.

Because wall tests use inset corners, the hero can appear to sink a pixel into a tile before `isHit` returns true. Ground snap then hides most of that.

## What is *not* collided

- Scenery ids 11–14 (decorative).
- The victory sign (id 12): you “clear” by walking far enough, not by touching the sign.
- Water (id 13): visual only; falling into a pit is the death, via `y > YSIZE`.
- Sky / background.
