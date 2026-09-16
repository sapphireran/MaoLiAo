# Physics

Every motion step is the same integrator in `Inertia::move`:

```
X = v * t + 0.5 * a * t * t
v = v + a * t
```

`t` is always `TIME = 0.01`. `Role::action` converts the returned **world metres** into **screen pixels** with

```
pixels = X * UNREAL_HEIGHT / REAL_HEIGHT
       = X * 101 / 3.5
       ≈ X * 28.857
```

`REAL_HEIGHT` (3.5) is the design jump in “metres”. `UNREAL_HEIGHT` (`3 * 32 + 5 = 101`) is how many pixels that jump should travel on screen. The comments in `define.h` say the hero should *feel* like a 3.5 m jump.

The portable reconstruction is `examples/include/maoliao_model.hpp` (`integrate`, `jump_speed`, `pixels_from_metres`).

## Constants

| Macro | Value | Used for |
| --- | --- | --- |
| `G` | `30.0` | Downward acceleration (both jump and fall) |
| `V_MAX` | `8.0` | Horizontal speed cap, **before** the pixel scale |
| `A_ROLE` | `20.0` | Horizontal run acceleration while A/D is held |
| `T1` | `0.5` | Time to `V_MAX` on high-friction tiles (pipe middle, `id == 6`) |
| `T2` | `1.2` | Time to `V_MAX` on mid-friction tiles (grass, snow, clouds) |
| `T3` | `1.5` | Time to `V_MAX` on everything else |
| `REAL_HEIGHT` | `3.5` | Jump height in integrator units |
| `UNREAL_HEIGHT` | `101` | Jump height in pixels |
| `XRIGHT` | `192` | Screen-space right rail (`6 * 32`) |

Friction on a tile is **not** stored as a time. `createMap` writes

```
u = (V_MAX / T) / G
```

so that `a_friction = ± G * u` has magnitude `V_MAX / T`. High `u` (small `T`) stops you faster.

| Tile `id` | `T` | `u = (8 / T) / 30` |
| --- | --- | --- |
| 1, 3, 4, 5, 2 | `T2 = 1.2` | `0.2222…` |
| 6 | `T1 = 0.5` | `0.5333…` |
| default (pipes, scenery leftovers) | `T3 = 1.5` | `0.1777…` |

World 3 overrides the first group: ids 1 and 3–6 all use `T1` (grip like a pipe). Clouds stay on `T2`.

## Jump

Takeoff only when `CMD_UP` is set and either `isFly == false` and `ending == false`, **or** `world == 3` (air jump). The vertical speed is replaced, not added:

```
vY = -sqrt(2 * G * REAL_HEIGHT)
   = -sqrt(210)
   ≈ -14.491376738
```

That is the closed-form speed for a jump that peaks at `REAL_HEIGHT` under constant `G`. While `isFly` is true, each tick:

```
dy_metres = Inertia::move(vY, TIME, G)     // G is +30, so vY climbs toward +∞
yy += dy_metres                            // then the code writes yy - (-dy)
y   = (int)yy
```

The source line is:

```cpp
myHero.yy = myHero.yy - (-Inertia::move(myHero.vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT);
```

The double negation is just `yy += pixels`. Negative `vY` therefore **decreases** screen `y` (up the window). After the apex, `vY` is positive and the sprite falls.

Landing: a map hit with `vY > 0` snaps `y` to the tile grid (`(y + 16) / 32 * 32`), zeroes `vY`, and clears `isFly`. A map hit with `vY <= 0` (head-bonk) also zeroes `vY` but leaves `isFly` true, so the next tick starts a fall.

If `vY > 0` and the sprite overlaps an enemy, that enemy is stomped (`+5`, bomb flash) instead of killing the hero.

Falling past `y > 384` is death.

Leaving the ground without jumping (walking off a ledge) is detected by a probe at `(x, y + 1)`: no tile ⇒ `isFly = true` with the current `vY` (usually 0), so the first airborne tick starts from rest and gravity takes over.

## Run

`CMD_LEFT` contributes `-A_ROLE`, `CMD_RIGHT` (or `ending`) contributes `+A_ROLE`. Friction `a1` is applied only when velocity and the input accel have **opposite signs or the input is zero** (`vX * a <= 0 && vX != 0`):

```
k = sign-ish helper:  +4 when vX < 0,  -2 when vX > 0
a1 = k * G * map->u
```

`k` is not a clean `±1`. For `vX > 0`, `k = 1 - 3 = -2`. For `vX < 0`, `k = -1 + 3 = +2`? Wait: `k = Xabs / vX + 3` when `vX < 0`, and `Xabs / vX` is `-1`, so `k = 2`. The comment says “+3-3调节惯性”. The extra factor of 2 makes friction twice `G u` on the positive side and… `k = 2` on the negative side as well. So `|a1| = 2 G u = 2 V_MAX / T`.

If there is no tile underfoot, `a1 = 0` (air has no horizontal drag).

After the integrator, a sign change on `vX` is snapped to `0` (no oscillation through zero). Then `|vX|` is clamped to `V_MAX`.

A horizontal map hit (probe at the new `(x, y)` with `world` forced to `1` so world-3 contact does not kill during the wall test) rewinds `x` by the step just taken, or clamps to `XRIGHT` if the hero is already past the rail.

## Camera rail

```
if x < 0:          snap to 0, kill vX
if x > 192 and not ending:
    x0 -= (x - 192)
    x   = 192
```

`x0` is the world origin drawn under the screen. After the rail, `Scene::action` sets `xMap = (int)x0` and, when the hero is pinned to `XRIGHT` and still moving right, shifts the sky by

```
bgStep = |vX| * TIME * 101 / 3.5 / 5
xBg   -= bgStep
```

`K_MAP_BG = 5` is the parallax ratio: five pixels of map for one pixel of sky. The sky image is blit twice and wrapped when `xBg <= -width`.

World-space distance used for the finish line is `x - (int)x0`. Enemies are stored in world pixels and drawn at `x0 + enemy.x`.

## Worked jump (first five ticks)

`v0 = -√210`, `t = 0.01`, `a = 30`.

| tick | v before | Δmetres | v after | Σmetres | Σpixels |
| --- | --- | --- | --- | --- | --- |
| 1 | −14.4914 | −0.1434 | −14.1914 | −0.1434 | −4.14 |
| 2 | −14.1914 | −0.1404 | −13.8914 | −0.2838 | −8.19 |
| 3 | −13.8914 | −0.1374 | −13.5914 | −0.4212 | −12.16 |
| 4 | −13.5914 | −0.1344 | −13.2914 | −0.5556 | −16.04 |
| 5 | −13.2914 | −0.1314 | −12.9914 | −0.6870 | −19.83 |

Peak is near tick 48 (`t ≈ 0.48 s = √210 / 30`). Discrete integration lands within a fraction of a pixel of `101` px. `examples/src/jump_trajectory.cpp` prints the full table and checks that bound.

## Why world 3 feels different

1. Every `W`/`K` press resets `vY` to the takeoff speed — you can flap.
2. Contact with any colliding tile except clouds sets `died` unless the weapon (star) was picked up. The jump itself does not use a different `G`.
3. Pipe tiles use the high-friction `T1` curve, but you are rarely walking on them.

The integrator is unchanged. The rules around when jump and death fire are what change the feel.
