# Constants reference

Source of truth: `MaoLiAo/define.h`, plus pool sizes in `role.h` and `scene.h`.

## Screen and tiles

| Macro | Value | Role |
| --- | --- | --- |
| `XSIZE` | 512 | Window width |
| `YSIZE` | 384 | Window height (12 tiles) |
| `WIDTH` | 32 | Tile / sprite cell |
| `HEIGHT` | 32 | Tile / sprite cell |
| `X0` | `2*WIDTH` = 64 | Hero spawn X (constructor also uses this for Y) |
| `Y0` | `3*HEIGHT` = 96 | Documented spawn Y; constructor currently sets `y = X0` (64) |
| `XLEFT` | 0 | Left clamp |
| `XRIGHT` | `WIDTH*6` = 192 | Camera lock X |
| `STEP` | 10 | Walk-cycle phase (`rolePos / STEP`) |
| `ENEMY_STEP` | 1 | Enemy pixels per AI tick |
| `K_MAP_BG` | 5 | World pixels per 1 background pixel |
| `F` | `TIME*0.3` | Scenery frame rate (macro accidentally ends with `;`) |
| `LIFE` | 5 | Starting lives (macro accidentally ends with `;`) |

`LIFE` and `F` are defined as `5;` and `TIME*0.3;`. The extra semicolon is harmless in `int life = LIFE;` but would break an expression such as `LIFE * 2`.

## Time and motion

| Macro | Value | Role |
| --- | --- | --- |
| `TIME` | 0.01 s | Integration step and frame delay |
| `REAL_HEIGHT` | 3.5 | “Meters” used in jump energy |
| `UNREAL_HEIGHT` | `3*HEIGHT+5` = 101 | Pixel scale of one “meter” |
| `G` | 30.0 | Gravity magnitude |
| `V_MAX` | 8.0 | Horizontal speed cap (model space) |
| `A_ROLE` | 20.0 | Horizontal run accel |
| `T1` | 0.5 s | High-friction time to `V_MAX` (ice/pipe id 6, world 3 solids) |
| `T2` | 1.2 s | Mid friction (grass, snow, clouds) |
| `T3` | 1.5 s | Low friction (default tiles) |

Friction coefficient stored on each `Map`:

```
u = (V_MAX / T) / G
```

so `G * u = V_MAX / T`. See [physics.md](physics.md).

## Command bits

| Macro | Bit | Source |
| --- | --- | --- |
| `CMD_LEFT` | 1 | `A` |
| `CMD_RIGHT` | 2 | `D` |
| `CMD_UP` | 4 | `W` or `K` |
| `CMD_DOWN` | 8 | `S` (read, unused) |
| `CMD_SHOOT` | 16 | `J` |
| `CMD_ESC` | 32 | Esc |
| `VIR_RETURN` | 64 | Pause → 返回游戏 |
| `VIR_RESTART` | 128 | Pause → 重新开始 |
| `VIR_HOME` | 256 | Pause → 退出游戏 (title) |

Bits are OR-ed so left+right+jump can be true in one `int`.

## Pools (`role.h` / `scene.h`)

| Name | Count | Notes |
| --- | --- | --- |
| `ENEMY_TOTE` | 30 | Walkers; `turn == 0` means unused |
| `BOMB_NUMBER` | 5 | Explosion slots |
| `BULLET_NUMBER` | 30 | Projectiles |
| `BULLET_INTERVAL` | 32 | Declared, unused |
| `LEHGTH_INTERVAL_BULLET` | 4 | Bullet speed (px / show tick) |
| `TIME_INTERVAL_BULLET` | 0.2 s | Auto-fire period while J is held |
| `MAX_DISTANCE` | 480 | Screen-X explode (not world distance) |
| `MAP_NUMBER` | 30 | Tile records; several worlds define more than 30 |
| `COINS_NUMBER` | 70 | Tile-space coins |
| `SCORE_NUMBER` | 5 | Pickup flash slots |
| `FOOD_NUMBER` | 5 | Weapon pickups |

## Helpers

```cpp
#define max(a,b) ((a) > (b) ? (a) : (b))
#define random(a,b) (rand()%(b-a)+a)   // [a, b)
```

World 3 pipe heights use `random(1, 7)` → 1..6. Coin heights use `random(3, 7)` → 3..6.

Include guard in `define.h` is `#ifndef MYDEFINE` / `#define MYDIFINE` (typo). The file still works because the first include defines the misspelled name and the guard token stays undefined — a second include would re-define every macro. In this project `define.h` is only included once per translation unit in practice.
