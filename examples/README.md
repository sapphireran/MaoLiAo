# Portable examples

Eight small C++11 programs that replay **猫里奥** math and tables
without EasyX, Windows, or the BMP/MP3 assets.

```
examples/
  common/          constants, integrator, AABB, maps, save, camera, patrol
  01_inertia_jump/ standing jump trajectory
  02_friction_run/ grass / ice / slick stops
  03_collision_aabb/ four-vertex hit test
  04_level_preview/ ASCII worlds + authored vs MAP_NUMBER cap
  05_command_mask/ CMD_* / VIR_* bits
  06_save_roundtrip/ gameRecord.dat
  07_camera_scroll/ XRIGHT window + 1/5 sky parallax
  08_enemy_patrol/ ledge / wall turns
  testdata/        checked-in reference dumps
```

## Build / test

Needs a C++11 compiler (`g++` or `clang++`).

```bash
cd examples
make          # binaries in examples/build/
make test     # run all; exit 1 if a self-check fails
make clean
```

Each program prints a short trace and a `N passed, 0 failed` line.
They are meant to be read next to the matching page in `docs/`.

| Program | Doc |
| --- | --- |
| 01, 02, 07 | [docs/physics.md](../docs/physics.md) |
| 03, 08 | [docs/architecture.md](../docs/architecture.md) |
| 04 | [docs/map-format.md](../docs/map-format.md) |
| 05, 06 | [docs/controls-and-ui.md](../docs/controls-and-ui.md) |

## What is copied vs invented

Copied from the 2020 game:

- every `#define` used by motion and input (`common/constants.hpp`)
- `Inertia::move`
- `Role::isHit` + pipe 2× scaling
- world 1 / 2 `Map` / coin / enemy tables
- `gameRecord.dat` as a single integer
- camera window `[0, 192]` and `K_MAP_BG = 5`

Not copied:

- EasyX blits, MCI, the title mouse loop
- world 3 `rand()` pipes (only the cloud runway + goal are listed)
- the off-by-one `<= sizeof` writes — examples use exact sizes

If a check fails after you edit `MaoLiAo/define.h` or `createMap`,
update the matching `common/` file first, then the assertion.
