# Examples

Headless personal kit for 猫里奥. Nothing here links EasyX.

```
catalog/     JSON pulled from the V2.0 literals
kit/         Python package (kinematics, collision, worlds, sim, …)
cpp/         C++17 single header + small demos
tools/       dump maps, jump table, gap list, replay, save file
replays/     tiny input scripts for the hero stepper
tests/       catalog ↔ source ↔ kit checks
```

## Run

From this directory:

```bash
make check
```

Pieces:

```bash
python3 tests/test_kit.py
python3 tools/jump_table.py
python3 tools/dump_worlds.py        # ASCII + SVG + PNG → out/
python3 tools/measure_gaps.py       # jump envelope vs authored gaps
python3 tools/replay.py replays/world1_opening.txt
python3 tools/record_demo.py read ../MaoLiAo/gameRecord.dat

make cpp                            # g++ -std=c++17
./out/jump_table
./out/aabb_check
./out/test_kit .
```

`out/world1.svg` … `out/world3.svg` are the plan views used in the PR.
World 2’s SVG is the **runtime** map (goal and trees omitted) unless you pass
`--no-cap` to `dump_worlds.py`.

## What is simulated

`kit.sim.HeroSim` steps jump, friction, the 192-px camera rail, coins, the
flower, the ending tripwire, and pit death. It does **not** patrol enemies
or fly bullets — those stay in `MaoLiAo/role.cpp`.

World 3 pipes use the MSVC `rand` LCG so seed `2020` is stable here. A real
V2.0 run never calls `srand`, so in-game columns will differ.

## Replay DSL

```
80 .        # 80 idle frames
20 D        # hold D
1  DW       # one frame of D+W (jump while moving)
```

`#` starts a comment. `.` is idle. Letters are the same as the game
(A D W K S J).
