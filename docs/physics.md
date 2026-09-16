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

That is the standard constant-acceleration step, with **velocity updated
after** the displacement. The Python port is `examples.maoliao_lab.inertia.step`.

`t` is always `TIME` (0.01 s). `a` and `v` live in the author's “real”
units. Callers convert the returned displacement to pixels with

```
pixels = real * UNREAL_HEIGHT / REAL_HEIGHT   // 101 / 3.5
```

## Vertical: jump and gravity

From `Role::action`:

1. If `CMD_UP` and the cat is grounded (or the world is 3), play `music_jump`
   and set
   `vY = -sqrt(2 * G * REAL_HEIGHT)`.
2. While `isFly`:
   `yy -= -Inertia::move(vY, TIME, G) * scale`
   which simplifies to `yy += move(vY, TIME, G) * scale`.
   Gravity `G = 30` is applied even on the way up, so the arc is a true
   parabola in real units.
3. Probe `hitMap(x, y+1)`. On a hit, snap `y` to the tile grid
   (`(y + HEIGHT/2) / HEIGHT * HEIGHT`), zero `vY`, and if `vY` *was*
   positive (falling), clear `isFly`.
4. While falling (`vY > 0`), a stomp (`hitEnemy`) scores +5 and spawns a
   bomb. Falling below `YSIZE` (384) is death.
5. If not flying, a failed `hitMap(x, y+1)` sets `isFly` again (walked off a
   ledge).

World 3 ORs an extra condition: `(KEY & CMD_UP) && world == 3`. That ignores
`isFly` and `ending`, so holding jump is a Flappy flap every tick. Combined
with pipe-touch death, that is the whole third-stage design.

Peak height in real units is `v² / (2G) = REAL_HEIGHT = 3.5`. In pixels that
is `UNREAL_HEIGHT = 101`, i.e. a hair over three tiles. The `+ 5` in
`3 * HEIGHT + 5` is a fudge so the sprite clears a 3-tile pipe lip.

## Horizontal: accel, friction, cap

Desired walk accel `a` is `±A_ROLE` (20) from `CMD_LEFT` / `CMD_RIGHT`.
When the cat is already moving the other way, friction accel `a1` is mixed
in:

```
k =  sign-ish fudge
     if vX < 0:  k = |vX|/vX + 3   // = 2
     else:       k = |vX|/vX - 3   // = -2
a1 = k * G * map->u     // only if standing on a tile and vX * a <= 0
```

So friction always points against `vX`, with magnitude `2 * G * u`.
Substituting `u = (V_MAX / T) / G` gives `|a1| = 2 * V_MAX / T`, i.e. twice
the “time-to-vmax” accel the comments describe. The extra factor of 2 is why
skids feel short.

After `Inertia::move`, if `vX` flipped sign, it is clamped to 0 (no reverse
skid through zero). Then `|vX|` is capped at `V_MAX`.

Hitting a solid tile at the new `x` rewinds `xx` by `H` and zeroes `vX`,
except when `x > XRIGHT`, in which case `x` is pinned to `XRIGHT` instead.
The `hitMap(..., world=1)` call here is **hard-coded world 1**, so World 3's
“touch pipe and die” rule is *not* applied to the horizontal resolver —
death for pipes is the vertical/overlap tests that pass `world` through.

## Camera and parallax

While `x > XRIGHT` and `ending` is false:

```
x0 -= (x - XRIGHT)
x  = XRIGHT
```

`x0` starts at 0 and goes negative. `Scene::action` copies it to `xMap`.
Sky motion only happens when the cat is glued to the right edge, `vX > 0`,
and `xMap` actually changed:

```
bgStep = |vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG
xBg   -= bgStep
```

When `xBg <= -img_bg.width` it wraps to 0 and a second blit fills the seam
(`putimage(width + xBg, yBg, &img_bg)`).

`K_MAP_BG = 5` means the sky crawls at 1/5 of the ground. `yBg` is not a
camera, it is a world-index: `-(world-1) * 384`.

## Auto-run into the goal

`Scene::isEnding(distance)` uses `distance = -x0 + x` (world-x of the cat).
Past 94 tiles (worlds 1 and 3) or 104 tiles (world 2), `ending` latches true.
From then on `CMD_RIGHT` is forced, jump is denied (except world 3's flap),
and walking off the right of the *window* (`x > XSIZE`) sets `passed`.

## Enemies

Enemies ignore inertia. Every other hundredth of `enemy_iframe` they shift
`turn * ENEMY_STEP` (1 px). They reverse if the *hero-relative* probe
`hitMap(x1, y1)` is solid **or** the foot probe `hitMap(x2, y2)` is empty.
Those probes pass `world = 1`, so World 3 enemies do not die on pipes.

## Bullets

`x += LEHGTH_INTERVAL_BULLET * turn` (4 px / frame, no accel). They vanish
off the 512-px window, on `hitEnemy`, on `hitMap(..., world=1)`, or when
`x > MAX_DISTANCE` (480). Explosion sprites are 64×64, last 4 iframes at
`TIME * 10`.

## Worked jump (first 5 ticks, no ceiling)

`vY0 ≈ -14.4913767`, `a = G = 30`, `scale ≈ 28.8571429`.

| tick | vY before | Δreal | vY after | Δpx ≈ |
| --- | --- | --- | --- | --- |
| 1 | -14.4914 | -0.14341 | -14.1914 | -4.14 |
| 2 | -14.1914 | -0.14041 | -13.8914 | -4.05 |
| 3 | -13.8914 | -0.13741 | -13.5914 | -3.97 |
| 4 | -13.5914 | -0.13441 | -13.2914 | -3.88 |
| 5 | -13.2914 | -0.13141 | -12.9914 | -3.79 |

`examples.run_lab jump-table` prints the full arc until `vY` crosses 0.
