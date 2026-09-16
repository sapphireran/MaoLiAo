# Constants (`define.h`)

Every gameplay number the 2020 build actually compiles. Python mirrors live in
`examples/maoliao_lab/constants.py`.

## Screen and tiles

| Macro | Value | Meaning |
| --- | --- | --- |
| `XSIZE` | 512 | Window width (px) |
| `YSIZE` | 384 | Window height (px) = 12 tiles |
| `WIDTH` | 32 | Tile / sprite cell width |
| `HEIGHT` | 32 | Tile / sprite cell height |
| `X0` | `2 * WIDTH` = 64 | Hero spawn x (also mistakenly used as spawn *y* in `Role::Role`) |
| `Y0` | `3 * HEIGHT` = 96 | Documented spawn y; **not** assigned in `Role::Role` |
| `STEP` | 10 | Walk-cycle: `rolePos / STEP` picks iframe 1 vs 2 |
| `ENEMY_STEP` | 1 | Enemy pixels per movement tick |
| `K_MAP_BG` | 5 | Parallax: map moves 5 px for 1 px of sky |
| `XLEFT` | 0 | Left screen clamp for the hero |
| `XRIGHT` | `WIDTH * 6` = 192 | Right screen clamp; overflow feeds `x0` |

`Role::Role` sets `myHero.y = X0` (64), not `Y0`. The cat therefore spawns two
tiles from the top, then immediately falls until `hitMap` finds ground.

## Time and kinematics

| Macro | Value | Meaning |
| --- | --- | --- |
| `TIME` | 0.01 | Fixed step (s), also `Timer::Sleep` argument × 1000 |
| `REAL_HEIGHT` | 3.5 | Author's “meters” for a full jump |
| `UNREAL_HEIGHT` | `3 * HEIGHT + 5` = 101 | Pixel height that 3.5 “meters” maps to |
| `G` | 30.0 | Gravity in the *real* unit system |
| `V_MAX` | 8.0 | Horizontal speed cap (real units) |
| `A_ROLE` | 20.0 | Walk acceleration (real units / s²) |
| `T1` | 0.5 | Time to `V_MAX` on high-friction ground (id 6 in worlds 1–2) |
| `T2` | 1.2 | Time to `V_MAX` on normal ground |
| `T3` | 1.5 | Time to `V_MAX` on “slick / default” tiles |

Pixel displacement for a real-unit step `H_real` is

```
H_px = H_real * UNREAL_HEIGHT / REAL_HEIGHT
     = H_real * 101 / 3.5
     ≈ H_real * 28.857
```

Jump launch speed (real units, upward negative in screen space after the
later sign flip):

```
vY0 = -sqrt(2 * G * REAL_HEIGHT) = -sqrt(210) ≈ -14.4913767
```

Surface friction coefficient stored on each `Map`:

```
u = (V_MAX / T) / G
```

| Surface | `T` | `u` |
| --- | --- | --- |
| High friction (id 6 worlds 1–2; also ids 1,3,4,5,6 in world 3) | `T1` 0.5 | `8/0.5/30` ≈ 0.533333 |
| Normal (ids 1,3,4,5 and clouds id 2 in worlds 1–2) | `T2` 1.2 | `8/1.2/30` ≈ 0.222222 |
| Default / scenery leftovers | `T3` 1.5 | `8/1.5/30` ≈ 0.177778 |

## Command bits

OR-able. `Control::GetCommand` sets them from `GetAsyncKeyState`.

| Macro | Value | Key |
| --- | --- | --- |
| `CMD_LEFT` | 1 | A |
| `CMD_RIGHT` | 2 | D |
| `CMD_UP` | 4 | W or K |
| `CMD_DOWN` | 8 | S |
| `CMD_SHOOT` | 16 | J |
| `CMD_ESC` | 32 | Esc |
| `VIR_RETURN` | 64 | Pause → 返回游戏 |
| `VIR_RESTART` | 128 | Pause → 重新开始 |
| `VIR_HOME` | 256 | Pause → 退出游戏 (label says 退出, code treats it as home) |

## Limits in the headers (not `#define`s)

From `scene.h` / `role.h`:

| Name | Value | Notes |
| --- | --- | --- |
| `MAP_NUMBER` | 30 | World 1 has 32 tile records; world 2 has 36. Extra rows are dropped. |
| `COINS_NUMBER` | 70 | Far more slots than any world fills |
| `SCORE_NUMBER` | 5 | Simultaneous “+score” sparkles |
| `FOOD_NUMBER` | 5 | Weapon pickups |
| `ENEMY_TOTE` | 30 | Enemy slots |
| `BOMB_NUMBER` | 5 | Explosion sprites |
| `BULLET_NUMBER` | 30 | Live bullets |
| `BULLET_INTERVAL` | 32 | Declared, unused |
| `LEHGTH_INTERVAL_BULLET` | 4 | Bullet px / frame (typo: length) |
| `TIME_INTERVAL_BULLET` | 0.2 | Held-fire period (s) |
| `MAX_DISTANCE` | 480 | Bullet dies past this *screen* x |

## Macros that compile with a trailing semicolon

```c
#define F TIME*0.3;
#define LIFE 5;
```

`int life = LIFE;` becomes `int life = 5;;`. `scenery_iframe += F;` becomes
`scenery_iframe += TIME*0.3;;`. Harmless extra empty statements, easy to trip
over if you write `if (cond) F` without braces.

## Header-guard typo

```c
#ifndef MYDEFINE
#define MYDIFINE
```

`MYDEFINE` is never defined, so `define.h` has no include guard. See
[quirks.md](quirks.md).
