# Reference tables

Copied from `MaoLiAo/define.h`, `role.h`, and `scene.h`.
`examples/catalog/constants.json` is the machine-readable twin.
The check suite fails if these numbers drift from the headers.

## Screen and tiles

| Symbol | Value |
| --- | --- |
| `XSIZE` | 512 |
| `YSIZE` | 384 |
| `WIDTH` / `HEIGHT` | 32 |
| `X0` | 64 |
| `Y0` (unused spawn) | 96 |
| Actual spawn y | 64 (`X0`) |
| `XLEFT` / `XRIGHT` | 0 / 192 |
| `TIME` | 0.01 s |
| `STEP` | 10 px / walk frame |
| `ENEMY_STEP` | 1 px |
| `K_MAP_BG` | 5 |
| `F` | `TIME * 0.3` |
| `LIFE` | 5 |

## Motion

| Symbol | Value |
| --- | --- |
| `REAL_HEIGHT` | 3.5 |
| `UNREAL_HEIGHT` | 101 |
| Pixel scale | 101 / 3.5 ≈ 28.857142857 |
| `G` | 30 |
| `V_MAX` | 8 |
| `A_ROLE` | 20 |
| `T1` / `T2` / `T3` | 0.5 / 1.2 / 1.5 |
| Launch `vY` | `-sqrt(210)` ≈ −14.491376751 |

## Combat and pools

| Symbol | Value |
| --- | --- |
| `ENEMY_TOTE` | 30 |
| `BOMB_NUMBER` | 5 |
| `BULLET_NUMBER` | 30 |
| `BULLET_INTERVAL` | 32 |
| `LEHGTH_INTERVAL_BULLET` | 4 px / frame |
| `TIME_INTERVAL_BULLET` | 0.2 s |
| `MAX_DISTANCE` | 480 (screen x) |
| `MAP_NUMBER` | 30 |
| `COINS_NUMBER` | 70 |
| `SCORE_NUMBER` | 5 (puff slots) |
| `FOOD_NUMBER` | 5 |

## Scores

| Event | Points |
| --- | --- |
| Coin | +10 |
| Stomp | +5 |
| Bullet hits enemy | +5 |
| Flower | 0 (sets `isShoot`) |
| Clear | 0 |

## Ending distances

| World | Predicate | Pixels |
| --- | --- | --- |
| 1 | `distance > 94 * WIDTH` | 3008 |
| 2 | `distance > 104 * WIDTH` | 3328 |
| 3 | `distance > 94 * WIDTH` | 3008 |

## Command bits

`1 2 4 8 16 32` = left right up down shoot esc.
`64 128 256` = return restart home.

## Resources (as loaded)

| File | Use |
| --- | --- |
| `res/home.bmp` | Title + UI strips (5 screens tall) |
| `res/mapsky.bmp` | Parallax sky, 4 screens tall |
| `res/map.bmp` | Terrain strip, row = `id - 1` |
| `res/scenery.bmp` | Animated tufts / water / trees / goal |
| `res/ani.bmp` | Coins, flower, enemies, bombs, bullets |
| `res/role.bmp` | Hero walk / death |
| `res/*.mp3` | MCI aliases in `main.cpp` and `role.cpp` |
| `MaoLiAo.ico` | `IDI_ICON1` |
| `gameRecord.dat` | Last saved world |

`res/` is not in this GitHub snapshot (only sources and the save file).
The examples never load those bitmaps.
