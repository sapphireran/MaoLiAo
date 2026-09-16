# 猫里奥 MaoLiAo

Personal 2020 course project: a three-world EasyX side-scroller written in C++.
The playable game still lives under `MaoLiAo/` and needs Visual Studio + EasyX on Windows.

This branch adds the notes and headless examples I wish I had left myself in 2020.
Nothing here rewrites the original gameplay. The Windows binary is unchanged.

| Want | Go here |
| --- | --- |
| How the loop, camera, and classes fit | [docs/architecture.md](docs/architecture.md) |
| Jump, friction, and `Inertia::move` | [docs/physics.md](docs/physics.md) |
| Hitboxes, tiles, and the scroll rail | [docs/collision-and-camera.md](docs/collision-and-camera.md) |
| Worlds 1–3, tiles, pickups, routes | [docs/worlds.md](docs/worlds.md) |
| Keys, pause menu, `gameRecord.dat` | [docs/input-ui-saves.md](docs/input-ui-saves.md) |
| 2020 bugs that still ship in V2.0 | [docs/quirks.md](docs/quirks.md) |
| Constant / tile / score tables | [docs/reference-tables.md](docs/reference-tables.md) |
| Headless kit, maps, replay, C++ header | [examples/README.md](examples/README.md) |

## Play (Windows)

1. Install [EasyX](https://easyx.cn/) and Visual Studio 2019+ with the C++ desktop workload.
2. Open `MaoLiAo.sln`.
3. Build **Debug \| Win32** (the Debug configs use the MultiByte charset the source assumes).
4. Run with the working directory on `MaoLiAo/` so `res\*.bmp` and `res\*.mp3` resolve.

| Key | Action |
| --- | --- |
| A / D | Walk |
| W or K | Jump (world 3 allows air jumps) |
| J | Shoot, after the flower |
| Esc | Pause: resume, restart, home, save |

Five lives. Coins are 10 points. Stomps and bullets are 5. The flower only unlocks shooting.

## Headless check (Linux, no EasyX)

```bash
cd examples && make check
```

That builds the C++ kit tests, runs the Python suite, and writes ASCII / SVG maps
under `examples/out/`. See [examples/README.md](examples/README.md).

## Layout

```
MaoLiAo/           original V2.0 sources, resources, and gameRecord.dat
docs/              personal notes extracted from those sources
examples/          portable kit, catalog, demos, and check suite
```

Original author credit in the in-game intro: PWB. This GitHub copy is a personal archive.
