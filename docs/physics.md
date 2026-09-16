# Physics

All motion goes through one function, `Inertia::move` in `MaoLiAo/inertia.cpp`:

```cpp
X = v * t + 0.5 * a * t * t;
v = v + a * t;
return X;
```

`t` is always `TIME = 0.01` s (100 Hz). The function updates `v` in place and returns displacement in an internal “meter” unit. Role then scales meters to pixels with

```
pixels = meters * UNREAL_HEIGHT / REAL_HEIGHT
       = meters * (3 * 32 + 5) / 3.5
       = meters * 101 / 3.5
       ≈ meters * 28.857
```

`REAL_HEIGHT = 3.5` is the authored jump height in meters. `UNREAL_HEIGHT = 101` is that same hop in pixels. The ratio is the only unit conversion in the engine.

The portable clone is `examples/lib/kinematics.hpp`. `examples/inertia_sim` prints a jump table and checks that a standing hop peaks near 101 px.

## Constants (`define.h`)

| Macro | Value | Role |
| --- | --- | --- |
| `TIME` | 0.01 | Integration step and frame cap |
| `G` | 30.0 | Gravity, **positive downward in meters** |
| `V_MAX` | 8.0 | Horizontal speed cap, meters / s |
| `A_ROLE` | 20.0 | Horizontal run acceleration, meters / s² |
| `T1` | 0.5 | Time to `V_MAX` on high-friction ice (tile id 6) |
| `T2` | 1.2 | Time to `V_MAX` on grass / snow / clouds |
| `T3` | 1.5 | Time to `V_MAX` on everything else |
| `REAL_HEIGHT` | 3.5 | Jump apex in meters |
| `UNREAL_HEIGHT` | 101 | Jump apex in pixels |
| `STEP` | 10 | Walk-cycle: which hero frame to blit |
| `ENEMY_STEP` | 1 | Enemy walk, world pixels per allowed tick |
| `K_MAP_BG` | 5 | Parallax: background moves at 1/5 map speed |

Friction coefficients are **not** stored as raw `u` in the level data. `Scene::createMap` derives them:

```
u = (V_MAX / T_surface) / G
```

so `a_friction = G * u` would be `V_MAX / T_surface` — the deceleration that stops a max-speed runner in `T*` seconds if it were applied once. The hero applies it with an extra factor of two (see below).

| Tile id | Surfaces | `T*` | `u = (V_MAX/T*)/G` |
| --- | --- | ---: | ---: |
| 1, 3, 4, 5 | Grass, dirt, snow | T2 = 1.2 | 0.2222 |
| 6 | Ice / pipe-top (world 1) | T1 = 0.5 | 0.5333 |
| 2 | Clouds | T2 = 1.2 | 0.2222 |
| other solid | Pipes, defaults | T3 = 1.5 | 0.1778 |

World 3 overrides the first group to `T1` (grippy) so the Flappy-style pipes do not feel icy.

## Jump

A jump is allowed when `isFly == false` and the stage is not in the auto-walk ending — **except world 3**, which allows a new hop every frame that `W`/`K` is held (`无限跳`).

Initial vertical speed:

```
vY0 = -sqrt(2 * G * REAL_HEIGHT) = -sqrt(210) ≈ -14.4914 m/s
```

That is the schoolbook `v = sqrt(2gh)` for a 3.5 m hop under 30 m/s², pointed **up** (negative).

Each airborne frame:

```cpp
yy = yy - (-Inertia::move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT);
```

which is `yy += move(...) * scale`. Because screen-y grows downward, a negative displacement in meters raises the sprite.

After moving, `hitMap(x, y+1)` is tested:

- If a tile is hit and `vY > 0` (falling), `isFly` clears and `y` snaps to a 32-pixel row through `(y + HEIGHT/2) / HEIGHT * HEIGHT`.
- If falling and `hitEnemy`, the enemy is stomped (+5 score, explosion, enemy cleared).
- If `y > YSIZE` (384), the hero dies (fell off the world).

Grounded frames that lose their floor tile set `isFly` again, so walking off a ledge drops you.

World 3 also treats most solid tiles as instant death unless `isShoot` is already true (the “star / weapon” pickup). Clouds (id 2) are the exception.

## Horizontal run

`A` subtracts `A_ROLE` from acceleration, `D` adds it. The ending rail (`Hero.ending`) forces rightward `A_ROLE` so the cat walks off the right edge of the window and `passed` becomes true.

Friction is applied only when `vX * a <= 0` and `vX != 0` — braking or coasting, not when you accelerate in the direction you already move. Air has no friction (`map == NULL` ⇒ `a1 = 0`).

The sign of friction uses a compact trick from the original source (`+3-3调节惯性`):

```
k = (vX < 0) ? (1 + 3) wait no:
    Xabs = |vX|
    k = (vX < 0) ? (Xabs / vX + 3)   // -1 + 3 = +2
                  : (Xabs / vX - 3)   // +1 - 3 = -2
a1 = k * G * map->u                  // ±2 G u
```

So the effective brake is **twice** `G*u`. On grass that is `2 * 8 / 1.2 ≈ 13.33 m/s²`.

After the step, if `vX` flipped sign it is zeroed (no oscillation through the origin). Speed is then clamped to `V_MAX`.

Running into a tile (`hitMap` with `world` forced to `1`, so world-3’s touch-death rule does **not** apply to this particular test) cancels the horizontal step and zeroes `vX`.

## Camera rail and ending

The sprite is clamped to `[XLEFT, XRIGHT] = [0, 192]` while `ending` is false. Overflow on the right is subtracted from `x0`, which is the camera.

`Scene::isEnding(distance)` uses world-pixel distance `-x0 + x`:

| World | Distance to auto-walk | Pixels (`* 32`) |
| ---: | ---: | ---: |
| 1 | 94 tiles | 3008 |
| 2 | 104 tiles | 3328 |
| 3 | 94 tiles | 3008 |

Once `ending` is set, rightward acceleration is forced, the camera rail is released, and crossing `x > XSIZE` (512) sets `passed`.

## Parallax

`Scene::action` sets `xMap = (int)hero.x0`. When the hero is on the right rail, moving right, and `xMap` actually changed, background `xBg` decreases by

```
|vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG
```

`show()` wraps `xBg` over the bitmap width and stacks a second copy so the sky loops. Vertical sky pick is `yBg = -(world - 1) * YSIZE` into the 4-screen-tall `mapsky.bmp`.

## Bullets

After the weapon pickup:

- `J` spawns a bullet at the hero, same facing.
- First tap fires immediately. Held `J` refires every `TIME_INTERVAL_BULLET = 0.2` s.
- Flight: `x += 4 * turn` pixels per **shown** frame (`LEHGTH_INTERVAL_BULLET`, original spelling).
- Expire on enemy, solid tile, `x > MAX_DISTANCE` (480), or leaving the window.
- Enemy hit: +5 score, explosion, both bullet and enemy cleared.

`bullteFlying` is invoked from `Role::show`, not `action`, so a paused mental model of “simulate then draw” is slightly wrong: bullets advance during blit.

## Enemies

Enemies step `ENEMY_STEP` (1 px) when `(int)(enemy_iframe * 100) % 2 == 0`. They reverse if the body overlaps a tile **or** the tile under their leading foot is empty. `turn == 0` means dead / unused.

Side contact with `vY <= 0` kills the hero. Downward contact stomps.

## Worked jump (first 20 ms)

`vY0 = -14.4914`

| t (s) | vY after step | Δmeters | Δpixels | y (px), start 96 |
| ---: | ---: | ---: | ---: | ---: |
| 0.01 | -14.1914 | -0.1434 | -4.14 | 91.86 |
| 0.02 | -13.8914 | -0.1404 | -4.05 | 87.81 |

Apex is near `t = vY0 / G ≈ 0.483 s`, 48 frames, ~101 px up from the launch row — that is what `inertia_sim` asserts.
