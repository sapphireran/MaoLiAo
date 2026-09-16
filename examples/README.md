# Portable examples

Personal reconstructions of 猫里奥 systems. They compile with any C++17 toolchain and do not
link EasyX, WinMM, or Visual Studio.

```bash
make test          # from this directory
./run_all.sh       # same thing
```

| Program | What it locks down |
| --- | --- |
| `physics/inertia_jump.cpp` | `Inertia::move`, jump apex, µ, the factor-of-two stop |
| `input/command_bits.cpp` | WASD/JK/Esc masks, pause virtual keys, menu rectangles |
| `collision/aabb_vertices.cpp` | 1-px inset, pipe 2×2 box, world-3 lethality |
| `save/save_format.cpp` | `gameRecord.dat` grammar, legal worlds 1–3 |
| `levels/world_catalog.cpp` | Authored tables, 30-tile cap, perfect scores |
| `levels/ascii_minimap.cpp` | World 1 raster + legend |
| `sprites/walk_frames.cpp` | Walk parity, sheet columns, iframe wraps, bullets |
| `camera/parallax_scroll.cpp` | `x0` pin, 1/5 sky, ending distances |
| `gameloop/tick_budget.cpp` | 100 Hz, five lives, 1→2→3→credits |

Shared headers in `common/` are the portable stand-ins for `define.h`, `inertia.cpp`,
`Role::isHit`, and the camera clamp.

`make` writes binaries into `build/` (gitignored via the Visual Studio `bin`/`obj` rules plus
`examples/.gitignore`).
