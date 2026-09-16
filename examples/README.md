# Portable 猫里奥 lab

Headless g++ programs that replay the homework constants and formulas. They do **not** link EasyX. Run everything from this directory so `testdata/` paths resolve.

```bash
make          # build examples/bin/*
make test     # every lab must print `<name>: ok`, then ALL EXAMPLE DEMOS PASSED
make clean
```

## Layout

```
include/maoliao/     header-only model (constants, kinematics, AABB, worlds…)
src/                 one main() per lab
testdata/            CSV dumps of worlds 1–2 plus a sample save file
bin/                 build output (gitignored)
```

## Labs

| Program | What it checks |
|---|---|
| `jump_lab` | `v0 = -√210`, discrete apex just under 3.5 m / 101 px |
| `friction_lab` | `u = (V_MAX/T)/G`, `|k|=2`, T1 stops sooner than T2, `V_MAX` clamp |
| `aabb_lab` | inset corners, 2× pipe boxes, scenery skip, world-3 instadeath |
| `command_lab` | `CMD_*` / `VIR_*` bit masks |
| `save_lab` | `gameRecord.dat` `"%d"` round-trip, valid worlds 1–3 |
| `camera_lab` | pin at `XRIGHT=192`, sky step `/5`, ending run past the window |
| `world_atlas` | authored vs `MAP_NUMBER` stored counts; CSV agrees with `worlds.hpp` |
| `enemy_patrol` | flip at walls / missing floor |
| `pipe_field` | seven pipe groups + runway + flag = 30 tiles |
| `score_lab` | +10 / +5 and the life / world machine |
| `tick_replay` | fall onto world-1 grass, jump, hold right, collect overlapping coins |
| `run_all` | runs the eleven labs and prints `ALL EXAMPLE DEMOS PASSED` |

Loops in this lab use `< count`. They do not copy the homework `<= sizeof` overruns.

Need g++ with `-std=c++17`. Verified with g++ 13.3 on this agent.
