# Physics

All motion goes through `Inertia::move`, then a pixel-scale factor
`UNREAL_HEIGHT / REAL_HEIGHT`. The split exists so jump *feel* can be
tuned in "meters" while sprites stay on a 32 px grid.

## Integrator

```cpp
double Inertia::move(double& v, double t, double a)
{
    double X = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return X;
}
```

Semi-implicit in velocity (velocity is updated after the displacement
uses the *old* `v`). `t` is always `TIME` (0.01).

Portable copy: `maoliao::inertia_move` in
`examples/include/maoliao_core.hpp`.

## Constants

From `define.h`, cleaned of the trailing-semicolon macros:

| Symbol | Value | Role |
| --- | --- | --- |
| `TIME` | 0.01 s | Tick |
| `G` | 30 | Gravity (screen-down positive after the jump sign flip) |
| `REAL_HEIGHT` | 3.5 | Design jump height in "meters" |
| `UNREAL_HEIGHT` | 3*32+5 = 101 | Pixel jump height the player sees |
| `V_MAX` | 8 | Horizontal speed cap (pre-scale) |
| `A_ROLE` | 20 | Left/right hold acceleration |
| `T1` / `T2` / `T3` | 0.5 / 1.2 / 1.5 | Time-to-`V_MAX` used to derive friction `u` |
| `XLEFT` | 0 | Screen clamp |
| `XRIGHT` | 192 | Camera pin; hero cannot walk past this unless `ending` |
| `STEP` | 10 | Walk-cycle distance |
| `ENEMY_STEP` | 1 | Enemy patrol pixels per allowed tick |
| `K_MAP_BG` | 5 | Parallax: map/bg speed ratio |

Pixel displacement after one integrator step:

```
pixels = inertia_move(v, TIME, a) * (UNREAL_HEIGHT / REAL_HEIGHT)
       = inertia_move(v, TIME, a) * (101 / 3.5)
```

Both axes use that scale. A `V_MAX` of 8 therefore becomes about
`8 * 0.01 * 101/3.5 ≈ 2.31` px per tick, ≈ 231 px/s, before friction.

## Jump

Impulse (only when `!isFly && !ending`, **or** always in world 3):

```
vY = -sqrt(2 * G * REAL_HEIGHT) = -sqrt(210) ≈ -14.491
isFly = true
```

That is the ballistic speed that reaches 3.5 "meters" under `G = 30`
with no drag. Each air tick:

```
yy += inertia_move(vY, TIME, G) * (101 / 3.5)
y   = (int)yy
```

(`role.cpp` writes this as `yy - (-move(...))`.)

Landing: `hitMap(x, y+1)` while `vY > 0` clears fly, zeros `vY`, and
snaps `y` to the nearest tile multiple via
`(y + HEIGHT/2) / HEIGHT * HEIGHT`.

Stomp: same falling test (`vY > 0`) plus `hitEnemy`. Enemy is zeroed,
score +5, bomb sprite queued.

Fall death: `y > YSIZE` while falling.

World 3's extra jump clause (`CMD_UP && world == 3`) is the Flappy
recharge: air jumps reset `vY` to the same impulse every tap.

Apex time (analytic, no collision): `t = |vY| / G ≈ 0.483 s` ≈ 48
ticks. Pixel apex ≈ `UNREAL_HEIGHT` = 101 if the scale were applied to
the real-meter height 3.5 — which is exactly why those two constants
exist. The discrete integrator undershoots slightly; `examples/06`
prints the measured peak.

## Horizontal motion

Desired accel `a`:

- `CMD_LEFT` and not ending: `a -= A_ROLE`
- `CMD_RIGHT` **or** ending: `a += A_ROLE`

Friction accel `a1` only when the player is **not** accelerating in the
direction of travel (`vX * a <= 0 && vX != 0`) and a floor `map` is
under the feet:

```
k  = (vX < 0) ? (Xabs/vX + 3) : (Xabs/vX - 3)
   = (vX < 0) ? 2 : -2          // because Xabs/vX is the sign
a1 = k * G * map->u
```

So friction is `2 * G * u` against velocity. Tile `u` is assigned in
`Scene::createMap`:

```
u = (V_MAX / T) / G
```

| Tile ids | T | Feel |
| --- | --- | --- |
| 6 (ice-ish / "high friction" comment vs value) | T1 = 0.5 | Strongest `u` |
| 1, 3, 4, 5, 2 | T2 = 1.2 | Medium |
| everything else | T3 = 1.5 | Weakest |

Comments in `define.h` say T1 is "摩擦力大" (large friction). The
formula matches: larger `u` → larger `a1` → faster stop. World 3
collapses 1–6 onto T1.

After the step, if `vX` **crossed through zero**, it is forced to 0
(no oscillation). Then `|vX|` is clamped to `V_MAX`.

Wall: `hitMap` at the new position (always with `world=1` so world-3
death-on-touch does not trigger here) rejects the step, restores the
previous `x`, zeros `vX`.

## Camera pin

The hero's screen `x` lives in `[XLEFT, XRIGHT]` = `[0, 192]` until
the ending walk.

When `x` would exceed 192 and `!ending`:

```
x0 -= (x - XRIGHT)
x = XRIGHT
xx = x
```

`x0` becomes a more negative scroll. `Scene::action` copies `xMap = x0`
and, if the hero is on the pin and moving right, shifts `xBg` by

```
bgStep = |vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG
```

Background therefore crawls at 1/5 of the map. When `xBg` passes
`-bg.width` it wraps to 0 so the sky loops.

Enemies, bombs, and coins are drawn at `x0 + worldX` so they scroll
with the map. Bullets are stored in **screen** x and do not add `x0`
when drawn — they fly in viewport space. `MAX_DISTANCE` (480) is also
a screen-x cutoff.

## Enemies

Every tick where `(int)(enemy_iframe * 100) % 2 == 0`, each living
enemy (`turn != 0`) steps `turn * ENEMY_STEP`. Reverse `turn` if the
body overlaps a solid **or** the cell one tile ahead-and-down is empty
(walked off a ledge).

`enemy_iframe` also drives the walk animation (`TIME * 5`, wrap at 3),
so patrol and animation share one clock.

## Bullets

Spawn at hero screen x (offset by facing) and hero y. Each `show`:

```
x += LEHGTH_INTERVAL_BULLET * turn    // 4 px
```

Expire if off-window, `x > MAX_DISTANCE`, `hitEnemy`, or `hitMap`.
Hold-to-fire spaces shots by `TIME_INTERVAL_BULLET` (0.2 s) once
`shootButtonDown` is latched. First tap fires immediately.

`isShoot` is granted by eating food (the mushroom). World 3 also uses
that flag as "do not die on pipe contact."

## Scale reminder

When porting, do not apply the 101/3.5 factor twice. The impulse
`sqrt(2*G*REAL_HEIGHT)` is in the **unscaled** velocity units that
`Inertia::move` consumes. The factor is applied only to the returned
displacement.
