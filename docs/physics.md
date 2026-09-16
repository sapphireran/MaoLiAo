# Physics

All motion goes through `Inertia::move` in `MaoLiAo/inertia.cpp`:

```
X = v * t + 0.5 * a * t * t
v = v + a * t
return X
```

`t` is always `TIME` = **0.01 s** (the frame dt). The function takes `v`
by reference and writes the new velocity back.

The rest of this page is the constants from `define.h` and how `Role`
feeds them into that integrator. The portable copy is
`examples/common/inertia.hpp`.

## Constants

| Macro | Value | Role |
| --- | --- | --- |
| `TIME` | 0.01 | Frame dt (s) |
| `G` | 30.0 | Gravity (m/s² in the "real" model) |
| `REAL_HEIGHT` | 3.5 | Intended jump height in metres |
| `UNREAL_HEIGHT` | 3×32+5 = 101 | Jump height in pixels |
| `V_MAX` | 8.0 | Horizontal speed cap (m/s) |
| `A_ROLE` | 20.0 | Run acceleration (m/s²) |
| `T1` / `T2` / `T3` | 0.5 / 1.2 / 1.5 | Time to `V_MAX` on high / mid / low friction |
| `STEP` | 10 | Walk-cycle pixel modulus |
| `ENEMY_STEP` | 1 | Enemy pixels per patrol tick |

Pixel scale:

```
px_per_metre = UNREAL_HEIGHT / REAL_HEIGHT
             = 101 / 3.5
             ≈ 28.857 px/m
```

Every displacement from `Inertia::move` is multiplied by that scale
before it is added to `Hero.xx` / `Hero.yy`.

## Jump

A jump is allowed when `!isFly && !ending`, **or always in world 3**.

```
vY = -sqrt(2 * G * REAL_HEIGHT)
   = -sqrt(2 * 30 * 3.5)
   = -sqrt(210)
   ≈ -14.4914 m/s
```

While `isFly`:

```
dy_m  = Inertia::move(vY, TIME, +G)     // gravity is +G in the metre model
yy   -= -dy_m * px_per_metre            // yy += dy_m * scale  (role.cpp)
```

Takeoff `vY` is negative, so the first `dy_m` is negative and **EasyX Y
decreases** (up the screen). After each step the code probes
`hitMap(x, y+1)`. The extra pixel is required because `isHit` insets
the sprite by 1 px: a cat whose feet sit exactly on a tile top does
not overlap that tile. `examples/03_collision_aabb` shows both cases.
A hit with `vY > 0` lands:
`isFly = false`, `vY = 0`, Y snapped to the tile grid
`(y + HEIGHT/2) / HEIGHT * HEIGHT`.

Leaving a platform (`!isFly` but `hitMap(x, y+1)` is null) sets
`isFly = true` with the current `vY` (usually 0), so the hero walks off
edges.

Falling past `YSIZE` (384) sets `died`.

While falling (`vY > 0`), overlapping an enemy **stomps** it: +5 score,
bomb sprite, enemy cleared. Walking into an enemy with `vY <= 0` kills
the hero.

## Horizontal run

Left subtracts `A_ROLE` from `a`. Right (or `ending`) adds `A_ROLE`.
`ending` is the auto-walk that starts once `isEnding(worldX)` is true;
the hero is forced right until `x > XSIZE`, which sets `passed`.

Friction only applies when velocity and input acceleration have opposite
signs (or input is 0 while `vX != 0`):

```
k  =  +3  when vX < 0
k  =  -3  when vX > 0          // see role.cpp; uses Xabs/vX ± 3
a1 =  k * G * map->u           // 0 if airborne (map == NULL)
```

`map->u` is chosen when the level is built:

```
u = (V_MAX / T*) / G
```

| Surface | Tile ids | T* | μ |
| --- | --- | --- | --- |
| High friction (ice / id 6 world 1) | 6 (world 1), 1/3/4/5/6 (world 3) | T1 = 0.5 | (8/0.5)/30 = **0.5333** |
| Mid (grass, cloud, snow) | 1, 2, 3, 4, 5 | T2 = 1.2 | (8/1.2)/30 = **0.2222** |
| Low (default / scenery) | everything else | T3 = 1.5 | (8/1.5)/30 ≈ **0.1778** |

After integration, if `vX` **changed sign** this step it is zeroed
(no rebound). Speed is then clamped to `V_MAX`.

If the next AABB overlaps a solid, X is rewound by `H` (or clamped to
`XRIGHT` if past the camera window) and `vX` is zeroed.

## Camera window

```
XLEFT  = 0
XRIGHT = 6 * 32 = 192
```

- `x < 0`        → pin to 0, kill `vX`
- `x > 192` and not ending → `x0 -= (x - 192)`, pin `x` to 192
- `x > 512`      → `passed = true`

`Scene::action` copies `x0` into `xMap`. Parallax:

```
bgStep = |vX| * TIME * px_per_metre / K_MAP_BG
K_MAP_BG = 5
```

`xBg` decreases only when the hero is glued to `XRIGHT`, `vX > 0`, and
`xMap` actually changed — sky moves at 1/5 of the ground scroll.

World finish distances (`isEnding`):

| World | Distance |
| --- | --- |
| 1 | `> 94 * 32` = 3008 px |
| 2 | `> 104 * 32` = 3328 px |
| 3 | `> 94 * 32` = 3008 px |

## Bullets

Enabled after `hitFood` (`isShoot = true`). Spawn at
`x + WIDTH/2` facing right, or `x - WIDTH` facing left.

Each draw tick: `x += LEHGTH_INTERVAL_BULLET * turn` with
`LEHGTH_INTERVAL_BULLET = 4`. Expire if:

- off the window (`x > 512` or `x < -32`)
- `x > MAX_DISTANCE` (480)
- `hitEnemy` or `hitMap` (world forced to `1` in that call)

Hold-to-fire uses `TIME_INTERVAL_BULLET = 0.2` s between shots.

## Enemies

Patrol every other tick of `enemy_iframe * 100`. Each tick:
`x += turn * ENEMY_STEP`. Reverse `turn` if the body overlaps a tile
**or** the tile under the leading foot is empty (ledge).

## Worked numbers

A standing jump, no horizontal input, lands in **N** frames when the
integrated pixel height returns to 0. The example
`examples/01_inertia_jump` prints the full table and checks:

- takeoff `vY ≈ -14.4914`
- apex near `UNREAL_HEIGHT` (101 px) — the discrete step undershoots
  slightly because gravity is applied from frame 0
- hang time ≈ `2 * |vY| / G` = `2 * 14.4914 / 30` ≈ **0.966 s** ≈ 97 frames

A full-speed stop on grass (`μ = 0.2222`, `k = -3`, so
`a1 = -3 * 30 * 0.2222 = -20`) from `vX = 8` with `a = 0`:

```
stop time  ≈ 8 / 20 = 0.40 s  →  40 frames
```

Ice (`μ = 0.5333`) gives `|a1| = 48` and stops in ≈ 17 frames.
`examples/02_friction_run` checks both.
