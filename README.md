# 猫里奥 MaoLiAo

Personal EasyX / C++ side-scroller from a 2020 research-and-development course.
The playable hero is **猫里奥** (Cat Mario). This repository now also keeps
standalone notes and portable examples so the original Windows game can be
studied without opening Visual Studio.

```
  ┌──────────┐     key / mouse      ┌──────────┐
  │ Control  │ ───────────────────► │   Role   │  hero, enemies, bullets
  └──────────┘                      └────┬─────┘
                                         │ collision + camera origin
                                         ▼
                                    ┌──────────┐
                                    │  Scene   │  tiles, coins, food, sky
                                    └──────────┘
```

## What this repo contains

| Path | What it is |
| --- | --- |
| `MaoLiAo/` | Original Visual Studio 2019 EasyX game (v2.0) |
| `docs/` | Architecture, physics, map format, UI, assets, known bugs |
| `examples/` | Portable C++11 samples that replay the game math on Linux or macOS |

The game itself still needs **Windows + EasyX + Visual Studio**. The
`examples/` tree does **not**. Those programs re-implement only the
formulas and level tables, so they compile with a stock `g++`.

## Play (Windows)

1. Install [EasyX](https://easyx.cn/) and Visual Studio 2019 (toolset v142) or later.
2. Open `MaoLiAo.sln`.
3. Build **Debug | Win32** or **Debug | x64**.
4. Run from a working directory that can see `MaoLiAo/res/` (bitmaps + MP3s).

### Controls

| Action | Key |
| --- | --- |
| Move left / right | `A` / `D` |
| Jump | `W` or `K` |
| Shoot (after picking up the weapon) | `J` |
| Pause | `Esc` |

Pause menu: return, restart, quit to title, write `gameRecord.dat`.
Title menu: start, introduction, directions, exit, read save.

World 1 is grass / water. World 2 is ice platforms. World 3 is a
pipe-gap stage with unlimited jumps (a Flappy-Bird-style gauntlet).

## Study the math without EasyX

```bash
cd examples
make
make test
```

That builds ten small programs:

1. **inertia jump** — kinematic jump arc (`v = -√(2gH)`, `x = vt + ½at²`)
2. **friction run** — grass / ice / slick surface μ and stop distance
3. **AABB collision** — the four-vertex-in-rect test used by `Role`
4. **level preview** — ASCII dump of the authored World 1–3 tile lists
5. **command mask** — `CMD_LEFT | CMD_RIGHT | …` bit packing
6. **save roundtrip** — `gameRecord.dat` world-number format
7. **camera scroll** — how `x0` and parallax `xBg` move
8. **enemy patrol** — walk, turn at walls, turn at ledges
9. **bullet flight** — `MAX_DISTANCE` and tile hits
10. **score events** — coin / stomp / bullet points

See [examples/README.md](examples/README.md) and the notes under [docs/](docs/).

## Scoring (as implemented)

| Event | Points |
| --- | --- |
| Coin | +10 |
| Stomp an enemy | +5 |
| Bullet hits an enemy | +5 |

Lives start at 5 (`LIFE`). Falling below the screen or walking into an
enemy (without a downward `vY`) kills the hero. World 3 also kills on
most solid tiles unless the weapon pickup has been collected.

## License / origin

Course project, 2020. Personal archive for Sapphire Ran (`sapphireran`).
Not a company codebase. Art and audio live under `MaoLiAo/res/`.
