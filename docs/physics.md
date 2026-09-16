# Physics

All motion uses a fixed step `TIME = 0.01` s. `Inertia::move` is the only integrator.

```c
X = v * t + 0.5 * a * t * t;
v = v + a * t;
return X;
```

`v` is updated in place. The game then scales horizontal and vertical **pixels** by

```text
UNREAL_HEIGHT / REAL_HEIGHT = (3 * 32 + 5) / 3.5 = 101 / 3.5 ≈ 28.857
```

so a “meter” in the jump formula becomes many screen pixels. `examples/inertia_demo` prints the same numbers.

## Constants (`define.h`)

| Macro | Value | Role |
| --- | --- | --- |
| `TIME` | 0.01 | frame dt |
| `G` | 30.0 | gravity (down is +Y on screen) |
| `REAL_HEIGHT` | 3.5 | jump height used in `√(2gH)` |
| `UNREAL_HEIGHT` | 101 | pixel jump scale |
| `V_MAX` | 8.0 | max \|vX\| before pixel scale |
| `A_ROLE` | 20.0 | walk accel while A/D held |
| `T1` `T2` `T3` | 0.5, 1.2, 1.5 | time-to-`V_MAX` used to derive friction |
| `XLEFT` `XRIGHT` | 0, 192 | screen clamp until ending |
| `K_MAP_BG` | 5 | sky moves 1 px per 5 px of map feel |
| `LEHGTH_INTERVAL_BULLET` | 4 | bullet step (pixels / show) |
| `TIME_INTERVAL_BULLET` | 0.2 | seconds between shots while J held |
| `MAX_DISTANCE` | 480 | bullet dies (and can explode) past this **screen** x |
| `ENEMY_STEP` | 1 | enemy pixel step when their ticker fires |

## Jump

On the frame `CMD_UP` is accepted:

```c
vY = -sqrt(2 * G * REAL_HEIGHT);   // ≈ -14.4914
isFly = true;
```

Worlds 1–2 require `isFly == false` and `ending == false`. World 3 ORs `(CMD_UP && world == 3)`, so you can refresh `vY` every frame you hold jump — that is the Flappy input.

Each airborne frame:

```c
yy -= -Inertia::move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT;
```

`move` with `a = +G` makes `vY` less negative, then positive. The extra minuses convert “up is negative velocity” into “up is smaller `y`”.

Landing: `hitMap(x, y+1)` while `vY > 0` clears `isFly`, zeros `vY`, and snaps `y` to a 32-pixel row through `(y + HEIGHT/2) / HEIGHT * HEIGHT`.

Falling below `YSIZE` (384) sets `died`.

Stomp: airborne, `vY > 0`, `hitEnemy` → +5, bomb sprite, enemy cleared. Side contact with `vY <= 0` kills the hero.

## Horizontal motion

`a` starts at 0. A subtracts `A_ROLE`, D (or `ending`) adds `A_ROLE`.

Friction `a1` only when `vX` and `a` have opposite signs or you released (the code checks `vX * a <= 0 && vX != 0`):

```c
a1 = k * G * map->u;
```

`k` is a sign tweak (`±3` plus `Xabs/vX`) so friction always opposes travel. Airborne (`map == NULL`) sets `a1 = 0`.

Then:

```c
H = Inertia::move(vX, TIME, a + a1) * UNREAL_HEIGHT / REAL_HEIGHT;
xx += H;
```

If `vX` crossed through 0 this step, it is forced to 0. If `|vX| > V_MAX`, it is clamped **after** the integrate (the pixel step can slightly exceed the scaled cap for one frame).

Wall: `hitMap` with `world` forced to `1` (so world 3’s “touch = death” does not apply to this horizontal resolver). Hero is pushed back by `H` or clamped to `XRIGHT`.

## Tile friction `u`

Assigned in `Scene::createMap`:

```text
u = (V_MAX / T) / G
```

| Tile ids (worlds 1–2) | T | u |
| --- | --- | --- |
| 1, 3, 4, 5, 2 (grass, dirt, snow, cloud) | T2 = 1.2 | 8/1.2/30 ≈ 0.22222 |
| 6 (icy / high-friction strip in world 1) | T1 = 0.5 | 8/0.5/30 ≈ 0.53333 |
| other | T3 = 1.5 | 8/1.5/30 ≈ 0.17778 |

World 3 maps 1 and 3–6 onto T1 (grippy), clouds (2) onto T2, pipes onto T3. Pipes in world 3 are also lethal unless `isShoot` is true ([levels.md](levels.md)).

## Camera and parallax

While `x > XRIGHT` and the run is not in `ending`:

```c
x0 -= (x - XRIGHT);
x = XRIGHT;
```

`x0` is the camera origin (negative as you walk right). `Scene::action` sets `xMap = x0`. Sky step:

```c
bgStep = |vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG
```

applied only when pinned at `XRIGHT`, `vX > 0`, and `xMap` just changed.

## Bullets

`setBullet` copies hero `x`/`y`/`turn`. `bullteFlying` (typo in the method name) each `show`:

1. Off-screen (`x > 512` or `x < -32`) → despawn
2. Else if `hitEnemy` or `hitMap(..., world=1)` or `x > 480` → bomb, maybe +5, despawn
3. Else `x += 4 * turn`

Interval: first tap fires immediately; while J stays down, the next shots wait 0.2 s of accumulated `TIME`.

## Enemies

When `(int)(enemy_iframe * 100) % 2 == 0`, each live enemy moves `turn * ENEMY_STEP`. Reverse if the body overlaps a solid (`hitMap` world 1) **or** the tile one body-width ahead at `y+1` is empty (edge).

## Worked jump (no collision)

From rest, one press in worlds 1–2:

```text
vY0 = -√210 ≈ -14.4913766
```

After n steps, `vY = vY0 + n * G * TIME = vY0 + n * 0.3`. Apex when `vY` crosses 0: `n ≈ 48.3` frames ≈ 0.483 s. Peak pixel lift is about `REAL_HEIGHT * UNREAL_HEIGHT / REAL_HEIGHT = 101` px if you ignore the discrete last step — the demo prints the exact discrete peak.

`make -C examples test` includes that peak check.
