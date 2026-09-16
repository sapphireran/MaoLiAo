# Architecture

MaoLiAo is a single-process EasyX game. There is no scene graph, ECS, or
resource manager: four concrete classes plus a handful of globals own
everything. That is enough for three hand-authored worlds and a 10 ms tick.

## Modules

```
                    ┌────────────┐
                    │  main.cpp  │  life, world, MCI aliases
                    └─────┬──────┘
           ┌──────────────┼──────────────┐
           ▼              ▼              ▼
      Control         Role           Scene
      menus           hero           tiles
      HUD             enemies        coins / food
      save/load       bullets        parallax
           │              │              │
           └──────────────┼──────────────┘
                          ▼
                      Inertia::move
                      Timer::Sleep
                      define.h macros
```

| Unit | Responsibility | Windows-only? |
| --- | --- | --- |
| `main.cpp` | `initgraph`, music aliases, life/world state machine | Yes (EasyX + MCI) |
| `Control` | Async key bits, home/pause UI, score/level text | Yes |
| `Role` | Hero kinematics, enemy patrol, bullets, hits | Yes (draw + sound) |
| `Scene` | Tile list, coin/food points, camera follow | Yes (blit) |
| `Inertia` | One kinematic step | No (pure math) |
| `Timer` | High-resolution sleep | Yes (`QueryPerformanceCounter`) |
| `define.h` | Screen, tile, physics, command constants | No |

`Role` and `Scene` forward-declare each other. They share a duplicated
`struct Map` behind the `_MAP` include guard so either header can be
included first.

## Ownership

`main` constructs one `Control`, one `Role`, and one `Scene`. Restart,
home, death, and clear **assign** new `Role`/`Scene` objects rather than
calling a reset method:

```cpp
gameRole  = Role(world);
gameScene = Scene(world);
```

`Role::action` stores a raw `Scene*` for the current tick so `show()` can
advance bullets against the same map. The pointer is not owned.

Globals in `main.cpp`:

| Name | Type | Meaning |
| --- | --- | --- |
| `life` | `int` | Remaining lives. Starts at `LIFE` (intended 5). |
| `world` | `int` | 1, 2, or 3. `Control` reads this via `extern` for save/load. |

`LIFE` is defined as `#define LIFE 5;` — the trailing semicolon is
swallowed by `int life = LIFE;` but would break other uses. See
[known-issues.md](known-issues.md).

## Data flow for one tick

1. `Control::getKey()` — `GetAsyncKeyState` bits, optional pause menu.
2. `Role::action(key, &scene, world)` — jump, walk, shoot, enemy patrol,
   coin/food/enemy tests, ending/passed flags.
3. `Scene::action(&role)` — `xMap = hero.x0`; parallax `xBg` when the
   hero is pinned at `XRIGHT`.
4. Death / clear handlers in `main` (music, delay, life--, world++).
5. `BeginBatchDraw` → `scene.show()` → `role.show()` → HUD →
   `EndBatchDraw`.
6. `Timer::Sleep(TIME * 1000)` — 10 ms target.

Drawing is a **second pass**. `Role::show` also steps bullets
(`bullteFlying`) so projectile motion is tied to the present call, not
`action`. That is why a paused death pose still needs a show before the
3.5 s delay.

## Coordinate systems

Three x-spaces appear in the code. Mixing them is the usual source of
collision bugs.

| Space | Origin | Who stores it |
| --- | --- | --- |
| Screen | Window left, pixels | `Hero.x`, `Hero.y` |
| World / camera | Scroll offset | `Hero.x0` (negative as you walk right), `Scene.xMap` |
| Tile | Grid cell | `Map.x`, `Map.y`, coin `POINT`s (world 1/2) |

World X of the hero is `hero.x - hero.x0` (because `x0` goes negative).
`Scene::isEnding` compares that distance to `94 * WIDTH` or
`104 * WIDTH`.

Tile solids use **grid** coordinates times `WIDTH`/`HEIGHT` (32 px).
World-1 food is stored in **pixels**. World-3 food is stored as a tiny
`{10,10}` pixel pair. Hit tests must use the same unit the creator wrote.

Y grows **downward**. Jump velocity is negative. Gravity `G` is a
positive scalar added to `vY` each step.

## Sprite / mask blit

Walk, death, coins, food, enemies, bombs, and scenery use EasyX's
`SRCAND` + `SRCPAINT` pair against a mask row and a color row in the
same BMP. Frame index is an accumulating `*_iframe` double, then cast
to `int` to pick a column.

Hero walk frames flip every `STEP` (10) pixels of world travel so the
gait stays tied to distance, not wall-clock time.

## Audio

`main` and `Role` open MCI aliases (`music_bg`, `music_jump`, …) from
`res\\*.mp3`. Aliases are process-global strings, not RAII objects.
`close all` is after the infinite loop and therefore never reached.

## What the portable examples keep

`examples/include/maoliao_core.hpp` copies:

- the numeric constants (without the broken `F` / `LIFE` semicolons)
- `Inertia::move`
- AABB vertex-in-box
- command bit packing
- friction `u` from tile id
- world-1 tile / coin / enemy tables
- jump and camera pin rules
- `gameRecord.dat` integer format

It does **not** copy EasyX, MCI, or the pause GUI.
