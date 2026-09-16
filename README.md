# 猫里奥 MaoLiAo

Personal EasyX side-scroller written in C++ for a 2020 research-and-development practice course.
The playable game stays a Visual Studio / Windows project. This repository now also keeps
portable notes and command-line examples that reconstruct the same numbers the game uses.

## What it is

Cat Mario (猫里奥) is a three-world platformer:

| World | Theme | Feel |
| --- | --- | --- |
| 1 | Grass, pipes, water, clouds | Classic walk / jump / stomp |
| 2 | Snow platforms and trees | More vertical hops |
| 3 | Random pipe gaps | Flappy-style flight, mid-air jumps |

The hero starts with five lives, can pick up a flower that unlocks shooting, and can save
the current world index to `MaoLiAo/gameRecord.dat`.

## Repository layout

```
MaoLiAo.sln                 Visual Studio 2019 solution
MaoLiAo/                    Playable EasyX game (Windows)
  main.cpp                  Game loop, music, life / world transitions
  control.*                 Title, pause, HUD, save / load
  role.*                    Hero, enemies, bullets, collision
  scene.*                   Maps, coins, food, camera
  inertia.*                 Constant-acceleration displacement
  define.h                  Screen, physics, and command constants
  timer.h                   High-resolution Sleep
  res/                      Bitmaps and MP3 cues
docs/                       Personal design notes extracted from the source
examples/                   Portable C++ reconstructions you can compile on Linux
```

The game itself is not being rewritten here. Docs and examples only explain, and then
re-implement in isolation, the formulas already sitting in `define.h`, `inertia.cpp`,
`role.cpp`, and `scene.cpp`.

## Play the Windows build

See [docs/build-and-play.md](docs/build-and-play.md). Short version:

1. Install Visual Studio with C++ desktop tools.
2. Install [EasyX](https://easyx.cn/).
3. Open `MaoLiAo.sln`, set the working directory to `MaoLiAo/`, build, run.

Controls: `A` / `D` move, `W` or `K` jump, `J` shoot (after the flower), `Esc` pause.

## Read the notes

- [Architecture](docs/architecture.md) — objects, loop, camera
- [Physics](docs/physics.md) — jump, gravity, friction, inertia
- [Collision](docs/collision.md) — inset vertices versus tile AABBs
- [Tiles](docs/tiles.md) — map IDs 1–14
- [Levels](docs/levels.md) — world layouts and spawn tables
- [Input and UI](docs/input-and-ui.md) — command bits, menus, save file
- [Scoring](docs/scoring.md) — coins, stomps, bullets, lives
- [Known issues](docs/known-issues.md) — include-guard typo and off-by-ones

## Run the portable examples

The examples folder does not link EasyX. It compiles with a plain C++17 toolchain:

```bash
cd examples
make test
```

That rebuilds the demos and runs the assertion-based suite. Details live in
[examples/README.md](examples/README.md).

## Personal scope

This tree is a personal course project. Documentation and examples here stay inside
this game: no company code, no third-party product work.
