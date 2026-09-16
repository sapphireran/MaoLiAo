# Physics

All motion in 猫里奥 goes through one function:

```cpp
// MaoLiAo/inertia.cpp
double Inertia::move(double& v, double t, double a)
{
    double X = v * t + 1.0 / 2.0 * a * t * t;
    v = v + a * t;
    return X;
}
```

That is the standard kinematic step. `v` is updated **in place**. `Role::action` then scales the returned displacement from “meters” into pixels:

```
pixels = Inertia::move(...) * UNREAL_HEIGHT / REAL_HEIGHT
```

with `UNREAL_HEIGHT / REAL_HEIGHT = 101 / 3.5 ≈ 28.857`.

The portable copy is `examples/maoliao_lib/inertia.py` (`step` / `jump_speed` / `pixels_per_meter`).

## Constants

From `define.h`:

| Symbol | Value | Meaning |
| --- | --- | --- |
| `TIME` | 0.01 s | Fixed step |
| `G` | 30.0 | Gravity magnitude (down is **positive** for `vY`) |
| `REAL_HEIGHT` | 3.5 | Designed jump height in fake meters |
| `UNREAL_HEIGHT` | 101 | Designed jump height in pixels |
| `V_MAX` | 8.0 | Horizontal speed cap, pre-scale |
| `A_ROLE` | 20.0 | Run acceleration, pre-scale |
| `T1` / `T2` / `T3` | 0.5 / 1.2 / 1.5 | Seconds to reach `V_MAX` against friction (high / mid / low) |
| `XLEFT` / `XRIGHT` | 0 / 192 | Hero screen-space clamp |

`Role` stores both integer blit coordinates (`x`, `y`) and double integration coordinates (`xx`, `yy`). The integrator writes the doubles; the ints are truncated copies used for collision and `putimage`.

## Jump

A jump is allowed when:

- worlds 1 and 2: `CMD_UP` and `isFly == false` and `ending == false`
- world 3: `CMD_UP` alone (infinite mid-air jumps)

The launch speed is the energy identity `v² = 2 g h`:

```
vY = -sqrt(2 * G * REAL_HEIGHT) = -sqrt(210) ≈ -14.4913767
```

Negative `vY` is up. While `isFly` is true, each tick does:

```
yy -= -Inertia::move(vY, TIME, G) * (UNREAL_HEIGHT / REAL_HEIGHT)
```

`move` is called with `+G`, so after the apex `vY` becomes positive and the hero falls. The extra minus signs exist because `move` returns a signed displacement in the same direction as `v`.

A probe `hitMap(x, y + 1)` decides landing. If that probe hits and `vY > 0`, `isFly` is cleared and `y` is snapped to the tile grid through `(y + HEIGHT/2) / HEIGHT * HEIGHT`. If the probe misses while the hero is supposed to be on the ground, `isFly` is set and gravity takes over — this is how walking off a ledge works.

Falling past `YSIZE` (384) sets `died`. Stomping is the same falling path: `vY > 0` and `hitEnemy` success awards +5, writes a bomb, and clears the enemy.

World 3's mid-air jump re-applies the same launch speed every time `CMD_UP` is sampled, which is why that stage feels like Flappy Bird instead of Mario.

## Horizontal run

`CMD_LEFT` subtracts `A_ROLE` from the working acceleration `a`. `CMD_RIGHT`, or the auto-walk flag `ending`, adds `A_ROLE`. `ending` is raised by `Scene::isEnding` once the hero's **world** x (`-x0 + x`) crosses a per-world threshold (see [levels.md](levels.md)). After that the cat is driven right until `x > XSIZE`, which sets `passed`.

Displacement:

```
H = Inertia::move(vX, TIME, a + a1) * UNREAL_HEIGHT / REAL_HEIGHT
xx += H
```

If `vX` flipped sign during the step, it is zeroed (no pendulum through the origin). If `|vX|` exceeds `V_MAX`, it is clamped back to `±V_MAX`.

## Friction

Friction acceleration `a1` is applied only when the hero is on a tile (`map != NULL`), `vX != 0`, and `vX` and the run acceleration `a` point in opposite directions **or** the player is not adding run acceleration (`vX * a <= 0`).

The source computes a sign helper `k`:

```
Xabs = |vX|
k = Xabs / vX - 3    if vX > 0     →  1 - 3 = -2
k = Xabs / vX + 3    if vX < 0     → -1 + 3 = +2
a1 = k * G * map->u
```

So `|a1| = 2 G u`, opposite the current velocity.

Each solid tile stores a friction coefficient `u` derived from the “time to `V_MAX`” macros:

```
u = (V_MAX / T) / G
```

| Tile IDs (worlds 1–2) | T | `u` | Feel |
| --- | --- | --- | --- |
| 6 | `T1` = 0.5 | `8 / 0.5 / 30 = 0.5333…` | High friction |
| 1, 2, 3, 4, 5 | `T2` = 1.2 | `8 / 1.2 / 30 = 0.2222…` | Mid friction |
| everything else | `T3` = 1.5 | `8 / 1.5 / 30 = 0.1777…` | Low friction |

World 3 assigns `T1` to IDs 1–6 and `T2` to ID 2 (clouds). Decorative IDs still get a `u` even though IDs ≥ 11 are not solid.

In air (`map == NULL`) `a1` is forced to 0 — no air drag.

## Camera lock

The hero is not free to cross the whole framebuffer. After integration:

- `x < XLEFT` (0): clamp, zero `vX`.
- `x > XRIGHT` (192) and not `ending`: slide the world origin instead of the sprite:

```
x0 -= (x - XRIGHT)
x = XRIGHT
```

`Hero::x0` is the camera origin (negative as the world scrolls right). `Scene::action` copies it into `xMap` and eases the sky at `1 / K_MAP_BG` (`K_MAP_BG = 5`) of the hero's horizontal speed:

```
bgStep = |vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / 5
```

The sky only steps when the hero is pinned on the right edge and actually moving right. Walking left does not rewind the sky, which is why the parallax is one-way.

Enemies, bombs, coins, and tiles are drawn at `xMap + worldX` (or `x0 + worldX`), so they slide under the locked hero.

## Worked jump (rest, no ceiling)

Launch `vY = -√210`. After `n` ticks of gravity `G` and no collision:

```
v(n) = v0 + G * n * TIME
Δmeters = v0 * (n TIME) + ½ G (n TIME)²
Δpixels = Δmeters * 101 / 3.5
```

Apex is at `t = √210 / 30 ≈ 0.483 s` (about 48 ticks). Peak pixel rise is `REAL_HEIGHT * 101 / 3.5 = 101` pixels — that is why `UNREAL_HEIGHT` exists. `examples/physics/demo_jump.py` prints the tick-by-tick table and asserts the peak.

## Worked run-up

From rest, holding right, on a `T2` tile, with friction only opposing when `a` and `v` disagree (so while holding right, `a` and `v` share a sign and `a1 = 0`):

```
vX(n) = min(V_MAX, A_ROLE * n * TIME) = min(8, 0.2 n)
```

`V_MAX` is reached in `8 / 20 = 0.4 s` (40 ticks), faster than `T2` because `T2` describes friction-limited coast, not powered run. Releasing the key then applies `a1 = -2 G u ≈ -13.333` and the cat slides to a stop. `examples/physics/demo_run_stop.py` walks both phases.
