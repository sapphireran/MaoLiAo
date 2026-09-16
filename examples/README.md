# Portable MaoLiAo examples

These programs rebuild the **non-graphical** kernels of the EasyX game so they compile with any C++17 toolchain. They do not open a window and they do not link EasyX.

```
include/maoliao/   headers (constants, inertia, AABB, commands, save, levels, camera, hero)
src/               implementations
demos/             small drivers that print tables
tests/             assertion suite
data/              CSV copies of the authored maps
```

## Build

```bash
cd examples
make
make test
```

Binaries land in `build/`.

| Demo | What it prints |
| --- | --- |
| `inertia_demo` | suvat steps, time to `V_MAX`, ice slide-to-stop |
| `jump_arc` | airborne Y/`vY` until landing, apex vs `UNREAL_HEIGHT` |
| `aabb_demo` | vertex-in-box vs full AABB, 64×64 pipe mouths |
| `command_demo` | bit masks and the sticky `_kbhit` model |
| `savefile_demo` | `gameRecord.dat` round-trip and rejects |
| `level_preview` | tile counts, dropped overflow records, ASCII slices |
| `camera_demo` | `XRIGHT` lock and 1/5 parallax |

`make test` must stay green: constants, integrator, jump apex, friction, collision, commands, save I/O, level tables, camera lock.

Formulas and tables are documented under `../docs/`.
