# Examples

Portable companions for the EasyX game. Nothing here is loaded by `MaoLiAo.exe`. The JSON is a documented export of `Scene::createMap` / `createCoin` / `createFood` and `Role::createEnemy`, plus two extra design files you can paste in by following [docs/adding-a-level.md](../docs/adding-a-level.md).

## Layout

```
examples/
  levels/     world JSON + schema notes
  toolkit/    Python 3 CLI (stdlib only)
  physics/    g++ sandbox that mirrors Inertia::move
  snippets/   paste-ready C++ and a compilable hit test
```

## Quick commands

```bash
python3 examples/toolkit/maoliao_levels.py legend
python3 examples/toolkit/maoliao_levels.py jump
python3 examples/toolkit/maoliao_levels.py validate examples/levels/*.json
python3 examples/toolkit/maoliao_levels.py render examples/levels/world-1-grassland.json
python3 examples/toolkit/maoliao_levels.py stats examples/levels/*.json
python3 examples/toolkit/maoliao_levels.py export-cpp examples/levels/example-world-4-canyon.json

python3 examples/toolkit/test_maoliao_levels.py

make -C examples/physics test
```

## Shipped worlds vs extra designs

| File | In the `.exe` today? |
| --- | --- |
| `levels/world-1-grassland.json` | yes (world 1) |
| `levels/world-2-snow.json` | yes (world 2) |
| `levels/world-3-flappy.json` | yes, as one frozen `rand` roll |
| `levels/example-world-4-canyon.json` | no — paste candidate |
| `levels/example-tutorial-parkour.json` | no — jump-budget teaching map |

World 1 and 2 JSON include every authored tile, including those past `MAP_NUMBER` (30). `validate` reports the overflow the C++ copy loops actually perform.
