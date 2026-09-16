# Fixtures

Machine-readable copies of the 2020 hardcoded levels, physics macros, tile catalog, and save-slot samples. The game does **not** load these files. `tools/validate_fixtures.py` is what keeps them honest.

## Layout

```text
fixtures/
  physics.json              define.h macros + derived friction
  tiles.json                ID catalog (collision, sheet, friction band)
  schema.md                 field-by-field contract
  levels/world-1.json       authored + runtime tiles, coins, enemies, food
  levels/world-2.json       same
  levels/world-3.json       generation rules + one deterministic example
  saves/*.dat               raw gameRecord.dat bodies
  expected/world-N.ascii.txt   render_level.py goldens (runtime tiles)
```

## Units

| Field | Unit | Notes |
| --- | --- | --- |
| tile `x`, `y`, `xAmount`, `yAmount` | tiles (32 px) | Same as `Map` in `scene.cpp` |
| coin `x`, `y` | tiles | `Scene` multiplies by `WIDTH`/`HEIGHT` when drawing |
| enemy `x`, `y` | tiles | `Role::createEnemy` multiplies by 32 when storing pixels |
| enemy `turn` | +1 / −1 | +1 faces / walks right |
| food `x`, `y` | **pixels** | Already multiplied in the C++ (`14 * WIDTH`, …) |
| `ending_distance_tiles` | tiles | `isEnding` compares `distance > n * WIDTH` |

World 3 food `{10, 10}` is also pixels in the source, not tiles.

## Runtime vs authored

`runtime_tiles` is `authored_tiles[:30]` (`MAP_NUMBER`). ASCII goldens use `runtime_tiles` plus coins / enemies / food. Decorative IDs 11–14 are stored but not painted in the ASCII (they do not collide).

## Regenerating goldens

```bash
python3 tools/render_level.py --world 1 --write-expected
python3 tools/render_level.py --world 2 --write-expected
python3 tools/render_level.py --world 3 --write-expected
```

Only after the JSON already matches `scene.cpp` / `role.cpp`.
