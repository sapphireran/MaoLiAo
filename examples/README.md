# Portable MaoLiAo examples

Extracts of the 2020 game rules that compile **without EasyX or Windows**.
They exist so the physics, collision, camera, save, and scoring kernels can be
re-run on a laptop that is not the original Visual Studio box.

```bash
make -C examples
make -C examples test
```

| Program | What it locks in |
| --- | --- |
| `01_inertia` | `s = vt + ½at²`, jump `√(2gH)`, friction `k = ±2`, `u = Vmax/(Tn g)` |
| `02_collision` | four-corner hit, pipe 64×64 box, world-3 kill table, known thin-wall miss |
| `03_input_bits` | `GetCommand` or-mask and `VIR_*` replacements |
| `04_save_record` | `gameRecord.dat` accept `{1,2,3}`, reject empty / missing / 99 |
| `05_camera_parallax` | pin to `[0, 192]`, `x0` scroll, ending release, sky step |
| `06_level_end` | 94 / 104 / 94 tile thresholds and `x > 512` clear |
| `07_world3_pipes` | seven pipe pairs + cloud runway, ASCII dump |
| `08_score_rules` | +10 coin, +5 stomp / shot, score dies with `Role` |
| `09_frame_sim` | 80-tick fall → walk → jump; pin at 192 and snap to row 8 |

Shared code: [`common/maoliao_core.h`](common/maoliao_core.h) /
[`common/maoliao_core.cpp`](common/maoliao_core.cpp).

These are **not** a rewrite of the game. Drawing, MCI, and the home/pause GDI
menus stay in `MaoLiAo/`. If a constant changes in `define.h`, copy it here
too (see `docs/11-build.md`).
