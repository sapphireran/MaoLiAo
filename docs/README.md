# Personal reference kit

These notes are a personal study archive of the 2020 猫里奥 homework tree. They describe **this checkout**, not a rewritten engine.

| Note | What it covers |
|---|---|
| [architecture.md](architecture.md) | Classes, globals, one-frame order in `main.cpp` |
| [physics.md](physics.md) | `Inertia::move`, jump, friction `u`, camera, sky parallax |
| [collision-and-combat.md](collision-and-combat.md) | Four-corner hit test, pipes, coins, food, bullets, score |
| [input-ui-save.md](input-ui-save.md) | `CMD_*` / `VIR_*`, home / pause, `gameRecord.dat` |
| [levels.md](levels.md) | Worlds 1–3 tables, ending distances, `MAP_NUMBER` clip |
| [assets.md](assets.md) | `res/` BMP measurements and which row each blit uses |
| [quirks.md](quirks.md) | Guard typo, off-by-one loops, spawn `y = X0`, world-2 flag clip |
| [build-windows.md](build-windows.md) | EasyX + Visual Studio |

Portable replays of the same numbers live in [`../examples/`](../examples/README.md).
