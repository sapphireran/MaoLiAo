# Portable examples

These programs copy numbers and helpers from the 2020 EasyX game so they
can run anywhere `g++` exists. They do **not** open a window and they do
**not** replace `MaoLiAo.sln`.

```bash
cd examples
make test
```

`make test` builds every demo and runs the self-checks. A non-zero exit
means a formula or table drifted from `docs/`.

| Program | Source it mirrors | What it checks |
| --- | --- | --- |
| `inertia_demo` | `Inertia::move`, jump / run in `Role::action` | apex ≈ 101 px, 40 frames to `V_MAX`, grass coast time |
| `jump_table` | same integrator | per-frame TSV of one jump |
| `collision_demo` | `Role::isHit` / `hitMap` | grass landing, 64×64 pipes, 1 px inset, camera shift |
| `command_bits_demo` | `Control::GetCommand` | bit-or masks and the sticky-key leftover |
| `save_record_demo` | `gameRecord.dat` | accept 1..3, reject 0/4, round-trip the sample `3` |
| `camera_scroll_demo` | `Role` pin + `Scene::action` | `XRIGHT=192`, `x0` scroll, sky `/5`, `isEnding` |
| `level_catalog` | `createMap` / `createEnemy` / `createCoin` | CSV or one-line summaries; `--warn` on truncation |

Shared header: [`include/maoliao_core.hpp`](include/maoliao_core.hpp).

## Catalog flags

```bash
./level_catalog --world 1
./level_catalog --world 2 --warn --format summary
./level_catalog --world 3 --seed 1 --format csv
./level_catalog --legacy-leq   # prints a note about the 2020 i<=count loops
```

World 3 pipes depend on `rand()`. Pass `--seed` so two dumps match.

## Sample save

`fixtures/gameRecord.sample.dat` is the same one-byte `3` that ships in
`MaoLiAo/gameRecord.dat`. `save_record_demo` writes a round-trip copy
next to it (`make clean` deletes that copy).
