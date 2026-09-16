# Headless MaoLiAo examples

These programs replay the V2.0 formulas and authored level tables without EasyX, Win32, or the BMP/MP3 pack. They exist so the personal notes in `docs/` can be checked on Linux.

```
examples/
  maoliao_sim/     importable ports (constants, inertia, collision, commands, save, worlds)
  physics/         jump arc + friction tables (Python) and Inertia::move (C++)
  levels/          ASCII maps and JSON export
  input/           command-bit decoder
  save/            gameRecord.dat read/write
  tests/           one-file regression suite
```

## Run the suite

From the repository root:

```bash
python3 examples/tests/test_all.py
```

The suite compiles `examples/physics/inertia_demo.cpp` with `g++` and diffs its peak jump against the Python integrator.

## Physics

```bash
python3 examples/physics/jump_profile.py
python3 examples/physics/run_kinematics.py
g++ -O2 -std=c++17 -o /tmp/inertia_demo examples/physics/inertia_demo.cpp
/tmp/inertia_demo
```

`jump_profile.py` should report a peak near 101 px (the discrete step undershoots the continuous `UNREAL_HEIGHT` a little). `run_kinematics.py` shows why id-6 ice dumps speed in half the frames of grass.

## Levels

```bash
python3 examples/levels/ascii_map.py --world 1 --legend
python3 examples/levels/ascii_map.py --world 2
python3 examples/levels/ascii_map.py --world 3 --seed 2020
python3 examples/levels/export_worlds.py -o /tmp/maoliao_worlds.json
python3 examples/levels/svg_map.py --world 1 -o /tmp/world1.svg
```

Worlds 1 and 2 draw only the first 30 map records, the same cap as `MAP_NUMBER` in `scene.h`. Pass `--no-cap` to see what `createMap` never copies: two water tiles on world 1, and the goal sign plus trees on world 2.

## Input and save

```bash
python3 examples/input/command_demo.py
python3 examples/input/command_demo.py a w j --world 3 --airborne
python3 examples/save/record_demo.py --read MaoLiAo/gameRecord.dat
python3 examples/save/record_demo.py --write /tmp/gameRecord.dat --world 2
```

## When you change the game

Update `maoliao_sim/worlds.py` and the counts in `tests/test_all.py` in the same commit as `scene.cpp` / `role.cpp` / `define.h`. The suite is the contract that the notes still describe this tree.
