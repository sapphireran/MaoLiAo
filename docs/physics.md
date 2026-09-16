# Physics

All of the motion is one function and a handful of `#define`s from `define.h`.

```cpp
double Inertia::move(double& v, double t, double a)
{
    double X = v * t + 1.0 / 2.0 * a * t * t;
    v = v + a * t;
    return X;
}
```

`t` is always `TIME = 0.01`. The returned `X` is in a made-up “metre” space.
Pixels are `X * UNREAL_HEIGHT / REAL_HEIGHT = X * 101 / 3.5`.

## Jump

On the ground (`isFly == false`) a rising `CMD_UP` sets

```
vY = -sqrt(2 * G * REAL_HEIGHT) = -sqrt(210) ≈ -14.491376751
isFly = true
```

World 3 skips the ground check, so W/K is an infinite air jump.

Each airborne frame:

```
dy_m = Inertia::move(vY, TIME, G)          // G = +30, screen-down
yy  += dy_m * 101 / 3.5
y    = (int)yy
```

The source writes this as `yy - (-move(...) * scale)`. Same thing.

Closed form, no collisions:

| Quantity | Formula | Value |
| --- | --- | --- |
| Time to apex | `sqrt(2 H / G)` | `0.4830458915` s |
| Frames to apex | that / `0.01` | 48 (first frame with `vY > 0` is 49) |
| Apex rise | `REAL_HEIGHT * 101 / 3.5` | **101 px** |
| Hang time (flat landing) | `2 * t_apex` | `0.9660917831` s ≈ 97 frames |

`examples/bin/jump_table.py` and `examples/cpp/jump_table.cpp` print the per-frame
arc. The discrete peak in the kit is **100.996 px on frame 48**, then the next
step crosses zero velocity. The authored `UNREAL_HEIGHT` is `3 * 32 + 5 = 101`.

Landing snaps with integer arithmetic:

```
y = (y + HEIGHT/2) / HEIGHT * HEIGHT    // 32-px grid
```

and clears `vY`. A falling overlap with an enemy (`vY > 0`) is a stomp (+5, bomb).
`y > 384` is a pit death.

## Horizontal

`A_ROLE = 20` while A or D is held. The ending rail forces D.

Friction only when `vX != 0` and `vX * a <= 0` (coasting or reversing) **and**
the foot sensor hits a tile:

```
u  = (V_MAX / T) / G          // stored on the tile
k  = -2 when vX > 0
k  = +2 when vX < 0
a1 = k * G * u
```

`T` is `T1 = 0.5` (ice, tile id 6), `T2 = 1.2` (grass / snow / clouds),
or `T3 = 1.5` (everything else).

| Surface | T | u | coast accel (px-space, sign opposite v) |
| --- | --- | --- | --- |
| Ice (id 6) | 0.5 | 8/15 ≈ 0.5333 | `60 u` → 32 / s² in metres |
| Grass / snow / cloud | 1.2 | 8/36 ≈ 0.2222 | `60 u` |
| Default | 1.5 | 8/45 ≈ 0.1778 | `60 u` |

Air has `a1 = 0`. There is no air-control drag.

`vX` is then clamped to `V_MAX = 8`. After the metre-space step, a sign change
in `vX` is zeroed (“no bounce when you finish sliding”).

At max speed, one frame travels

```
8 * 0.01 * 101 / 3.5 ≈ 2.30857 px
```

A full jump at `V_MAX` therefore covers about `2.30857 * 96.6 ≈ 223 px` —
enough for every authored grass gap in world 1 if you arrive at speed.

## Standing-still NaN

`k` is computed **before** the `vX != 0` guard:

```
k = Xabs / vX - 3;    // vX == 0 → 0/0
```

IEEE `0.0 / 0.0` is NaN. The friction block is skipped, so gameplay usually
survives, but `k` is a NaN every grounded idle frame. The kit mirrors this
only when you opt into `strict_nan_k`.

## Shoot cadence

Unlocked by the flower (`isShoot = true`). First tap fires immediately.
Held J waits `TIME_INTERVAL_BULLET = 0.2` s between shots.
Bullet speed is `LEHGTH_INTERVAL_BULLET = 4` px/frame (original typo).
Range cap `MAX_DISTANCE = 480` is an **on-screen x**, not a world distance,
so shots vanish near the right half of the 512-px window.

## What the kit does not fake

- Sprite iframe timing (`STEP`, `F`, coin flash).
- MCI / `Sleep` during UI strips.
- The EasyX `SRCAND` + `SRCPAINT` blit.
