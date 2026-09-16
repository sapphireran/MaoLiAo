# Physics sandbox

A Linux-friendly cut of `MaoLiAo/inertia.cpp` plus the numbers in `define.h`.

```bash
make -C examples/physics test
./examples/physics/physics_sandbox jump
./examples/physics/physics_sandbox friction
./examples/physics/physics_sandbox rail
```

| File | What it is |
| --- | --- |
| `inertia_portable.h` | `X = vt + ½at²`, `v += at` |
| `physics_constants.h` | screen, jump, friction, camera rail |
| `physics_sandbox.cpp` | CLI for the three worked examples in [docs/physics.md](../../docs/physics.md) |
| `test_physics.cpp` | assertions: closed form, discrete apex, `u`, left/right `k`, rail |

This does **not** link EasyX. It will not play the game. It will tell you whether a 4-tile wall is inside the 101 px jump (it is not).
