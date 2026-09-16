# Example suite (index)

The EasyX game cannot compile on this Linux checkout. The portable
suite under [`examples/`](../examples/README.md) is the stand-in: it
rebuilds the integrator, collision rule, command bits, save format,
authored maps, jump, friction, and bullets with the same numbers as
`define.h` / `role.cpp` / `scene.cpp`.

```
cd examples && make test
```

| Program | Asserts |
| --- | --- |
| `01_inertia` | `Inertia::move` algebra; jump impulse `√(2Gh)` |
| `02_aabb` | Inset vertices; pipe 2×2 AABB |
| `03_commands` | `CMD_*` / `VIR_*` values and combinations |
| `04_save_load` | `gameRecord.dat` round-trip; reject 0/4 |
| `05_map_layout` | 32 vs 36 authored tiles; 30-slot cap; floaters |
| `06_jump_sim` | ~101 px rise; land snap; world-3 double tap |
| `07_friction` | T1/T2/T3 coast times; camera pin at 192 |
| `08_bullet` | +4 px/tick; expire at 480; hold-fire 4 shots / 0.61 s |
| `09_define_sync` | Every `define.h` number matches `maoliao_core.hpp` |
| `10_world_walk` | No-jump sprint dies in the first pit; auto-hop clears it |

Failed checks print `FAIL` and make `make test` return non-zero.
That is the regression gate for later personal tweaks to the math
headers.
