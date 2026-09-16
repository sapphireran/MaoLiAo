# Physics

The integrator is eight lines in `Inertia::move`. Everything else is how `Role::action` chooses `a` and how it maps model metres onto 32-pixel tiles.

## Integrator

```cpp
double Inertia::move(double& v, double t, double a)
{
    double X = v * t + 1.0 / 2.0 * a * t * t;
    v = v + a * t;
    return X;
}
```

That is the standard constant-acceleration step:

\[
\Delta x = v t + \tfrac{1}{2} a t^{2},\qquad v \leftarrow v + a t
\]

`t` is always `TIME` (0.01). The function **mutates** `v`. Callers that need the pre-step speed save a copy (`tmp = myHero.vX`) to detect a sign flip (skid-to-stop), then zero `vX`.

Displacement is then scaled into pixels:

```cpp
H = Inertia::move(v, TIME, a) * UNREAL_HEIGHT / REAL_HEIGHT
```

`UNREAL_HEIGHT / REAL_HEIGHT` = `101 / 3.5` ≈ **28.857 px per model metre**.

The portable copy is `examples/include/maoliao/inertia.h`.

## Jump

On the frame where `CMD_UP` is held and the hero is grounded (`isFly == false`) and not in the ending auto-walk — **or** any time in world 3:

```cpp
vY = -sqrt(2 * G * REAL_HEIGHT)
isFly = true
```

Energy identity \(v = \sqrt{2 g h}\) with \(g = 30\), \(h = 3.5\) gives

\[
|v_Y| = \sqrt{210} \approx 14.4914
\]

While airborne, the same integrator runs with acceleration `+G` (down):

```cpp
yy = yy - (-Inertia::move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT)
```

the double negation is “EasyX Y grows downward, model Y was written as up-positive in the comment.” Apex height in pixels is about `REAL_HEIGHT * 101 / 3.5` = 101 px if started from rest in model space — the same as `UNREAL_HEIGHT`. That is a bit over three tiles.

World 3 reapplies this impulse every frame `W`/`K` is held, which is why the stage plays like a flap.

Landing: `hitMap(x, y+1)` while `vY > 0` clears `isFly`, zeros `vY`, and snaps `y` to the tile grid through `(y + HEIGHT/2) / HEIGHT * HEIGHT`. Walking off a ledge sets `isFly` when the probe at `y+1` misses.

Falling below `YSIZE` is a pit death.

## Horizontal run

Input sets `a = ±A_ROLE` (`±20`). Ending forces `+A_ROLE`.

If velocity and input accel have opposite signs (or input is zero) and the hero is standing on a tile, a friction accel `a1` is added:

```cpp
k = (vX < 0) ? (Xabs / vX + 3) : (Xabs / vX - 3);
// Xabs / vX is sign(vX) → k is +2 when moving left, -2 when moving right
a1 = k * G * map->u;
```

So \(|a_1| = 2 G u = 2 V_\max / T\).

| Surface | `T` | `u = (V_MAX/T)/G` | `|a1|` |
| --- | --- | --- | --- |
| id 6 (world 1 “ice” strip), world 3 default solids | T1 = 0.5 | 8/0.5/30 = 0.533 | 32 |
| grass / snow / clouds (ids 1–5, 2) | T2 = 1.2 | 8/1.2/30 ≈ 0.222 | ≈ 13.33 |
| other ids | T3 = 1.5 | 8/1.5/30 ≈ 0.178 | ≈ 10.67 |

In air `map == NULL` so `a1 = 0` (no air drag).

After the step, speed is clamped:

```cpp
if (|vX| > V_MAX) vX = sign(vX) * V_MAX
```

`V_MAX` is in **model** units. Pixel speed at the cap is

```
8 * 0.01 * 101 / 3.5 ≈ 2.31 px / frame
≈ 231 px / s
```

## Camera lock

If `x < 0`, snap to 0 and zero `vX`.

If `x > 192` and not ending:

```cpp
x0 -= (x - 192)
x = 192
```

The world slides left; the sprite stays in the left-center of the screen.

If a horizontal step intersects a tile (`hitMap` with `world` forced to `1`, so world-3 kill logic is skipped for this correction), the step is undone and `vX` is zeroed. If the sprite is already past `XRIGHT`, it is clamped to `XRIGHT` instead.

## Enemies

Every other tick of `enemy_iframe * 100` (`% 2 == 0`), each live enemy (`turn != 0`) moves `turn * ENEMY_STEP` (1 px). They reverse if the body overlaps a tile **or** the probe one tile ahead and one pixel down is in the air. They do not use `Inertia`.

## Bullets

Spawned at the hero’s chest, same facing. Each `Role::show`, `x += 4 * turn`. Expire off-screen, on tile/enemy hit, or when `x > MAX_DISTANCE` (480). That last check is a **screen** X, so shots fired while the camera has scrolled still die at pixel 480.

Hold-to-fire: first shot on the press edge; further shots every `0.2` s while `J` stays down.

## Worked jump table

The `examples/demos/jump_arc` program prints the airborne trajectory with `dt = 0.01` until `vY` has been positive and `y` returns to the start. Expected apex ≈ 101 px above the launch pixel if no ceiling.
