# Catalog

JSON and tiny save files copied out of the V2.0 sources so the kit does not
have to parse C++ on every run.

| File | Source |
| --- | --- |
| `constants.json` | `define.h`, `role.h`, `scene.h` |
| `tiles.json` | comments in `scene.cpp` + `hitMap` |
| `worlds/world1.json` | `Scene::createMap/Coin/Food`, `Role::createEnemy` |
| `worlds/world2.json` | same |
| `worlds/world3.json` | generation recipe + two MSVC LCG seeds |
| `saves/*.dat` | `gameRecord.dat` format |

World 2's flower is `39*WIDTH - WIDTH/3` and `3*HEIGHT + HEIGHT/5`. Those
macros are integers, so the pixel is **(1238, 102)**, not the real-valued
`(1248 - 32/3, 96 + 6.4)`.

World 3 tiles are **not** listed here. Build them with
`kit.worlds.generate_world3(seed=2020)`.
