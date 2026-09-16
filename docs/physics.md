# Physics

All motion goes through `Inertia::move` in `inertia.cpp`. The rest of the feel comes from the macros in `define.h` and the friction / jump / camera rules in `Role::action`.

The Python and C++ examples under `examples/physics/` implement the same formulas so you can inspect trajectories without EasyX.

## Integrator

```
X = v * t + 0.5 * a * t * t
v = v + a * t
return X
```

`t` is always `TIME = 0.01`. The function mutates `v` and returns the displacement for this step. There is no velocity Verlet, no drag term, and no sub-stepping.

After the call, `Role` scales the displacement from “meters” into pixels:

```
pixels = X * UNREAL_HEIGHT / REAL_HEIGHT
```

where `REAL_HEIGHT = 3.5` (the authored jump height in fictional meters) and `UNREAL_HEIGHT = 3 * 32 + 5 = 101` (pixels). The scale factor is `101 / 3.5 ≈ 28.857`.

## Constants (from `define.h`)

| Macro | Value | Use |
| --- | --- | --- |
| `TIME` | `0.01` | Frame dt |
| `G` | `30.0` | Gravity (down is +Y after the sign flip in `action`) |
| `V_MAX` | `8.0` | Horizontal speed cap (pre-scale units) |
| `A_ROLE` | `20.0` | Held-key horizontal acceleration |
| `T1` / `T2` / `T3` | `0.5` / `1.2` / `1.5` | Time to `V_MAX` on high / mid / low friction |
| `REAL_HEIGHT` | `3.5` | Jump height used in `v = -√(2 g h)` |
| `UNREAL_HEIGHT` | `101` | Pixel jump height the scale aims at |
| `XLEFT` / `XRIGHT` | `0` / `192` | On-screen hero clamp before ending |
| `K_MAP_BG` | `5` | Sky moves 1 px per 5 px of map |

`LIFE` and `F` are defined with a trailing semicolon (`#define LIFE 5;`). That is harmless at the two call sites (`int life = LIFE;` and `scenery_iframe += F;`) but is still a macro footgun. See [known-issues.md](known-issues.md).

## Jump

On the frame where `CMD_UP` is held and the hero is grounded (`isFly == false`) and not in the ending walk:

```
vY = -sqrt(2 * G * REAL_HEIGHT)   ≈ -14.4914
isFly = true
```

World 3 skips the grounded check (`|| world == 3`), so you can flap every frame.

While `isFly`:

```
yy -= -Inertia::move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT
```

The double negation is because `move` with `+G` increases `vY` from a negative launch toward positive (falling). After the first apex, `vY > 0` means descending. A map hit while descending clears `isFly` and snaps `y` to a 32 px grid through `(y + HEIGHT/2) / HEIGHT * HEIGHT`. A map hit while still rising (ceiling) zeroes `vY` but leaves `isFly` true, so the next step starts falling.

Walking off a ledge sets `isFly` when `hitMap(x, y+1)` misses.

Falling past `YSIZE` (384) is a death.

Time to apex from rest jump: `t = √(2h/g) = √(7/30) ≈ 0.483 s` ≈ 48 frames.

## Horizontal motion and friction

Held `A` / `D` add `-A_ROLE` or `+A_ROLE` to acceleration `a`. The ending flag forces `+A_ROLE` so the cat walks off-screen by itself.

Friction acceleration `a1` is applied only when velocity and input oppose or input is zero (`vX * a <= 0` and `vX != 0`):

```
if vX < 0:  k = |vX| / vX + 3   # k = 2
else:       k = |vX| / vX - 3   # k = -2
a1 = k * G * map.u              # 0 if airborne (map == NULL)
```

`k` is therefore a signed “twice gravity” scale that always points against `vX`. Surface `u` is baked when the map is created:

```
u = (V_MAX / T) / G
```

| Tile ids | T used | `u` | Feel |
| --- | --- | --- | --- |
| 6 (and world-3 solids 1/3/4/5/6) | `T1 = 0.5` | `8/0.5/30 ≈ 0.5333` | High friction, short slide |
| 1, 2, 3, 4, 5 (worlds 1–2) | `T2 = 1.2` | `8/1.2/30 ≈ 0.2222` | Default grass / cloud / snow |
| everything else | `T3 = 1.5` | `8/1.5/30 ≈ 0.1778` | Low friction |

After the step, a sign change in `vX` snaps speed to 0 (no ping-pong). `|vX|` is then clamped to `V_MAX`.

A horizontal `hitMap` (using `world=1` collision rules, so world 3 does not instantly kill on this query) rewinds `x` by the step, zeroes `vX`, and zeroes both accelerations.

## Camera and parallax

While `x` is inside `[XLEFT, XRIGHT]` the camera is still. When `x` would pass `XRIGHT` and the stage is not ending:

```
x0 -= (x - XRIGHT)
x = XRIGHT
```

`Scene::action` copies `xMap = (int)x0` and, if the hero is pinned to the right edge and still moving right, slides the sky by

```
bgStep = |vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG
```

When `xBg <= -skyWidth` it wraps to 0 so the sky loops.

## Ending walk

`Scene::isEnding(distance)` uses world X `-x0 + x`:

| World | Distance in tiles | Pixels |
| --- | --- | --- |
| 1 | 94 | 3008 |
| 2 | 104 | 3328 |
| 3 | 94 | 3008 |

Once `ending` is set, left input is ignored, jump is blocked (except the world-3 flap clause), and right acceleration is forced. `passed` becomes true when on-screen `x > XSIZE` (512).

## Enemies and bullets (not Newtonian)

Enemies step `ENEMY_STEP = 1` px on even hundredths of `enemy_iframe`, reverse when `hitMap` at their feet misses or their body hits a wall. They do not accelerate.

Bullets move `LEHGTH_INTERVAL_BULLET = 4` px per drawn frame in `turn`’s direction (`+1` right, `-1` left). Hold-to-fire is gated by `TIME_INTERVAL_BULLET = 0.2` s after the first shot.

## Worked jump (first 5 frames)

Launch `vY = -√210`. Gravity `G = 30`, `dt = 0.01`, scale `s = 101/3.5`.

| Frame | vY before | Δ“meters” | Δpixels | vY after |
| --- | --- | --- | --- | --- |
| 1 | -14.4914 | -0.1434 | -4.14 | -14.1914 |
| 2 | -14.1914 | -0.1404 | -4.05 | -13.8914 |
| 3 | -13.8914 | -0.1374 | -3.97 | -13.5914 |
| 4 | -13.5914 | -0.1344 | -3.88 | -13.2914 |
| 5 | -13.2914 | -0.1314 | -3.79 | -12.9914 |

`examples/physics/jump_profile.py` prints the full ascent and the pixel peak (should land near 101 px of rise from the launch y).
