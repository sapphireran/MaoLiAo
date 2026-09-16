# Physics

Every integrator in the game is the same textbook step in `Inertia::move`:

```
X = v * t + 0.5 * a * t * t
v = v + a * t
return X
```

`t` is always `TIME = 0.01`. The function **writes the new velocity back** through `double& v`.

EasyX y grows downward. The homework keeps a “meters” space (`REAL_HEIGHT = 3.5`) and a pixel space (`UNREAL_HEIGHT = 3 * 32 + 5 = 101`). The scale is

```
px = meters * UNREAL_HEIGHT / REAL_HEIGHT
   = meters * 101 / 3.5
   ≈ meters * 28.857142857
```

`examples/src/jump_lab.cpp` reprints the numbers below.

## Tunables (`define.h`)

| Macro | Value | Role |
|---|---|---|
| `TIME` | `0.01` | frame dt (seconds) |
| `G` | `30.0` | gravity (m/s²), large on purpose so air time is short |
| `REAL_HEIGHT` | `3.5` | intended jump height in “meters” |
| `UNREAL_HEIGHT` | `101` | jump height in pixels |
| `V_MAX` | `8.0` | horizontal speed cap (m/s) |
| `A_ROLE` | `20.0` | walk acceleration (m/s²) |
| `T1` | `0.5` | time to `V_MAX` on high-friction tiles |
| `T2` | `1.2` | medium friction |
| `T3` | `1.5` | low friction |
| `XLEFT` | `0` | hero screen clamp |
| `XRIGHT` | `192` (`6 * WIDTH`) | camera rail; hero sticks here while the world slides |
| `K_MAP_BG` | `5` | sky moves 1 px per 5 px of map |

## Jump

Grounded jump (worlds 1–2) or *any* `CMD_UP` in world 3 (infinite air jump):

```
vY = -sqrt(2 * G * REAL_HEIGHT) = -sqrt(210) ≈ -14.49137674
isFly = true
```

Each airborne frame:

```
meters = Inertia::move(vY, TIME, +G)     # gravity pulls vY toward +∞
yy    += meters * (101 / 3.5)            # written as yy - (-move * scale)
y      = (int)yy
```

The first line of the source is

```cpp
myHero.yy = myHero.yy - (-Inertia::move(myHero.vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT);
```

which is just `yy += move(...) * scale`. After landing, `y` is snapped to the tile grid through the sprite center: `(y + HEIGHT/2) / HEIGHT * HEIGHT`.

Continuous (not discrete) kinematics would peak at exactly **3.5 m / 101 px**. The Euler-like step undershoots by a few millimetres; the lab prints `3.4999` m and `100.997` px.

Falling off the bottom (`y > YSIZE`) sets `died` and plays `music_died`.

World 3 does not require `isFly == false` to jump, so the player can flap.

## Horizontal motion

`A_ROLE` is applied as `-20` (left) or `+20` (right). During the ending rail (`hero.ending == true`) right-walk is forced.

Friction only runs when velocity and walk-accel **oppose** (`vX * a <= 0` and `vX != 0`) and a floor tile is under the feet:

```
a1 = k * G * map->u
```

`k` is **not** `sign(vX)`. The homework computes

```
Xabs = |vX|
if vX < 0:  k = Xabs / vX + 3   →  -1 + 3 = 2
else:       k = Xabs / vX - 3   →   1 - 3 = -2
```

so `|k| = 2`. That doubles the friction impulse versus a textbook `a = ±μg`. After the step, a velocity sign flip is zeroed (`if tmp * vX < 0: vX = 0`). `|vX|` is then clamped to `V_MAX`.

### Tile friction `u`

`Scene::createMap` stores

```
u = (V_MAX / T) / G
```

| Tile ids | `T` | `u` | Feel |
|---|---|---|---|
| 1, 3, 4, 5 (and id 2 in worlds 1–2) | `T2 = 1.2` | `8 / 1.2 / 30 = 0.2̅` | grass / snow / clouds |
| 6 | `T1 = 0.5` | `8 / 0.5 / 30 = 0.5̅` | the short icy slab on world 1 |
| everything else | `T3 = 1.5` | `8 / 1.5 / 30 ≈ 0.1778` | pipes / scenery |

World 3 overrides the switch: ids 1 and 3–6 use `T1` (grippy), id 2 uses `T2`, default `T3`.

## Camera rail

The hero is **not** free to walk off the right side of the window.

- `x < XLEFT` → clamp, zero `vX`.
- `x > XRIGHT` and not yet in the ending zone → keep the sprite at `XRIGHT` and subtract the overflow from `hero.x0` (the world origin, a **negative** number as you walk right).
- `Scene::action` copies `xMap = (int)hero.x0`. Enemies, coins, and tiles are drawn at `xMap + world_x`.
- When `x > XSIZE` (512) the level is cleared (`passed = true`).
- `Scene::isEnding(distance)` trips when the hero’s **world** x (`-x0 + x`) exceeds `94 * 32` (worlds 1 and 3) or `104 * 32` (world 2). Then `ending = true`, right-walk is forced, and the sprite is allowed past `XRIGHT` toward the window edge.

## Sky parallax

`mapsky.bmp` is 512×1536 (four stacked 384 px skies). `yBg = -(world - 1) * YSIZE` selects the strip. Horizontally the sky only moves when the hero is pinned at `XRIGHT` with `vX > 0` and `xMap` actually changed:

```
bgStep = |vX| * TIME * (101 / 3.5) / K_MAP_BG
xBg   -= bgStep
```

When `xBg <= -sky_width` it wraps to 0; a second blit at `width + xBg` fills the seam.

## Walk cycle (not physics, but pose)

`hero_iframe` is 1 or 2 from `(-x0 + x) / STEP % 4` with `STEP = 10`. Enemies animate with `enemy_iframe += TIME * 5` (period 2 frames). Those counters only choose sprite columns; they do not move the body.

## Worked jump table

`vY0 = -√210`, `dt = 0.01`, `a = +30`, pixel scale `101/3.5`.

| t (s) | vY after step (m/s) | yy (px), start 64 |
|---|---|---|
| 0.00 | -14.4914 | 64.000 |
| 0.01 | -14.1914 | 59.861 |
| 0.24 | -7.2914 | ~-36.8  (near apex) |
| 0.49 | +0.2086 | ~-36.9 then falling |
| … | → +∞ | falls until a tile or `y > 384` |

The lab’s `jump_lab` integrates until `vY` crosses 0 and reports the discrete apex.
