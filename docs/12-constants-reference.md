# 12 — Constants reference

All gameplay macros live in `MaoLiAo/define.h` unless noted. A few capacities live in `scene.h` / `role.h`.

## Screen and tiles

| Macro | Value | Meaning |
| --- | --- | --- |
| `XSIZE` | 512 | Window width |
| `YSIZE` | 384 | Window height (12 tiles) |
| `WIDTH` | 32 | Tile / sprite cell width |
| `HEIGHT` | 32 | Tile / sprite cell height |
| `X0` | `2*WIDTH` (64) | Hero spawn x (also wrongly used as spawn y in `Role::Role`) |
| `Y0` | `3*HEIGHT` (96) | Intended spawn y — **not used** by the constructor |

## Timing and steps

| Macro | Value | Meaning |
| --- | --- | --- |
| `TIME` | 0.01 | Integration step and frame budget (seconds) |
| `STEP` | 10 | Walk-cycle: `rolePos / STEP` picks hero frame 1 vs 2 |
| `ENEMY_STEP` | 1 | Enemy pixels per patrol tick |
| `F` | `TIME*0.3` | Scenery frame advance (note: `#define F TIME*0.3;` includes a semicolon) |
| `LIFE` | `5;` | Life count (semicolon is part of the macro — see design notes) |

## Camera

| Macro | Value | Meaning |
| --- | --- | --- |
| `XLEFT` | 0 | Hero cannot walk left of screen x = 0 |
| `XRIGHT` | `WIDTH*6` (192) | Hero pins here; further motion scrolls `x0` |
| `K_MAP_BG` | 5 | Sky moves 1 px per 5 px of “feel” motion |

## Physics

| Macro | Value | Meaning |
| --- | --- | --- |
| `REAL_HEIGHT` | 3.5 | Jump height in the fake “meters” space |
| `UNREAL_HEIGHT` | `3*HEIGHT+5` (101) | Pixel jump scale |
| `G` | 30.0 | Gravity (meters / s² in the fake space) |
| `V_MAX` | 8.0 | Horizontal speed cap (same space as `G`) |
| `A_ROLE` | 20.0 | Horizontal acceleration from A / D |
| `T1` | 0.5 | Time to `V_MAX` on high-friction tiles (id 6, and most world-3 solids) |
| `T2` | 1.2 | Time to `V_MAX` on mid friction (grass / snow / clouds) |
| `T3` | 1.5 | Time to `V_MAX` on low friction (default / scenery leftovers) |

Friction on a tile is stored as `u = (V_MAX / Tn) / G`, then applied as `a1 = k * G * u` when the hero is decelerating. `k` is `+2` when `vX < 0` and `-2` when `vX > 0`.

Launch velocity for a jump:

```text
vY = -sqrt(2 * G * REAL_HEIGHT)  ≈ -14.491
```

Pixel motion each tick:

```text
pixels = Inertia::move(v, TIME, a) * UNREAL_HEIGHT / REAL_HEIGHT
```

## Command bits (`define.h`)

| Macro | Value | Source |
| --- | --- | --- |
| `CMD_LEFT` | 1 | `A` |
| `CMD_RIGHT` | 2 | `D` |
| `CMD_UP` | 4 | `W` or `K` |
| `CMD_DOWN` | 8 | `S` |
| `CMD_SHOOT` | 16 | `J` |
| `CMD_ESC` | 32 | `Esc` |
| `VIR_RETURN` | 64 | Pause: resume |
| `VIR_RESTART` | 128 | Pause: restart world |
| `VIR_HOME` | 256 | Pause: home |

`GetCommand` **ors** whatever is down. Virtual keys **replace** `Control::key`.

## Combat (`role.h`)

| Constant | Value | Meaning |
| --- | --- | --- |
| `ENEMY_TOTE` | 30 | Enemy array |
| `BOMB_NUMBER` | 5 | Explosion FX slots |
| `BULLET_NUMBER` | 30 | Bullet slots |
| `BULLET_INTERVAL` | `WIDTH` (32) | Declared, unused |
| `LEHGTH_INTERVAL_BULLET` | 4 | Bullet pixels per show-tick (typo in the name) |
| `TIME_INTERVAL_BULLET` | 0.2 | Seconds between shots while J is held |
| `MAX_DISTANCE` | 480 | Screen-x where a bullet self-explodes |

## Scene capacities (`scene.h`)

| Constant | Value |
| --- | --- |
| `MAP_NUMBER` | 30 |
| `COINS_NUMBER` | 70 |
| `SCORE_NUMBER` | 5 |
| `FOOD_NUMBER` | 5 |

## Score rules (hard-coded in `role.cpp`)

| Event | Delta |
| --- | --- |
| Coin | +10 |
| Stomp enemy | +5 |
| Bullet hits enemy | +5 |

Mushrooms do not add score. They set `Hero::isShoot = true`.

## Header guard typo

```cpp
#ifndef MYDEFINE
#define MYDIFINE    // not MYDEFINE
```

The guard never matches on a second include via this pair. `#pragma once` at the top of `define.h` is what actually saves you.
