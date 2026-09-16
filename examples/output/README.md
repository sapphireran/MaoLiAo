# Regenerated snapshots from the headless CLI

These files are the output of `examples/python/cli.py` (and `examples/cpp/inertia_demo`) at the time of the docs/examples commit. `unittest` compares the ASCII maps and lint log to this folder.

| File | Command |
| --- | --- |
| `world1.txt` | `python3 examples/python/cli.py preview --world 1` |
| `world2.txt` | `python3 examples/python/cli.py preview --world 2` |
| `world3_seed1.txt` | `python3 examples/python/cli.py preview --world 3 --seed 1` |
| `lint.txt` | `python3 examples/python/cli.py lint` |
| `physics_jump_head.txt` | `python3 examples/python/cli.py physics-jump --ticks 8` |
| `inertia_demo_head.txt` | `examples/cpp/inertia_demo \| head` |
| `extent_world1.json` / `extent_world2.json` | `cli.py extent --world N` |
| `decode_awj.txt` | `cli.py decode --keys A+W+J` |
| `save_read.txt` | `cli.py save-read MaoLiAo/gameRecord.dat` |

World 3 is a Python `Random(seed)` stand-in for `rand()`. Coin draws happen first (same order as `Scene::Scene`), then pipe heights.

How to read a map: each character is one 32×32 tile. Row 0 is the top of the 384 px window. `@` is the spawn pixel `(64, 64)` snapped to tile `(2, 2)`. The hero falls onto the first solid row under that column.
