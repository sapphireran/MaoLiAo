# Physics

All motion goes through one kinematic step, then a handful of clamps that make
the cat feel like a 32×32 sprite instead of a free particle.

## The integrator

`Inertia::move` is the whole engine:

```
X = v * t + 0.5 * a * t * t
v = v + a * t
return X
```

`t` is always `TIME = 0.01`. The function mutates `v` in place and returns the
displacement in the same unit `v` and `a` used. Horizontal and vertical axes
call it independently.

## Authored constants (`define.h`)

| Symbol | Value | Meaning |
| --- | --- | --- |
| `G` | 30.0 | Gravity (meters / s² in the “real” space) |
| `REAL_HEIGHT` | 3.5 | Authored jump height in meters |
| `UNREAL_HEIGHT` | 101 | Pixel height that 3.5 m maps onto (`3 * 32 + 5`) |
| `V_MAX` | 8.0 | Max horizontal speed in real space |
| `A_ROLE` | 20.0 | Walk acceleration |
| `T1` | 0.5 s | Time to `V_MAX` on high friction (pipe tops, id 6) |
| `T2` | 1.2 s | Medium friction (grass / snow / clouds) |
| `T3` | 1.5 s | Low friction (default / scenery leftovers) |

Pixel displacement is real displacement times `UNREAL_HEIGHT / REAL_HEIGHT`
≈ **28.857 px per real meter**.

## Jump

A grounded `CMD_UP` (or any `CMD_UP` in world 3) sets:

```
isFly = true
vY    = -sqrt(2 * G * REAL_HEIGHT)   // = -sqrt(210) ≈ -14.4914
```

That is the ballistic launch speed that peaks at 3.5 m under gravity 30.
Each airborne tick then does:

```
dy    = Inertia::move(vY, TIME, G)
yy   += dy * UNREAL_HEIGHT / REAL_HEIGHT
y     = (int)yy
```

Note the sign dance in the source:

```
yy = yy - (-Inertia::move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT)
```

`move` already uses +G, so subtracting a negated displacement is just adding
`dy * scale`. After the first tick:

```
dy  = -14.4914 * 0.01 + 0.5 * 30 * 0.0001 = -0.143414
vY  = -14.4914 + 0.3                      = -14.1914
yy += -0.143414 * 28.857…                 ≈ -4.138 px   (up the screen)
```

Time to apex is `v / G ≈ 0.483 s` (~48 ticks). Peak pixel rise is 101 px,
a little over three tiles.

World 3 reapplies the launch speed every time `W`/`K` is down, so the cat can
hover by mashing jump. There is no coyote time and no jump buffer.

## Landing and walk-off

After the vertical integrate, `hitMap(x, y + 1)` probes one pixel below the
sprite.

- Hit and `vY > 0`: `isFly = false`, `vY = 0`, snap `y` to the tile grid
  through `(y + HEIGHT/2) / HEIGHT * HEIGHT`.
- Hit and `vY <= 0`: still zero `vY` (head bump / pipe ceiling) but stay flying
  if you did not set `isFly = false`.
- Miss while grounded: `isFly = true` (walk off a ledge).
- `y > YSIZE` while falling: death (pit).

Falling onto an enemy (`vY > 0` and `hitEnemy`) awards 5 points, writes a bomb
sprite, and clears that enemy. Side or upward contact with `vY <= 0` kills the
hero instead.

## Horizontal motion

Each tick rebuilds acceleration from the held keys:

```
a  = 0
if left  and not ending: a -= A_ROLE
if right or ending:      a += A_ROLE
```

`ending` is the auto-walk after the flag distance. The cat is forced right
until `x > XSIZE` (off the right of the 512 px window), which sets `passed`.

Friction is applied only when velocity and walk acceleration disagree
(`vX * a <= 0` and `vX != 0`), i.e. you released the key or you are braking.
`Xabs` starts as the signed `vX` and is negated only in the leftward branch,
so `k` is symmetric:

```
k  = +2   if vX < 0     // |vX|/vX + 3 = -1 + 3
k  = -2   if vX > 0     // |vX|/vX - 3 =  1 - 3
a1 = k * G * map->u     // 0 if airborne (map == NULL)
```

`u` is stored per tile when the map is built:

```
u = (V_MAX / T) / G
```

| Tile ids | T | u | Notes |
| --- | --- | --- | --- |
| 1, 3, 4, 5 | T2 = 1.2 | 8 / 1.2 / 30 ≈ 0.2222 | Grass / snow |
| 6 | T1 = 0.5 | 8 / 0.5 / 30 ≈ 0.5333 | High-friction pipe (world 1) |
| 2 | T2 = 1.2 | ≈ 0.2222 | Clouds |
| other | T3 = 1.5 | 8 / 1.5 / 30 ≈ 0.1778 | Default |
| world 3 ids 1–6 | T1 = 0.5 | ≈ 0.5333 | Pipes use the sticky set |

`k` only chooses the sign of the friction acceleration so it always opposes
`vX`. After `move`, if `vX` flipped sign, it is snapped to 0 (no oscillation
through rest). Then `|vX|` is capped at `V_MAX` — but only if the **old**
`|vX|` was already above the cap, because `Xabs` is captured before `move`.

Pixel step:

```
H = Inertia::move(vX, TIME, a + a1) * UNREAL_HEIGHT / REAL_HEIGHT
xx += H
x   = (int)xx
```

A full-speed walk is `8 * 28.857 ≈ 230.9 px/s`, about 2.3 px per tick.

## Screen edges

- `x < 0`: clamp, zero `vX`.
- `x > 192` and not ending: slide the world origin (`x0`) instead of the sprite.
- After a horizontal integrate, `hitMap(x, y)` with `world` forced to `1`
  (so world-3 death-on-touch does not fire on a wall slide) either clamps to
  `XRIGHT` or rewinds `xx` by `H` and zeroes `vX`.

## Enemies

Enemies ignore inertia. Every other tick of `enemy_iframe * 100` they step
`ENEMY_STEP = 1` px along `turn` (±1). They flip `turn` when the body overlaps
a solid or the pixel in front-and-below is not solid — a one-pixel patrol
along the current platform.

## Bullets

No gravity. Each show-tick advances `x` by `LEHGTH_INTERVAL_BULLET * turn`
(`4 * ±1`). They die on leaving the window, hitting a tile, hitting an enemy,
or passing `MAX_DISTANCE = 480`. Holding `J` spaces shots by 0.2 s; tapping
fires immediately because `shootButtonDown` is false on the first frame.

## Worked jump table

The portable example `examples/src/demo_jump.cpp` prints the first 60 airborne
ticks. The closed form for the real-space height is the usual

```
h(t) = v0 t + 0.5 G t²     with v0 = -sqrt(2 G H)
```

and the pixel height is `h(t) * 101 / 3.5`. Apex at `t = -v0 / G` must be
`-3.5 m` in real space and `-101 px` on screen, which is the assertion the
example suite uses to lock these constants.
