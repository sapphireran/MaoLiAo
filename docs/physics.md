# Physics

All motion in MaoLiAo is the same kinematic step, then a scale from
"meters" to pixels. The step lives in `Inertia::move`:

```cpp
double Inertia::move(double& v, double t, double a)
{
    double X = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return X;
}
```

`t` is always `TIME` (`0.01`). After a vertical or horizontal step the
source multiplies the returned displacement by

```
UNREAL_HEIGHT / REAL_HEIGHT = (3 * 32 + 5) / 3.5 = 101 / 3.5 ≈ 28.857 px/m
```

`REAL_HEIGHT` is the comment's "feels like 3.5 meters". `UNREAL_HEIGHT`
is the pixel apex the author wanted for a full jump.

The portable copy of this function is `examples/include/maoliao_core.hpp`
(`maoliao::inertia_move`). `examples/inertia_demo` and
`examples/jump_table` print the same numbers this page derives.

## Constants (`define.h`)

| Symbol | Value | Role |
| --- | --- | --- |
| `TIME` | 0.01 s | integration step |
| `G` | 30.0 | gravity (m/s² in the "real" unit) |
| `REAL_HEIGHT` | 3.5 | designed jump height, meters |
| `UNREAL_HEIGHT` | 101 | designed jump height, pixels |
| `V_MAX` | 8.0 | max run speed, m/s |
| `A_ROLE` | 20.0 | held-key horizontal accel, m/s² |
| `T1` | 0.5 s | time to `V_MAX` on high-friction tiles |
| `T2` | 1.2 s | medium friction |
| `T3` | 1.5 s | low friction |
| `XLEFT` | 0 | hero screen-x minimum |
| `XRIGHT` | 192 | hero screen-x maximum (`6 * 32`) |
| `K_MAP_BG` | 5 | sky scroll divisor |
| `STEP` | 10 px | walk-cycle period |
| `ENEMY_STEP` | 1 px | enemy crawl |

Friction coefficients stored on each `Map` are not raw µ from a textbook.
They are derived so that `a = G * u` equals `V_MAX / Tn`:

```
u = (V_MAX / Tn) / G
```

| Tile group | `Tn` | `u` | `G * u` (m/s²) |
| --- | --- | --- | --- |
| grass / dirt / snow (`id` 1, 3, 4, 5) | T2 = 1.2 | 8/1.2/30 ≈ 0.2222 | 6.667 |
| ice-like `id` 6 | T1 = 0.5 | 8/0.5/30 ≈ 0.5333 | 16.0 |
| clouds `id` 2 | T2 = 1.2 | same as grass | 6.667 |
| everything else | T3 = 1.5 | 8/1.5/30 ≈ 0.1778 | 5.333 |

World 3 forces `id` 1 and 3–6 onto T1 (the sticky set).

## Jump

Impulse, from `Role::action`:

```cpp
myHero.vY = -sqrt(2 * G * REAL_HEIGHT);   // -√210 ≈ -14.4914 m/s
myHero.isFly = true;
```

That is the closed-form takeoff speed for a jump that peaks at
`REAL_HEIGHT` under constant `G`, ignoring the later pixel scale.

Each airborne frame:

```cpp
yy = yy - (-Inertia::move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT);
y  = (int)yy;
```

The double negation means "add the kinematic Δy, scaled to pixels".
`vY` starts negative (up on a y-down screen) and increases by `G * TIME`
= 0.3 every step.

Apex in the real unit: `v² = v0² + 2 G s` with `v = 0` gives
`s = REAL_HEIGHT` exactly, by construction. In pixels the same integral
is `101` px if you start at rest vertically and do not hit a ceiling.

`examples/jump_table` integrates the loop until `vY` crosses zero and
prints the pixel apex. It should report **101 px** (within one
integration step of rounding).

World 3 skips the `isFly == false` gate, so another impulse can be
applied every frame the jump key is sampled. That is the Flappy feel.

Landing: `hitMap(x, y+1)` while `vY > 0` clears `isFly`, zeroes `vY`,
and snaps `y` to the tile grid:

```cpp
y = (y + HEIGHT/2) / HEIGHT * HEIGHT;
```

Falling below `YSIZE` (384) sets `died`.

Stomp: if `vY > 0` and `hitEnemy` succeeds, the enemy slot is zeroed,
score += 5, and a bomb is spawned. A side hit (`vY <= 0`) kills the hero.

## Horizontal motion

Held `A` subtracts `A_ROLE` from `a`. Held `D`, or the ending auto-walk,
adds `A_ROLE`. Both can be down at once (`GetCommand` is a bit mask);
they cancel.

Friction is applied only when `vX * a <= 0` and `vX != 0` (coasting or
reversing) **and** a supporting tile exists:

```cpp
// vX > 0  =>  k =  1 - 3 = -2
// vX < 0  =>  k = -1 + 3 =  2
a1 = k * G * map->u;
```

So friction acceleration has magnitude `2 * G * u`, opposite the current
velocity. On grass that is about `13.33` m/s² — stronger than `A_ROLE`.
Letting go of `D` therefore stops the hero faster than a naive
`a = G*u` model.

After `Inertia::move`, if `vX` flipped sign during the step it is forced
to 0. Speed is then clamped to `V_MAX`.

Walls: `hitMap` with `world` forced to `1` (so world-3's "touch and die"
rule does not run on this probe). On a hit, `x` is rewound by the step
and `vX` is zeroed. If `x > XRIGHT` it is also clamped to `XRIGHT`.

## Camera

The hero is a sprite in **screen space**. The world slides underneath
via `hero.x0` (a double, stored negative as the camera advances).

```
if x < XLEFT:          pin x, zero vX
if x > XRIGHT
    and not ending:    x0 -= (x - XRIGHT);  x = XRIGHT
if ending and x > XSIZE:  passed = true
```

`Scene::action` sets `xMap = (int)hero.x0`. Tiles are drawn at
`xMap + tile.x * 32`. Enemies and bombs add `x0` the same way when
blitted, so they stay glued to the map.

World-space hero x used for "is the flag near?" is

```
-(int)hero.x0 + hero.x
```

`Scene::isEnding` compares that distance to `94 * 32` (worlds 1 and 3)
or `104 * 32` (world 2). Once `ending` is true, `CMD_RIGHT` is forced
on and jump is disabled (except the world-3 air-jump rule still sees
`CMD_UP`).

Sky parallax only runs while the hero is stuck on the right pin with
`vX > 0` and `xMap` actually changed this frame. The sky is two copies
of `mapsky.bmp` (512 px wide) with wrap when `xBg <= -width`. Vertical
slice is `-(world-1) * 384`, so each world uses a different 512×384
band of the 512×1536 sheet.

## Collision

`Role::isHit` tests the hero's four inset vertices against an axis-aligned
rectangle. A vertex counts if

```
p.x >= rect.left  && p.y >= rect.top
&& p.x <= rect.right && p.y <= rect.bottom
```

Hero inset is 1 px on each side (`x+1 .. x+WIDTH-1`). That is **not** a
full SAT / swept AABB. Fast objects can tunnel; bullets move 4 px per
frame (`LEHGTH_INTERVAL_BULLET`) so they usually connect.

Tiles `id` 8 and 10 (pipe mouths) use a 2×2 tile box
(`xAmount * 2 * WIDTH`). Other tiles use `xAmount * WIDTH` by
`yAmount * HEIGHT`.

World 3: any `hitMap` whose `id != 2` sets `died` unless `isShoot` is
already true. The weapon pickup is therefore also the invincibility star
for that stage. The horizontal wall probe passes `world = 1` on purpose
so walking into a pipe does not use this death rule; the vertical /
generic probes pass the real `world`.

Coins and food use the same four-vertex test. Food hit boxes are
`1.5 * 32 + 4` by `0.8 * 32` because the sprite is not a full tile.

## Bullets

Armed only after `hitFood`. First tap fires immediately. Held `J` then
waits `TIME_INTERVAL_BULLET` (0.2 s) between shots. On release,
`shootTimeInterval` is set to `0.01` rather than `0`, so the next tap
still takes the "button was down" branch for one interval — a quirk
noted in [known-quirks.md](known-quirks.md).

Flight (inside `show`):

- Off-screen (`x > 512` or `x < -32`): despawn.
- `hitEnemy` or `hitMap(..., world=1)` or `x > MAX_DISTANCE` (480):
  explode, maybe +5 score, `setBomb`.
- Else `x += 4 * turn`.

`MAX_DISTANCE` is a **screen-x** cap, not a world-x cap. From the right
pin (x = 192) a right-facing shot travels 480 − 192 = 288 px before it
self-destructs.

## Enemies

Each slot is `{world-x, world-y, turn}`. `turn` is +1 or −1. Zero means
dead / unused.

They step `ENEMY_STEP` pixels when `(int)(enemy_iframe * 100) % 2 == 0`.
`enemy_iframe` advances by `TIME * 5` = 0.05 per frame, so the integer
part changes slowly; the `% 2` test on `iframe * 100` is a crude
decimator, not a clock.

Turn-around: if the enemy overlaps a tile **or** the tile under the
forward foot is empty, flip `turn`. The probes reuse `hitMap` with the
hero's `x0` baked into the x that is passed in — the same function
subtracts `x0` again internally. That double application is why enemy
walk AI is coupled to camera position. See [known-quirks.md](known-quirks.md).

## Worked numbers

One jump from rest, no ceiling, `TIME = 0.01`, `G = 30`:

| Frame | `vY` after step (m/s) | pixel Δy this step | notes |
| --- | --- | --- | --- |
| 0 | −14.4914 | — | impulse |
| 1 | −14.1914 | ≈ −4.15 | rising |
| … | … | … | |
| ~49 | ~0 | ~0 | apex ≈ 101 px above start |
| later | positive | downward | |

Time to apex: `v0 / G` = 14.4914 / 30 ≈ **0.483 s**. The discrete
`TIME = 0.01` loop crosses `vY >= 0` on **frame 49** (0.49 s) with
apex **100.996 px** (`examples/inertia_demo`).

Time to `V_MAX` on grass if you hold `D` from rest, ignoring friction
while `vX * a > 0` (friction is off while accelerating in the same
direction): `V_MAX / A_ROLE` = 8 / 20 = **0.40 s** = 40 frames.

`examples/inertia_demo` prints both timelines.
