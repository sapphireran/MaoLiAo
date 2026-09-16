# Physics

All numbers below come from `MaoLiAo/define.h` and the integrators in `MaoLiAo/inertia.cpp` and `MaoLiAo/role.cpp`. The portable sandbox in `examples/physics/` reprints the same formulas.

## Design constants

| Macro | Value | Role |
| --- | --- | --- |
| `TIME` | 0.01 s | integration step and frame budget |
| `G` | 30 | gravity magnitude (design “metres” / s²) |
| `REAL_HEIGHT` | 3.5 | intended jump height in those metres |
| `UNREAL_HEIGHT` | `3*HEIGHT+5` = 101 | pixels that should correspond to 3.5 m |
| `V_MAX` | 8.0 | max run speed in design metres / s |
| `A_ROLE` | 20.0 | horizontal key acceleration |
| `T1` / `T2` / `T3` | 0.5 / 1.2 / 1.5 | seconds to reach `V_MAX` on high / mid / low friction |
| `XLEFT` / `XRIGHT` | 0 / 192 | screen clamp for the hero |
| `WIDTH` / `HEIGHT` | 32 / 32 | tile and sprite size |

The metres-to-pixels scale is always

```text
pixels = metres * UNREAL_HEIGHT / REAL_HEIGHT
       = metres * 101 / 3.5
       ≈ metres * 28.857
```

## Integrator

`Inertia::move(v, t, a)` is the textbook constant-acceleration step, and it **mutates** `v`:

```text
X = v * t + 0.5 * a * t * t
v = v + a * t
return X
```

`Role::action` uses it twice per frame:

1. Vertical, only while `isFly`:
   `yy -= -Inertia::move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT`
2. Horizontal, every frame:
   `H = Inertia::move(vX, TIME, a + a1) * UNREAL_HEIGHT / REAL_HEIGHT`

If horizontal velocity crosses through zero in one step (`tmp * vX < 0`), `vX` is snapped to 0 so you do not bounce backwards from friction.

## Jump

A jump is allowed when:

```text
(CMD_UP && !isFly && !ending)  ||  (CMD_UP && world == 3)
```

World 3 therefore has infinite mid-air jumps. Worlds 1–2 need a floor contact (`isFly == false`).

On jump:

```text
isFly = true
vY    = -sqrt(2 * G * REAL_HEIGHT) = -sqrt(210) ≈ -14.4914
```

That is the closed-form takeoff speed for an apex of exactly `REAL_HEIGHT` metres under constant `G`. Pixel apex is therefore `UNREAL_HEIGHT` ≈ 101 px, a little over **three tiles**.

While airborne, gravity is applied as `+G` through `Inertia::move`. After integrating, the code probes `hitMap(x, y+1)`. If that hits:

- `vY` is zeroed
- `y` is quantized to the tile grid: `(y + HEIGHT/2) / HEIGHT * HEIGHT`
- if the probe happened while `vY > 0`, `isFly` becomes false (landed)
- if it happened while still rising, you bonk and `isFly` stays true

A stomp is `vY > 0` plus `hitEnemy`. That awards +5, writes a bomb sprite, and clears the enemy. Side contact (`vY <= 0`) kills the hero.

Falling past `YSIZE` (384) also kills.

Grounded frames that fail the `y+1` probe immediately set `isFly = true` (walked off a ledge). There is no coyote time.

## Horizontal motion and friction

Keys set a command acceleration `a`:

- left: `a -= A_ROLE` (−20)
- right, or `ending == true`: `a += A_ROLE` (+20)

Friction `a1` is applied only when you are **decelerating** (`vX * a <= 0` and `vX != 0`) **and** standing on a `Map*`:

```text
k  =  +4   if vX < 0     // Xabs/vX + 3 = -1 + 3
k  =  -2   if vX > 0     // Xabs/vX - 3 =  1 - 3
a1 = k * G * map->u
```

`map->u` is **not** a textbook µ. `Scene::createMap` stores

```text
u = (V_MAX / T) / G
```

so that `G * u = V_MAX / T`, the deceleration that would stop you from `V_MAX` in `T` seconds. Combined with `k`, the actual friction acceleration is `k * V_MAX / T`, i.e. 4× (left) or 2× (right) that design deceleration. The left/right asymmetry is intentional in the source (“+3-3调节惯性”).

`T` depends on tile id (worlds 1–2):

| Tile ids | `T` | Feel |
| --- | --- | --- |
| 1, 2, 3, 4, 5 | `T2` = 1.2 | grass / snow / cloud |
| 6 | `T1` = 0.5 | the short “icy / slick” strip in world 1 at x=67 |
| everything else (default) | `T3` = 1.5 | pipes and scenery-as-collider |

World 3 uses `T1` for ids 1, 3, 4, 5, 6 so pipes feel snappy.

After integration, `|vX|` is clamped to `V_MAX`.

Numeric values used by the toolkit and sandbox:

```text
u_mid   = (8 / 1.2) / 30 = 0.2222...
u_high  = (8 / 0.5) / 30 = 0.5333...
u_low   = (8 / 1.5) / 30 = 0.1777...
```

## Camera

The hero is not free to cross the window.

- `x < XLEFT` (0): clamp, zero `vX`
- `x > XRIGHT` (192) and not ending: `x0 -= (x - XRIGHT)`, then pin `x` to 192
- `x > XSIZE` (512): `passed = true` (only reachable once `ending` auto-runs you)

So after the first screen, the sprite sits on a 192 px rail and the **world slides under it** via `x0`. `Scene::action` copies `x0` to `xMap`. Sky moves `1/K_MAP_BG` as fast (`K_MAP_BG == 5`), only while pinned to the rail and `vX > 0`.

Map-space hero position (used by `isEnding` and by collision) is:

```text
map_x = -x0 + x
```

## Collision

`Role::isHit(p1[4], p2[2])` is a point-in-rect test: each of the hero’s four inset corners must be tested against the box `[p2[0], p2[1]]`.

Hero inset (screen `x,y` → map):

```text
(-x0 + x + 1,           y + 1)
(-x0 + x + WIDTH - 1,   y + 1)
(-x0 + x + 1,           y + HEIGHT - 1)
(-x0 + x + WIDTH - 1,   y + HEIGHT - 1)
```

Solid tiles are those with `0 < id < 11`. Ids 8 and 10 (pipe mouths) occupy `2*WIDTH × 2*HEIGHT` per `xAmount`/`yAmount`. Other solids occupy `WIDTH × HEIGHT` per cell.

Decorative ids 11–14 (grass tufts, goal sign, water, trees) are drawn by `Scene::show` and ignored by `hitMap`.

World 3 special case: a solid hit with `id != 2` sets `died` unless `isShoot` is true. Clouds (`id == 2`) stay safe landing pads. The weapon pickup is what sets `isShoot`, so it also functions as a star.

Horizontal resolution on a map hit is crude: if `x > XRIGHT` pin to the rail, else rewind this frame’s `H` and zero `vX`. There is no axis separation, so a jump into a ceiling and a run into a wall share that path.

## Enemies

Spawn positions are tiles; stored positions are pixels. Each qualifying frame (`(int)(enemy_iframe * 100) % 2 == 0`) they step `turn * ENEMY_STEP` (`ENEMY_STEP == 1`).

They reverse when:

- `hitMap` at their current drawn position, or
- the tile **in front and one pixel down** is empty (ledge)

They have no gravity and will not fall. `turn == 0` means “dead / unused”.

## Bullets

Unlocked by `hitFood`. `J` calls `setBullet` at the hero’s facing side.

- Step: `x += 4 * turn` (`LEHGTH_INTERVAL_BULLET`)
- Hold-to-fire interval: 0.2 s (`TIME_INTERVAL_BULLET`)
- Die when `x` leaves `[-WIDTH, XSIZE]`, they hit an enemy or a solid, or `x > MAX_DISTANCE` (480)

`MAX_DISTANCE` is compared against the **screen** `x`, so shots cannot reach far into a scrolled map — they explode around the right side of the window. Hitting an enemy is +5.

Bullet vs map always uses `hitMap(..., world=1)`, so world 3 shots do **not** trigger the “touch pipe and die” branch (that branch is keyed off `world == 3` inside `hitMap`).

## Worked jump (sandbox)

Closed form and discrete step agree to a few pixels:

```text
v0        = -sqrt(210) ≈ -14.4914 m/s
apex time = |v0| / G   ≈ 0.4830 s  (about 48 frames)
apex metres = 3.5
apex pixels = 101
```

`examples/physics/physics_sandbox.cpp` integrates the same loop the game uses and checks those numbers. `examples/toolkit/maoliao_levels.py jump` prints them for level authors (you can clear a 3-tile gap; a 4-tile wall needs a cloud).
