# Portable examples

These programs copy the **rules** from `MaoLiAo/` into plain C++. They do not link EasyX or Win32. Use them to print jump tables, prove the four-corner hit test, dump the authored maps, and validate `gameRecord.dat`.

```bash
cd examples
make          # build bin/*
make test     # build + run every self-check
make clean
```

Need `g++` with C++11. `make test` is the check that runs on Linux; the `.sln` does not.

## Programs

| Target | Source | What it proves |
| --- | --- | --- |
| `bin/inertia_demo` | `inertia_demo/main.cpp` | `Inertia::move` matches `vt + ½at²`; discrete jump peak ≈ 101 px |
| `bin/collision_demo` | `collision_demo/main.cpp` | `Role::isHit` four-corner vs AABB; world-3 lethal tiles |
| `bin/map_layout` | `map_layout/main.cpp` | authored vs stored (`MAP_NUMBER` 30) tables + ASCII ground |
| `bin/save_file` | `save_file/main.cpp` | accept 1–3, reject 0/4/empty; write the same `"%d"` format |
| `bin/input_bits` | `input_bits/main.cpp` | `CMD_*` / `VIR_*` masks from `define.h` |

Shared headers live in `common/`:

- `maoliao_constants.h` — `define.h` numbers without `graphics.h`
- `inertia_portable.*` — `Inertia::move`
- `aabb.*` — `isHit` + `heroCorners`
- `map_data.*` — world 1–2 map / coin / enemy snapshots
- `save_format.*` — parse / format `gameRecord.dat`

## Samples

`save_file/samples/` holds tiny files you can copy to `MaoLiAo/gameRecord.dat`:

| File | Contents |
| --- | --- |
| `world1.dat` | `1` |
| `world2.dat` | `2` |
| `world3.dat` | `3` |
| `invalid.dat` | `9` |

Worked numbers without a compiler: [physics_workbook.md](physics_workbook.md).

## If you change the game

Update `common/maoliao_constants.h` when you edit `define.h`. Update `common/map_data.cpp` when you edit `createMap` / `createCoin` / `createEnemy`. Then `make test`.
