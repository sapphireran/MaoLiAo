# Fixture schema

Informal contract used by `tools/maoliao_fixtures.py`. There is no JSON Schema draft file so the validator can stay dependency-free; this document is the checklist.

## `physics.json`

```text
{
  "macros": { "<NAME>": number | string-number },
  "derived": {
    "friction": { "T1": u, "T2": u, "T3": u },
    "jump_v0": -sqrt(2*G*REAL_HEIGHT),
    "pixel_scale": UNREAL_HEIGHT / REAL_HEIGHT,
    "ending_distance_tiles": { "1": 94, "2": 104, "3": 94 }
  },
  "caps": { "MAP_NUMBER": 30, "COINS_NUMBER": 70, ... }
}
```

Every key in `macros` must appear as `#define NAME` in `define.h` with the same numeric value. Expressions (`3*HEIGHT+5`, `WIDTH*6`) are evaluated with the other macros.

## `tiles.json`

Array `tiles[]` of:

| Field | Type | Required |
| --- | --- | --- |
| `id` | int 1–14 | yes |
| `name` | string | yes |
| `collides` | bool | yes |
| `sheet` | `map.bmp` \| `scenery.bmp` | yes |
| `friction_band_w12` | `high` \| `medium` \| `low` \| `none` | yes |
| `notes` | string | no |

ID 9 may exist as `unused`.

## `levels/world-1.json` / `world-2.json`

```text
{
  "world": 1 | 2,
  "kind": "authored",
  "name": string,
  "ending_distance_tiles": int,
  "authored_tiles": [ Map, ... ],
  "runtime_tiles": [ Map, ... ],
  "dropped_tiles": [ Map, ... ],
  "coins": [ {x, y}, ... ],
  "enemies": [ {x, y, turn}, ... ],
  "food": [ {x, y, unit: "px"}, ... ]
}
```

`Map` is `{x, y, id, xAmount, yAmount}` with all ints. `runtime_tiles` must equal `authored_tiles[:30]`. `dropped_tiles` must equal `authored_tiles[30:]`. Tile IDs must exist in `tiles.json`.

## `levels/world-3.json`

```text
{
  "world": 3,
  "kind": "procedural",
  "ending_distance_tiles": 94,
  "generation": {
    "column_count_authored": 10,
    "column_count_used": 7,
    "x_formula": "i * 10 + 10",
    "height_formula": "rand() % (7 - 1) + 1",
    "height_min": 1,
    "height_max": 6,
    "coin_x_formula": "k * 10 + 5",
    "coin_y_formula": "rand() % (7 - 3) + 3",
    "coin_y_min": 3,
    "coin_y_max": 6,
    "coin_count": 7,
    "runway": {x, y, id, xAmount, yAmount},
    "flag": {x, y, id, xAmount, yAmount}
  },
  "example": {
    "pipe_heights": [7 ints in 1..6],
    "coin_heights": [7 ints in 3..6],
    "tiles": [30 Maps],
    "coins": [...],
    "enemies": [...],
    "food": [...]
  }
}
```

The validator rebuilds `example.tiles` and `example.coins` from the height tables + `generation` and diffs them against the stored arrays.

## `saves/*.dat`

Raw text. Companion `saves/manifest.json`:

```text
{ "file": "world-1.dat", "class": "valid", "world": 1 }
{ "file": "invalid-zero.dat", "class": "missing-archive" }
{ "file": "empty.dat", "class": "undefined-parse" }
```

Classes: `valid` | `missing-archive` | `undefined-parse` | `crash-open`.
