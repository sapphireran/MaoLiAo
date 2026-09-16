# Physics

All motion goes through `Inertia::move` in `inertia.cpp`:

```text
X = v * t + 0.5 * a * t * t
v = v + a * t
```

`t` is always `TIME` (0.01 s). The function takes `v` by reference and returns the displacement **in the “real meters” space**, not pixels.

## Pixel mapping

Comments in `define.h` treat a jump as `REAL_HEIGHT` = 3.5 “meters.” The matching pixel travel is `UNREAL_HEIGHT` = `3 * 32 + 5` = 101 px.

Every integrator result is scaled:

```text
pixels = meters * UNREAL_HEIGHT / REAL_HEIGHT
       = meters * 101 / 3.5
       ≈ meters * 28.857
```

That scale is applied to both the vertical jump integration and the horizontal step.

## Gravity and jump

On a grounded jump (`CMD_UP` and `isFly == false`, and not in the ending auto-run):

```text
vY = -sqrt(2 * G * REAL_HEIGHT)
G  = 30
```

So the initial upward speed is about **-14.49** (meters / time-unit). While `isFly` is true, each tick:

```text
yy -= -Inertia::move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT
y   = (int)yy
```

The double negative is just “add the downward-positive displacement after `move` updated `vY`.” After a jump, `vY` increases toward positive (falling). Contact with a solid tile from above sets `isFly = false`, zeros `vY`, and snaps `y` to a 32 px grid using the sprite center: `(y + HEIGHT/2) / HEIGHT * HEIGHT`.

World 3 is special: `CMD_UP` is accepted even in the air, so the player can flap.

Leaving a platform without jumping (`hitMap(x, y+1)` is null while grounded) sets `isFly = true` so the hero falls.

Falling past `YSIZE` (384) sets `died`.

## Horizontal acceleration

`A_ROLE` = 20. Left subtracts it, right (or the ending auto-run) adds it.

Friction only applies when velocity and desired acceleration have opposite signs (or you released the key so `a == 0` while still sliding):

```text
a1 = k * G * map->u
```

`k` is a sign-ish fudge: `Xabs / vX - 3` when moving right (−2) and `Xabs / vX + 3` when moving left (+2). Combined with `u` this is a tuned brake, not a textbook `μN/m`.

If there is no floor under the hero (`map == NULL`), `a1` is forced to 0 — air has no friction in this game.

After integration, a sign flip of `vX` is treated as “stopped” (`vX = 0`) so the hero does not jitter across zero.

## Speed cap

If `|vX| > V_MAX` (8), `vX` is rescaled to length 8. That cap is applied **after** wall resolution, so a frame can theoretically integrate above 8 and then clamp.

## Tile friction `u`

When `Scene::createMap` copies tiles, it writes:

| Tile ids | Formula | Feel |
| --- | --- | --- |
| 1, 3, 4, 5 (and id 2 in worlds 1–2) | `(V_MAX / T2) / G` = `(8 / 1.2) / 30` ≈ 0.222 | Medium |
| 6 (icy / low-grip strip used on world 1) | `(V_MAX / T1) / G` = `(8 / 0.5) / 30` ≈ 0.533 | High friction (stops quicker) |
| Default (pipes, scenery leftovers) | `(V_MAX / T3) / G` = `(8 / 1.5) / 30` ≈ 0.178 | Slippery |

World 3 forces ids 1 and 3–6 onto the `T1` (grippy) curve and keeps clouds (id 2) on `T2`.

`T1 < T2 < T3` are documented as “time to reach max speed” under high / medium / low friction. The stored `u` is ` (V_MAX / T) / G `, i.e. the μ that would produce that accel if `a = μG`.

## Camera coupling

Horizontal speed also drives sky parallax in `Scene::action`:

```text
bgStep = |vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG
```

The sky only slides when the hero is on the right rail, `vX > 0`, and `xMap` actually changed this frame. The bitmap is 512 px wide and loops when `xBg <= -width`.

## Ending auto-run

When world-space X exceeds the per-world threshold (`Scene::isEnding`), `hero.ending` becomes true. From then on:

- Jump is ignored (except the world-3 flap condition is still gated by `ending == false` on the first clause — world 3 can still flap because of the `|| world == 3` term).
- Left is ignored.
- Right acceleration is applied every frame (`KEY & CMD_RIGHT || ending`).

The hero then walks himself off the right of the 512 px window (`x > XSIZE`) and `passed` becomes true.

## Worked jump (one tick)

Portable numbers you can check in `examples/physics_kinematics`:

```text
v0      = -sqrt(2 * 30 * 3.5)     ≈ -14.4914
dt      = 0.01
a       = 30
dMeters = v0*dt + 0.5*30*dt^2     ≈ -0.1434
dPixels ≈ -0.1434 * 101 / 3.5     ≈ -4.14
v1      = v0 + 30*dt              ≈ -14.1914
```

Hang time to `vY = 0` is `14.4914 / 30` ≈ 0.483 s ≈ 48 ticks, about 101 px up, which matches `UNREAL_HEIGHT`.
