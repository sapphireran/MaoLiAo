# 03 — Physics

Movement is not a tile-step. It is a 2D kinematic integrator with a fake “meters” space and a pixel scale factor.

The only integrator is `Inertia::move` in `inertia.cpp`:

```cpp
double Inertia::move(double& v, double t, double a)
{
    double X = v * t + 1.0 / 2.0 * a * t * t;
    v = v + a * t;
    return X;
}
```

That is the textbook `s = vt + ½at²` plus an in-place velocity update. `t` is always `TIME` (0.01 s) during gameplay.

## Two spaces

| Space | Units | Used for |
| --- | --- | --- |
| Feel / “real” | fake meters | `vX`, `vY`, `G`, `A_ROLE`, `V_MAX`, `REAL_HEIGHT` |
| Screen | pixels | `Hero::x`, `Hero::y`, `Hero::xx`, `Hero::yy` |

After each integrate:

```text
pixels = meters * UNREAL_HEIGHT / REAL_HEIGHT
       = meters * 101 / 3.5
       ≈ meters * 28.857
```

`UNREAL_HEIGHT` (101 px) is documented as “how high a 3.5 m jump should look.” Every horizontal step uses the same scale so walk speed and jump arc stay consistent.

## Jump

Grounded jump (worlds 1–2) or any-time jump (world 3):

```cpp
if ((KEY & CMD_UP) && !isFly && !ending || ((KEY & CMD_UP) && world == 3))
{
    isFly = true;
    vY = -sqrt(2 * G * REAL_HEIGHT);
}
```

Energy identity: `½ v² = g h` ⇒ `v = sqrt(2 g h)`. With `G = 30` and `REAL_HEIGHT = 3.5`:

```text
vY ≈ -14.4914   (up is negative in screen space)
```

While `isFly`:

```cpp
yy = yy - (-Inertia::move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT);
y  = (int)yy;
```

Gravity is applied as **+G** to `vY`. The double negation is just “add the downward pixel delta.” After the move, `hitMap(x, y+1)` is tested:

- Contact + `vY > 0` (falling) → snap `y` to the tile grid (`(y + HEIGHT/2) / HEIGHT * HEIGHT`), zero `vY`, `isFly = false`.
- Contact + `vY < 0` (rising into a ceiling) → zero `vY` but keep `isFly` true. The cat bonks and falls next ticks.
- No contact while grounded → `isFly = true` (walked off a ledge).
- Falling and `y > YSIZE` → death (pit).

World 3 re-triggers the launch every frame that `CMD_UP` is held, so holding jump flutters. That is the Flappy-Bird homage.

## Horizontal walk

```text
a  = 0
if LEFT  and not ending: a -= A_ROLE (20)
if RIGHT or ending:      a += A_ROLE
```

When the cat is already moving **against** the desired accel (`vX * a <= 0` and `vX != 0`), a friction term `a1` is added:

```text
Xabs = |vX|
k    = (vX < 0) ? Xabs/vX + 3 : Xabs/vX - 3
     = (vX < 0) ? -1 + 3      :  1 - 3
     = ±2
a1   = (map == NULL) ? 0 : k * G * map->u
```

`k` is a sign so friction always faces the current velocity. `map->u` is precomputed per tile:

```text
u = (V_MAX / Tn) / G
G * u = V_MAX / Tn
```

So `|a1| = 2 * V_MAX / Tn`. That is **twice** the textbook deceleration `V_MAX / Tn` — the extra factor of 2 is the `±3` fudge in `k` (“+3-3调节惯性”).

| Tile family | `Tn` | `u` | Feel |
| --- | --- | --- | --- |
| id 6 (pipe middle), most world-3 solids | T1 = 0.5 | `8/0.5/30 = 0.533` | sticky |
| id 1,3,4,5 (ground) and id 2 (cloud) | T2 = 1.2 | `8/1.2/30 ≈ 0.222` | normal |
| default | T3 = 1.5 | `8/1.5/30 ≈ 0.178` | icy |

After integrate, if velocity **crossed through zero** (`tmp * vX < 0`), `vX` is clamped to 0 so the cat does not ping-pong.

Then:

```text
if |vX| > V_MAX:  vX *= V_MAX / |vX|
```

## Camera pin

The cat’s **screen** x is forced into `[XLEFT, XRIGHT] = [0, 192]`.

- `x < 0` → snap to 0, kill `vX`.
- `x > 192` and not `ending` → `x0 -= (x - 192)`, `x = 192`. The world origin slides left, which is how the stage scrolls.
- `ending` is set when `isEnding(-x0 + x)` is true. Then right input is forced, the pin is released, and the cat walks off `x > 512` to clear.

Side collision against a solid (always queried with `world=1` so world-3’s “touch = death” path is skipped here) zeros `vX` and rewinds `xx` by `H`. If `x > XRIGHT` after a hit, it snaps to `XRIGHT`.

## Worked jump numbers

One tick after leaving the ground, no extra input:

```text
vY_0     = -14.4914
Δmeters  = vY_0 * 0.01 + 0.5 * 30 * 0.01²
         = -0.144914 + 0.0015
         = -0.143414
vY_1     = -14.4914 + 0.3 = -14.1914
Δpixels  = -0.143414 * 101 / 3.5 ≈ -4.138
```

Peak is when `vY` crosses 0, about `14.491 / 30 ≈ 0.483` s (48 frames). Apex height in meters is 3.5 by construction; in pixels it is ~101.

The portable check is `examples/01_inertia`.
