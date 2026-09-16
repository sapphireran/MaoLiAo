# Physics

All motion goes through one integrator in `Inertia::move`:

```text
X = v * t + 0.5 * a * t * t
v = v + a * t
return X
```

`t` is always `TIME = 0.01`. The function **mutates** `v` and returns the displacement for that
tick. Horizontal and vertical axes call it separately.

## Constants (`define.h`)

| Symbol | Value | Role |
| --- | --- | --- |
| `G` | 30.0 | Gravity (and the scale used to derive friction) |
| `REAL_HEIGHT` | 3.5 | Authoring jump height in “meters” |
| `UNREAL_HEIGHT` | 3 * 32 + 5 = 101 | Pixel jump height the player sees |
| `V_MAX` | 8.0 | Horizontal speed cap (physics units) |
| `A_ROLE` | 20.0 | Held-key acceleration |
| `T1` / `T2` / `T3` | 0.5 / 1.2 / 1.5 | Time-to-`V_MAX` used to derive µ (high / mid / low friction) |

Pixel displacement is physics displacement times `UNREAL_HEIGHT / REAL_HEIGHT` ≈ 28.857.

## Jump

On the ground (`isFly == false`) — or **always** in world 3 — `CMD_UP` sets:

```text
vY = -sqrt(2 * G * REAL_HEIGHT)   ≈ -14.4914
isFly = true
```

Each airborne tick:

```text
yy += Inertia::move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT
```

(`role.cpp` writes this as `yy - (-move(...))`.) Continuous kinematics give apex time
`|v0| / G ≈ 0.483 s` and height exactly 3.5 m. At 100 Hz the discrete sum is a few millimeters
short; the portable example records both.

Landing: a map hit with `vY > 0` clears flight, zeroes `vY`, and snaps `y` to the tile grid
through `(y + HEIGHT/2) / HEIGHT * HEIGHT`. Falling past `YSIZE` (384) is a death.

World 3 also kills on most solid tiles (anything except id 2) unless `isShoot` is true after the
weapon pickup.

## Walk and friction

Held `A` / `D` contribute `±A_ROLE`. If velocity and that acceleration have opposite signs (or the
player released the key so `a == 0` while `vX != 0`), a friction term is added **only when a floor
tile is under the cat**:

```text
k =  (vX < 0) ? ( |vX|/vX + 3 )   // = 2
   :            ( |vX|/vX - 3 )   // = -2
a1 = k * G * map->u
```

Tile friction is authored as:

```text
u = (V_MAX / T) / G
```

so `|a1| = 2 * V_MAX / T`. The extra factor of two is an old tuning quirk (the `±3` offset on
`k`). Grass / snow use `T2`, pipe-middle tiles use `T1`, everything else uses `T3`.

After the integrator, a sign change of `vX` is snapped to 0 (no oscillation through rest). Then
`|vX|` is clamped to `V_MAX`.

Wall hits (using `hitMap` with `world` forced to 1 so world-3 lethality does not trigger here)
rewind the horizontal step and zero `vX`.

## Camera coupling

If `x` would exceed `XRIGHT` (192) and the ending walk has not started, the overflow is subtracted
from `x0` instead. Scene parallax uses `|vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG` with
`K_MAP_BG = 5`, and only while the hero is pinned to the right edge and `x0` actually changed.

## Ending walk

`Scene::isEnding(worldX)` becomes true after 94 tiles (worlds 1 and 3) or 104 tiles (world 2).
`ending` then forces `CMD_RIGHT` every frame until `x > XSIZE` (off the right of the 512 px
window), which sets `passed`.

## Enemies

Enemies step `ENEMY_STEP = 1` px in `turn` (±1) when `(int)(enemy_iframe * 100) % 2 == 0`.
`enemy_iframe` advances by `TIME * 5`, so the gate is true on even hundredths of the frame
counter. They reverse if the body overlaps a tile **or** the tile under the next foot is empty.

## Bullets

Need `isShoot` (weapon pickup). First tap fires immediately; a held `J` retriggers every
`TIME_INTERVAL_BULLET = 0.2` s. Projectiles move `LEHGTH_INTERVAL_BULLET = 4` px per display
pass, die at screen edges, on a map hit, on an enemy, or past `MAX_DISTANCE = 480`.

Run `examples/physics/inertia_jump.cpp` for the numeric tables.
