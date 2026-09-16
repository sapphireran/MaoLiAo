# Personal notes for 猫里奥

These pages are a field notebook for the 2020 EasyX build. Every constant, formula,
and table is taken from the current `MaoLiAo/` sources. If a sentence disagrees with
the code, the code wins — file an issue against the note, not against the binary.

## Reading order

1. [build-and-play.md](build-and-play.md) if you want to launch the Windows game.
2. [architecture.md](architecture.md) for the object graph and the 10 ms tick.
3. [physics.md](physics.md) and [collision.md](collision.md) for how the cat moves.
4. [tiles.md](tiles.md) and [levels.md](levels.md) for what each world is made of.
5. [input-and-ui.md](input-and-ui.md) and [scoring.md](scoring.md) for menus and points.
6. [known-issues.md](known-issues.md) before you copy a loop from `scene.cpp`.

The portable reconstructions in `../examples` follow the same order: inertia first,
then collision, friction, commands, save format, pipe gaps, camera, and scoring.

## Source map

| Note | Primary sources |
| --- | --- |
| Architecture | `main.cpp`, `role.h`, `scene.h`, `control.h` |
| Physics | `define.h`, `inertia.cpp`, `Role::action` |
| Collision | `Role::isHit`, `hitMap`, `hitCoins`, `hitFood`, `hitEnemy` |
| Tiles | `Scene::createMap`, `Scene::show` |
| Levels | `createMap`, `createCoin`, `createFood`, `Role::createEnemy` |
| Input / UI | `control.cpp`, command macros in `define.h` |
| Scoring | `Role::action`, `Role::bullteFlying`, `main.cpp` lives |
| Known issues | include guard in `define.h`, `sizeof` loops, `LIFE` macro |

## Units

The game mixes three unit systems on purpose:

- **Screen pixels.** The window is 512×384. A tile is 32×32.
- **Tile coordinates.** Map records store `{x, y, id, xAmount, yAmount, u}` in tiles.
- **“Real meters.”** Jump height is authored as `REAL_HEIGHT = 3.5` meters, then
  scaled onto `UNREAL_HEIGHT = 101` pixels so a hop reads as about three tiles.

Time is fixed at `TIME = 0.01` seconds per tick (100 Hz), driven by `Timer::Sleep`.
