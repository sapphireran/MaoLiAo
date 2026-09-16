# Level toolkit

Python 3, standard library only.

```bash
python3 examples/toolkit/maoliao_levels.py legend
python3 examples/toolkit/maoliao_levels.py jump
python3 examples/toolkit/maoliao_levels.py validate examples/levels/*.json
python3 examples/toolkit/maoliao_levels.py render examples/levels/world-1-grassland.json
python3 examples/toolkit/maoliao_levels.py stats examples/levels
python3 examples/toolkit/maoliao_levels.py export-cpp examples/levels/example-world-4-canyon.json
python3 examples/toolkit/maoliao_levels.py pipes 3 5 2 6 4 1 5

python3 examples/toolkit/test_maoliao_levels.py
```

`validate` treats `MAP_NUMBER` overflow as a **warning** on the three shipped worlds (the C++ already drops the extra rows) and as an **error** on the example worlds.

`pipes` prints the same 30 records `Scene::createMap` would build for a given height roll.
