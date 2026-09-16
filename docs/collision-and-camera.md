# Collision and camera

## Hit test

Every query (hero vs tile, coin, flower, enemy; bullet vs tile / enemy) uses
the same four-point test. The moving box is **inset by 1 px** on each edge:

```
r[0] = (-x0 + x + 1,     y + 1)
r[1] = (-x0 + x + W - 1, y + 1)
r[2] = (-x0 + x + 1,     y + H - 1)
r[3] = (-x0 + x + W - 1, y + H - 1)
```

`isHit` returns true if **any** of those four points lies inside the target
axis-aligned rectangle `[x0, x1] × [y0, y1]` inclusive.

Coins and enemies are 32×32. The flower is `3*WIDTH/2+4` by `4*HEIGHT/5`
(52×25.6, truncated in pixel draws).

## Which tiles collide

`hitMap` walks `Scene::map` while `id > 0 && id < 11 && i < MAP_NUMBER`.

| id | Name | Solid? | Box |
| --- | --- | --- | --- |
| 1 | Grass top | yes | `xAmount × yAmount` tiles |
| 2 | Cloud | yes | same |
| 3 | Dirt | yes | same |
| 4 | Snow fill | yes | same |
| 5 | Snow top | yes | same |
| 6 | Ice top | yes | same |
| 7 | Pipe shaft | yes | 1×`yAmount` tiles (draw is 2 tiles wide) |
| 8 | Upside-down pipe mouth | yes | **2×2 tiles** |
| 9 | (unused in V2.0 maps) | yes | 1×1 |
| 10 | Upright pipe mouth | yes | **2×2 tiles** |
| 11 | Background grass | no | scenery sheet |
| 12 | Goal sign | no | scenery sheet |
| 13 | Water | no | scenery sheet |
| 14 | Tree | no | scenery sheet |

World 3 special case: a solid that is **not** a cloud (`id != 2`) kills the
hero unless `isShoot` is already true. The comment calls the flower a star.

Foot sensor is `hitMap(x, y+1)`. Empty means you start falling.
The horizontal probe uses the current `x, y` and, on a hit, refuses the step
and zeroes `vX`. If `x > XRIGHT` it also clamps to `XRIGHT` (the rail).

## Camera rail

`XRIGHT = 6 * 32 = 192`. While `ending == false` and `x > 192`:

```
x0 -= (x - 192)
x   = 192
xx  = 192
```

`x0` becomes more negative. `Scene::action` sets `xMap = (int)x0` and, when
the hero is glued to 192 and still walking right, slides the sky by

```
|vX| * TIME * 101 / 3.5 / K_MAP_BG
```

(`K_MAP_BG = 5`). The sky blit wraps at `-img_bg.width`. The sky strip is
chosen by world: `yBg = -(world - 1) * YSIZE`.

Left clamp is `x = 0`, `vX = 0`. There is no leftward scroll.

## Ending rail

`Scene::isEnding(-x0 + x)` becomes true after:

| World | Distance |
| --- | --- |
| 1 | `> 94 * 32` = 3008 px |
| 2 | `> 104 * 32` = 3328 px |
| 3 | `> 94 * 32` = 3008 px |

Then `ending = true`: jump is blocked (except world 3’s air jump), left is
ignored, and right is forced. When **on-screen** `x > 512`, `passed = true`.
The camera is no longer pinned, so the hero walks off the right edge.

## Enemy patrols

Every other centiframe (`(int)(enemy_iframe * 100) % 2 == 0`) each living
enemy steps `turn * ENEMY_STEP` (1 px). They flip if the body overlaps a
solid **or** the toe one tile ahead in `turn` is in the air. `turn == 0`
is the dead / unused slot.

## Bullet collisions

`bullteFlying` probes with `hitMap(..., world=1)` — world 3’s touch-death
rule does **not** apply to bullets. A hit, an enemy, or `x > 480` spawns a
bomb and clears the shot. Enemy hits score +5.
