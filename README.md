# 猫里奥 MaoLiAo

A personal C++ / EasyX side-scroller from a 2020 research-and-development practice course. The playable hero is **猫里奥** (Cat Mario). The game is a three-world platformer: two grounded stages plus a Flappy-style pipe run.

This repository now also ships **personal documentation** and **portable examples** that reconstruct the original formulas (inertia, collision, camera, scoring, map tiles) without requiring Visual Studio or EasyX. The Windows game itself still builds with the Visual Studio solution; the examples build with any C++17 compiler.

```
MaoLiAo/          Windows EasyX game (VS 2019, toolset v142)
docs/             Architecture, physics, levels, input, assets, quirks
examples/         Portable C++ model + runnable demos + Python helpers
```

## Play (Windows)

1. Install [EasyX](https://easyx.cn/) for Visual Studio.
2. Open `MaoLiAo.sln`.
3. Build **Debug | Win32** or **Debug | x64** (the project uses the multi-byte character set in Debug).
4. Run with the working directory set so `res\` bitmaps and MP3s resolve next to the executable.

The window is **512×384**. Title bar reads `猫里奥 V2.0`.

## Controls

| Action | Keys |
| --- | --- |
| Move left / right | `A` / `D` |
| Jump | `W` or `K` |
| Shoot (after picking up the weapon) | `J` |
| Pause | `Esc` |

On the pause overlay: return, restart the current world, quit to the title menu, or write the current world index into `gameRecord.dat`. The title screen can start a new run, show the in-game introduction, show the control guide, exit, or read that save file.

World 3 allows **air jumps** on every `W`/`K` press. Worlds 1–2 only jump while grounded.

## Scoring

| Event | Points |
| --- | --- |
| Coin | +10 |
| Stomp an enemy (falling onto them) | +5 |
| Shoot an enemy | +5 |

Lives start at **5**. Falling below the screen, walking into an enemy while not falling, or (in world 3) touching a solid pipe without the weapon flag kills the hero.

## Source map

| File | Role |
| --- | --- |
| `MaoLiAo/main.cpp` | Window, music aliases, main loop, life / world transitions |
| `MaoLiAo/control.cpp` | Async key bits, title / pause UI, HUD, save / load |
| `MaoLiAo/role.cpp` | Hero, enemies, bullets, AABB hits, jump / run integration |
| `MaoLiAo/scene.cpp` | Tile maps, coins, food, parallax background |
| `MaoLiAo/inertia.cpp` | `x = v t + ½ a t²` step used by run and jump |
| `MaoLiAo/timer.h` | `QueryPerformanceCounter` sleep that keeps the 10 ms tick honest |
| `MaoLiAo/define.h` | Screen size, gravity, friction times, command bits |

Forward-declared `Role` / `Scene` pointers keep the two headers from including each other. The `Map` struct is duplicated behind `#ifndef _MAP` in both `role.h` and `scene.h`.

## Documentation

- [Architecture](docs/architecture.md) — objects, coordinates, and who owns what
- [Game loop](docs/game-loop.md) — the `while (true)` in `main.cpp`, death, and clear
- [Physics](docs/physics.md) — gravity, jump apex, run acceleration, tile friction
- [Collision](docs/collision.md) — inset AABB tests for tiles, coins, food, enemies
- [Levels](docs/levels.md) — tile IDs, world layouts, win distances
- [Input and save](docs/input-and-save.md) — command bits, pause menu, `gameRecord.dat`
- [Assets](docs/assets.md) — bitmaps, sprite sheets, MCI aliases
- [Known quirks](docs/known-quirks.md) — header typos, map overflow, world-3 off-by-one
- [Verified numbers](docs/verified-numbers.md) — apex, friction, rail, scores from `make test`

## Portable examples

The EasyX game is Windows-only. `examples/` is a **header-only reconstruction** of the numbers and rules, plus small programs that print trajectories, collide boxes, decode key bits, dump maps, and score pickups.

```bash
cd examples
make
make test
```

See [examples/README.md](examples/README.md) for the program list and what each one asserts.

## Worlds at a glance

| World | Feel | Win distance | Notes |
| --- | --- | --- | --- |
| 1 | Grass / mixed ground | 94 tiles (`3008` px) | Classic walk, pipes, coins, one weapon |
| 2 | Snow platforms | 104 tiles (`3328` px) | Taller jumps between clouds and ledges |
| 3 | Pipe corridor | 94 tiles (`3008` px) | Random gap heights, air jump, contact kills |

Camera: the hero is clamped between `x = 0` and `x = 192`. Past the right rail, `hero.x0` (the world origin) slides left so the map scrolls. The sky bitmap drifts at `1/5` of the map scroll (`K_MAP_BG`).

## History

Course project, 2020. Later personal passes added the third world, UI polish, an application icon, and comment cleanup. This docs/examples pass is personal reference material only — it does not change the EasyX gameplay code.
