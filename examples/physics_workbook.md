# Physics workbook

Numbers you can recompute without opening Visual Studio. `make test` in this folder checks the same identities.

## Scale

```text
UNREAL_HEIGHT / REAL_HEIGHT = 101 / 3.5 ≈ 28.8571428571
```

A 1 “meter” displacement from `Inertia::move` becomes ≈ 28.86 pixels in `Role::action`.

## Jump

```text
vY0 = -√(2 × 30 × 3.5) = -√210 ≈ -14.4913769
Δv per frame = G × TIME = 0.3
frames to vY = 0: 14.4913769 / 0.3 ≈ 48.30
```

Continuous peak height in pixels is `REAL_HEIGHT × scale = 101`. The discrete loop in `inertia_demo` should peak within 2 px of that, around frame 48.

## Friction μ

```text
u = (V_MAX / T) / G = (8 / T) / 30
T2 = 1.2 → u ≈ 0.222222
T1 = 0.5 → u ≈ 0.533333
T3 = 1.5 → u ≈ 0.177778
```

World 1 ice strip (id 6 at x=67) uses T1. Clouds and grass use T2.

## One walk step

Hold D, `vX = 0`, on grass, `a = +20`, `a1 = 0` (same sign as `vX` after the first instant):

```text
H = (0 × 0.01 + ½ × 20 × 0.01²) × 28.857 ≈ 0.02886 px
vX ← 0.2
```

Cap: if `|vX| > 8`, it is scaled back to 8 **after** the step.

## Camera

Hero screen x is locked at 192 until `ending`. Each overflow pixel subtracts from `x0`. Sky moves at `1/5` of the scaled `|vX|` step when that lock is active.

## Finish

```text
world 1,3:  94 × 32 = 3008
world 2:   104 × 32 = 3328
then auto-run until screen x > 512
```

## Score

```text
coin 10    stomp 5    bullet-kill 5
```
