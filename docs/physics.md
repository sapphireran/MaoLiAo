# Physics

All motion goes through `Inertia::move` in `MaoLiAo/inertia.cpp`:

```
X = v * t + 0.5 * a * t * t
v = v + a * t
return X
```

`t` is always `TIME` (0.01 s). The function **mutates** `v`. Callers treat `X` as metres, then multiply by the pixel scale.

## Scale

| Symbol | Value | Meaning |
| --- | ---: | --- |
| `REAL_HEIGHT` | 3.5 | “feels like 3.5 metres” |
| `UNREAL_HEIGHT` | `3 * HEIGHT + 5` = 101 | pixel height of a max jump |
| scale | `101 / 3.5` ≈ 28.857 px/m | applied to every displacement |

Vertical update (from `Role::action`):

```
yy = yy - (-move(vY, TIME, G) * scale)
```

which is `yy += move(...) * scale`. Screen Y grows downward, but `vY` uses the usual “up is negative” convention (`G = +30`).

Horizontal:

```
H = move(vX, TIME, a + a1) * scale
xx = xx + H
```

## Jump

Allowed when `!isFly && !ending`, **or** always in world 3 (`world == 3` ignores grounded).

```
vY = -sqrt(2 * G * REAL_HEIGHT) = -sqrt(210) ≈ -14.4914
isFly = true
```

Time to apex: `t = 14.4914 / 30` ≈ **0.483 s** ≈ 48 frames.
Peak metres: 3.5. Peak pixels: 101. That is why `UNREAL_HEIGHT` exists.

Landing: probe `hitMap(x, y + 1)`. If `vY > 0` (falling) and a tile is there, snap

```
y = (y + HEIGHT/2) / HEIGHT * HEIGHT
```

to the 32 px grid, zero `vY`, clear `isFly`.

Walking off a ledge: if grounded but `hitMap(x, y+1)` is null, set `isFly` without giving an upward impulse — you fall.

Falling below `YSIZE` (384) sets `died`.

World 3 lets you flap every frame you hold jump. Each press (actually: every frame the bit is held) **reassigns** `vY` to the launch speed, so holding `W`/`K` is a hover. Combined with “most tiles kill”, that is the Flappy feel.

## Run

Wish acceleration `A_ROLE = 20` m/s² left or right (bit flags). `ending` forces `+A_ROLE` and ignores left.

Speed cap: if `|vX| > V_MAX` (8), rescale to ±8.

## Friction

`a1` is applied only when `vX * a <= 0 && vX != 0` (coasting or reversing). Air (`map == NULL`) gets `a1 = 0`.

On ground:

```
k = (vX < 0) ? ( |vX|/vX + 3 ) : ( |vX|/vX - 3 )
  = (vX < 0) ? 2 : -2
a1 = k * G * map->u
```

`u` is stored on each `Map` when `Scene::createMap` runs:

| Tile IDs | Formula | Typical `u` | Feel |
| --- | --- | ---: | --- |
| 1, 3, 4, 5 (and 2 in worlds 1–2) | `(V_MAX / T2) / G` | `(8/1.2)/30` ≈ 0.222 | medium (`T2 = 1.2`) |
| 6 (world 1 “ice” strip) | `(V_MAX / T1) / G` | `(8/0.5)/30` ≈ 0.533 | high friction (`T1 = 0.5`) |
| everything else | `(V_MAX / T3) / G` | `(8/1.5)/30` ≈ 0.178 | slip (`T3 = 1.5`) |

World 3 assigns ice (`T1`) to ids 1 and 3–6, medium to id 2, slip to pipes.

Comment in source says `a = mg*u/g`; the actual extra `k = ±2` doubles the usual `a = G*u`.

After the step, if `vX` **changed sign** relative to the pre-step value, it is clamped to 0 so you do not ping-pong through zero in one tick.

## Camera

Hero screen X is clamped to `[XLEFT, XRIGHT]` = `[0, 192]` while not in `ending`.

When the hero would pass 192:

```
x0 -= (x - XRIGHT)
x = XRIGHT
```

`x0` becomes more negative. `Scene::action` copies `x0` into `xMap` so tiles / coins / enemies are drawn at `xMap + worldX`.

Parallax: if the hero is parked on the right edge and still has `vX > 0` and the map origin actually moved,

```
bgStep = |vX| * TIME * scale / K_MAP_BG
xBg -= bgStep
```

`K_MAP_BG = 5` means the sky crawls at 1/5 of the ground. When `xBg <= -img_bg.width` it wraps by adding the width and a second copy is drawn to the right — seamless if the bitmap tiles.

World 3 (and 2) pick a vertical strip of `mapsky.bmp` with `yBg = -(world-1) * YSIZE`.

## Wall slide

After the horizontal step, `hitMap(x, y, scene, 1)` (note: **world forced to 1**, so world-3 “touch = death” does not fire here) rewinds `xx` by `H` and zeros `vX` if the new pose overlaps a solid. If `x > XRIGHT` it also clamps to `XRIGHT`.

## Worked numbers (checked by `examples/01_inertia_motion`)

Launch `vY = -sqrt(210)`. After 48 steps of `move(v, 0.01, 30)`:

- `vY` crosses 0 between frames 48 and 49
- summed pixel rise ≈ 101

A 1 s sprint from rest with `a = 20`, no friction, scale 101/3.5, caps when `vX` hits 8 (after `8/20 = 0.4 s`). Remaining time is constant-speed at `8 * scale` px/s ≈ 230.86 px/s.

## Example programs

- `examples/01_inertia_motion` — jump apex, scale, speed cap
- `examples/05_level_friction` — `u` tables and a coast-to-stop distance
- `examples/06_side_scroller_sim` — run + camera `x0` on a short track
