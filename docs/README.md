# MaoLiAo documentation

Personal notes for the 2020 EasyX course game. Read these if you are opening the project years later and want the *why*, not only the *what*.

| Doc | Topic |
| --- | --- |
| [00-overview.md](00-overview.md) | What the game is, what it is not |
| [01-architecture.md](01-architecture.md) | Classes, globals, and who owns what |
| [02-game-loop.md](02-game-loop.md) | Frame order in `main.cpp` |
| [03-physics.md](03-physics.md) | Inertia, friction, jump, speed cap |
| [04-collision.md](04-collision.md) | Four-corner AABB vs tiles / coins / food / enemies |
| [05-scene-and-maps.md](05-scene-and-maps.md) | Tile IDs, parallax, sprite sheets |
| [06-roles-and-combat.md](06-roles-and-combat.md) | Hero, enemies, bullets, bombs |
| [07-input-and-ui.md](07-input-and-ui.md) | Command bits, home, pause, HUD |
| [08-audio.md](08-audio.md) | MCI aliases and when they fire |
| [09-save-system.md](09-save-system.md) | `gameRecord.dat` format |
| [10-levels.md](10-levels.md) | World layouts and end distances |
| [11-build.md](11-build.md) | Visual Studio, EasyX, portable examples |
| [12-constants-reference.md](12-constants-reference.md) | Every `#define` that matters |
| [13-player-guide.md](13-player-guide.md) | How to play the 2020 binary |
| [design-notes.md](design-notes.md) | Known quirks, later-self comments |

Runnable extracts of the math and rules: [`../examples/README.md`](../examples/README.md).
