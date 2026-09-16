# MaoLiAo documentation

Personal notes for the EasyX 猫里奥 sources under `MaoLiAo/`. Numbers and behaviors below are taken from the current C++ files, not from memory of an old course write-up.

## Reading order

| Doc | What it answers |
| --- | --- |
| [architecture.md](architecture.md) | Classes, globals, who owns what |
| [game-loop.md](game-loop.md) | `main` frame order, death, pass, audio |
| [constants-reference.md](constants-reference.md) | Every `#define` and pool size |
| [physics.md](physics.md) | `Inertia::move`, jump, friction, speed cap |
| [collision.md](collision.md) | Vertex-in-AABB tests, world-3 kill tiles |
| [levels.md](levels.md) | Tile ids, per-world maps, coins, food, enemies |
| [input-and-ui.md](input-and-ui.md) | Command bits, pause menu, title screens |
| [save-format.md](save-format.md) | `gameRecord.dat` |
| [sprites-and-audio.md](sprites-and-audio.md) | BMP atlases and MCI aliases |
| [build.md](build.md) | VS / EasyX build |
| [known-issues.md](known-issues.md) | Off-by-one loops, include-guard typo, map overflow |

Portable reconstructions live in `../examples/`.
