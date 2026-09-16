# Physics

All motion goes through `Inertia::move` in `inertia.cpp`:

```cpp
double Inertia::move(double& v, double t, double a)
{
    double X = v * t + 1.0 / 2.0 * a * t * t;
    v = v + a * t;
    return X;
}
```

That is the standard constant-acceleration step. `t` is always `TIME` (`0.01`) during gameplay.

Pixel positions are **not** in metres. The code keeps a “real” height `REAL_HEIGHT = 3.5` (comment: metres) and a pixel jump cap `UNREAL_HEIGHT = 3 * HEIGHT + 5 = 101`. Every displacement returned by `move` is scaled by `UNREAL_HEIGHT / REAL_HEIGHT` before it is added to `Hero.xx` / `Hero.yy`.

```
scale = 101 / 3.5 = 28.857...
```

The examples print this factor as `PIXELS_PER_METRE`.

## Vertical: jump and gravity

Jump is allowed when:

- `(KEY & CMD_UP) && !isFly && !ending`, or
- `(KEY & CMD_UP) && world == 3`  (air jump / flap)

Initial vertical speed:

```
vY = -sqrt(2 * G * REAL_HEIGHT) = -sqrt(2 * 30 * 3.5) = -sqrt(210) ≈ -14.4914
```

While `isFly`, each tick:

```
yy = yy - (-move(vY, TIME, G) * scale)
   = yy + move(vY, TIME, +30) * scale
```

`G` is **+30** and applied even on the way up, so the jump is a ballistic arc. After integrating, `hitMap(x, y+1)` is tested:

- On hit: `vY = 0`, snap `y` to the tile grid via `(y + HEIGHT/2) / HEIGHT * HEIGHT`, set `isFly = false` if `vY` was positive (falling onto a floor)
- While falling (`vY > 0`): stomping (`hitEnemy`) scores +5 and spawns a bomb; `y > YSIZE` (fell out of the window) sets `died`

If the hero is not flying but `hitMap(x, y+1)` is null, `isFly` becomes true (walked off a ledge).

## Horizontal: run, friction, camera

Acceleration from input:

- Left (`CMD_LEFT`, and not `ending`): `a -= A_ROLE` (`A_ROLE = 20`)
- Right (`CMD_RIGHT` **or** `ending`): `a += A_ROLE`

Friction acceleration `a1` is applied only when velocity and control acceleration have opposite signs or the player released the key (`vX * a <= 0 && vX != 0`):

```
k = +3   if vX < 0     (code: Xabs/vX + 3 → -1 + 3)
k = -3   if vX > 0     (code: Xabs/vX - 3 →  1 - 3)
a1 = k * G * map->u     if standing on a tile; else 0 in air
```

`map->u` is assigned in `Scene::createMap` from tile id:

| Tile ids | Formula | T used | Intent (comment) |
| --- | --- | --- | --- |
| 1, 3, 4, 5 | `(V_MAX / T2) / G` | `T2 = 1.2` | medium friction |
| 6 | `(V_MAX / T1) / G` | `T1 = 0.5` | high friction (“ice-looking” pipe tops in world 1 use id 6) |
| 2 (clouds) | `(V_MAX / T2) / G` | `T2` | same as grass |
| other | `(V_MAX / T3) / G` | `T3 = 1.5` | low friction |

With `V_MAX = 8` and `G = 30`:

```
u_T1 = (8 / 0.5) / 30 = 0.5333...
u_T2 = (8 / 1.2) / 30 ≈ 0.2222
u_T3 = (8 / 1.5) / 30 ≈ 0.1778
```

After `move`, if velocity **reversed** this tick (`tmp * vX < 0`), `vX` is zeroed (no oscillation through the origin). Speed is then clamped so `|vX| ≤ V_MAX`.

Walls: if `hitMap(x, y, world=1)` after the horizontal step, x is either clamped to `XRIGHT` or rolled back by `H`, and `vX` is zeroed. Note the hard-coded `world=1` so world-3 “touch = death” does **not** trigger on this particular call.

## Camera window

The hero’s on-screen x is kept in `[0, XRIGHT]` with `XRIGHT = WIDTH * 6 = 192`, unless `ending` is set.

- `x < XLEFT` (0): clamp, zero `vX`
- `x > XRIGHT` and not ending: `x0 -= (x - XRIGHT)` then pin `x` to 192. The world origin slides; `Scene::action` copies `x0` into `xMap` and, when pinned to the right and moving right, shifts the sky by

```
bgStep = |vX| * TIME * scale / K_MAP_BG
K_MAP_BG = 5
```

so the far background moves at 1/5 of the map.

## Worked jump (first 20 ms)

`vY0 = -√210`, `dt = 0.01`, `a = 30`, `scale = 101/3.5`.

Tick 1:

```
disp = vY0*dt + ½*30*dt² ≈ -0.144914 + 0.0015 = -0.143414
vY1  = vY0 + 30*0.01 ≈ -14.1914
Δpx  = disp * scale ≈ -4.138
```

The examples in `examples/python` and `examples/cpp/inertia_demo` reproduce this table.

## World 3

Gravity and jump speed are unchanged, but because `CMD_UP` is accepted while already airborne, the player can reset `vY` to `-√210` every tick they hold `W`/`K`. Combined with “any solid except tile id 2 kills unless `isShoot`”, the stage plays like a flap-through-pipes level rather than a walker.
