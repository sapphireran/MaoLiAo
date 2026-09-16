# Portable 猫里奥 examples

The Win32 game needs EasyX. These programs do not. They copy the numbers and
formulas from `MaoLiAo/` into plain Python (and one C++ file) so the physics,
collision, world tables, save file, and a keyboard-scripted tick can be run
on Linux.

```
examples/
  maoliao_lib/          shared ports (constants, inertia, AABB, worlds, save, tick)
  physics/              jump + run demos, Python tests, C++ harness
  collision/            inset-corner vs full AABB
  input/                CMD_* / VIR_* bit masks
  levels/               table tests + ASCII dumps
  save/                 gameRecord.dat round-trip
  game_loop/            Role::action-shaped tick + a scripted run
  run_all.py            one entry point
```

## Run everything

From the repository root:

```bash
python3 examples/run_all.py
```

That script runs every `test_*.py`, the two physics demos, the overlap demo,
the three-world dump, a short scripted run, and (when `g++` is on PATH) the
C++ inertia harness.

Individual programs:

```bash
python3 examples/physics/test_inertia.py
python3 examples/physics/demo_jump.py
python3 examples/physics/demo_run_stop.py
g++ -std=c++17 -O2 -o /tmp/inertia_port examples/physics/inertia_port.cpp && /tmp/inertia_port

python3 examples/collision/test_aabb.py
python3 examples/input/test_commands.py
python3 examples/levels/test_worlds.py
python3 examples/levels/dump_layouts.py --world 1
python3 examples/save/test_save.py
python3 examples/game_loop/test_tick.py
python3 examples/game_loop/simulate_run.py
```

## What is — and is not — ported

| Ported | Left in the EasyX tree |
| --- | --- |
| `Inertia::move`, jump launch, friction `k` / `u` | `putimage`, MCI aliases, fonts |
| Corner-inset `isHit` | Sprite frame picking |
| World 1–3 tables, `MAP_NUMBER` cap, ASCII fixtures under `levels/fixtures/` | Unseeded CRT `rand()` (examples seed Python's RNG) |
| `gameRecord.dat` integer | Pause / title mouse hit boxes |
| One `TIME` step of run / jump / coins / stomps | Bullets (those advance in `Role::show`) |

Design notes that these programs illustrate: [docs/physics.md](../docs/physics.md),
[docs/collision.md](../docs/collision.md), [docs/levels.md](../docs/levels.md).
