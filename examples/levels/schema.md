# Level JSON schema

Used only by `examples/toolkit/maoliao_levels.py`. The EasyX game does not read these files.

## Root object

| Field | Type | Required | Meaning |
| --- | --- | --- | --- |
| `id` | string | yes | Stable file id (`world-1`, `example-world-4-canyon`, …) |
| `world` | int | yes | Value you would pass to `Scene(world)` / `Role(world)` |
| `name` | string | yes | Human title |
| `theme` | string | no | `grass`, `snow`, `pipes`, `canyon`, `tutorial` |
| `ending_tiles` | int | yes | `Scene::isEnding` distance in tiles |
| `friction_profile` | string | yes | `overworld` or `pipes` |
| `notes` | string | no | Free text |
| `procedural` | object | no | World 3 generator parameters |
| `tiles` | array | yes | `Map` records |
| `coins` | array | yes | Tile-space coins |
| `food` | array | yes | Pickups |
| `enemies` | array | yes | Walkers |

## `tiles[]`

| Field | Type | Meaning |
| --- | --- | --- |
| `x`, `y` | int | Tile column / row |
| `id` | int | 1–14, see [docs/map-format.md](../../docs/map-format.md) |
| `x_amount`, `y_amount` | int | Repeat counts (`>= 1` for something visible) |
| `kind` | string | optional label for authors |

`u` is never stored; the toolkit derives it from `id` + `friction_profile`.

## `coins[]`

`{ "x": int, "y": int }` in tiles. `(0, 0)` is treated as empty by `Scene::show` / `hitCoins`, so do not place a coin on the origin.

## `food[]`

| Field | Type | Meaning |
| --- | --- | --- |
| `x`, `y` | number | Position |
| `unit` | `"tiles"` or `"pixels"` | Required. Worlds 1–3 store pixels. |

## `enemies[]`

`{ "x": int, "y": int, "turn": 1 | -1 }` in tiles. `Role::createEnemy` multiplies by 32.

## `procedural` (world 3)

```json
{
  "pipe_columns": 7,
  "height_min": 1,
  "height_max": 6,
  "x0": 10,
  "x_step": 10,
  "frozen_heights": [3, 5, 2, 6, 4, 1, 5]
}
```

`frozen_heights` is one legal `rand() % 6 + 1` roll, used so `render` / `validate` are deterministic. The running game calls `rand` with no `srand`.

## Caps (must match headers)

```
MAP_NUMBER     30
COINS_NUMBER   70
FOOD_NUMBER     5
ENEMY_TOTE     30
```

`validate` emits an **error** if a *new* example exceeds a cap, and a **warning** if a shipped world exceeds `MAP_NUMBER` the way the C++ already does.
