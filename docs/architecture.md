# Architecture

MaoLiAo is a single-window EasyX application. There is no engine, scene graph, or resource manager beyond raw `IMAGE` handles and `mciSendString` aliases. Four types own the runtime:

```
main.cpp
  ├─ Control   input + title/pause/HUD/transitions
  ├─ Role      hero, enemies, bullets, bombs, scoring, collision queries
  ├─ Scene     tile map, coins, food, parallax sky, scenery frames
  └─ Timer     QueryPerformanceCounter sleeper (header-only)
         │
         └─ Inertia::move   kinematic step used by Role
```

`Role` and `Scene` know about each other through forward declarations. `Role` stores a `Scene*` so bullets can query tiles after `action()` has run. `Scene::action` only reads `Hero::x0` / `vX` to scroll the sky.

## Source files

| File | Responsibility |
| --- | --- |
| `main.cpp` | `initgraph(512, 384)`, music aliases, the infinite game loop |
| `define.h` | Screen, tile, physics, and command-bit macros |
| `control.cpp` / `control.h` | Async keyboard bits, pause menu, title screens, HUD |
| `role.cpp` / `role.h` | Hero integration, enemies, bullets, AABB hits |
| `scene.cpp` / `scene.h` | Per-world tile / coin / food tables and drawing |
| `inertia.cpp` / `inertia.h` | `X = v t + ½ a t²`, then `v += a t` |
| `timer.h` | Multimedia-timer-style `Sleep(ms)` using QPC |
| `MaoLiAo.rc` / `resource.h` | Application icon `IDI_ICON1` |

`timer.h` both declares and defines `Timer`. It is included only from `main.cpp`, so there is no ODR clash today.

## Game loop

The loop in `main()` is the entire scheduler. One iteration is one frame of `TIME` seconds (0.01 s, i.e. a 100 Hz design rate):

```
while (true):
    key = Control.getKey()          # sticky last-command + Esc pause
    if VIR_RESTART: rebuild Scene + Role for the current world
    if VIR_HOME:    reset lives/world, show title, rebuild
    Role.action(key, Scene, world)  # integrate hero, AI, pickups
    Scene.action(Role)              # parallax sky from hero.x0 / vX
    if Role.isDied():
        freeze a frame, decrement life, showDied or showGameOver
    if Role.isPassed():
        world++ or showPassedAll and return to title
    BeginBatchDraw(); Scene.show(); Role.show(); HUD; EndBatchDraw()
    Timer.Sleep(TIME * 1000)
```

Death and clear paths stop the looping BGM, play a sting, sleep for a hard-coded number of milliseconds, then reconstruct `Role` and `Scene` by assignment (`gameRole = Role(world)`). There is no object pool.

## Coordinate systems

Three X values travel together on `Hero`:

| Field | Meaning |
| --- | --- |
| `x`, `xx` | On-screen pixel position (int + double). Clamped to `[0, 192]` until the ending auto-walk. |
| `x0` | Camera / map origin. Negative as the world scrolls right. `Scene.xMap = (int)hero.x0`. |
| world X | `-x0 + x` — used for the ending trigger and walk-cycle frame. |

Y is screen-down, origin at the top of the 384 px window. Tile space is `WIDTH = HEIGHT = 32`. Most map / coin / enemy tables are authored in tile units and multiplied by 32 when spawned. Food on worlds 1–2 is authored in pixels.

## Ownership and lifetimes

- `Control` lives for the process and keeps one `IMAGE` of `home.bmp`.
- `Role` and `Scene` are value objects rebuilt on death, restart, world change, and title return.
- MCI aliases (`music_bg`, `music_jump`, …) are opened in `main` or `Role::Role` and never closed until the unreachable `closegraph()` after the infinite loop.
- Global `int life` and `int world` in `main.cpp` are `extern` in `control.cpp` so the pause-menu save and title-screen load can read/write the current world.

## Collision module (inside Role)

All hit tests share one helper: `isHit(p1[4], p2[2])` treats the actor as four inset vertices and the target as an axis-aligned rectangle. Queries:

| Function | Target | Notes |
| --- | --- | --- |
| `hitMap` | tiles with `1 <= id < 11` | IDs 11–14 are scenery only. IDs 8 and 10 use a 2×2 tile AABB (pipes). World 3 kills on any non-cloud solid unless `isShoot`. |
| `hitCoins` | `POINT` in tile units | Clears the coin to `{0,0}` after a hit. |
| `hitFood` | `POINT` in pixels | Enables `Hero.isShoot`. |
| `hitEnemy` | `Enemy` with `turn != 0` | Stomp if `vY > 0` during a fall; otherwise death if `vY <= 0`. |

Bullets reuse `hitMap` / `hitEnemy` and cap travel at `MAX_DISTANCE = 480` screen pixels.

## Drawing

EasyX has no alpha channel in this project. Sprites are blitted twice: `SRCAND` of a mask row, then `SRCPAINT` of the color row (`role.bmp`, `ani.bmp`, `scenery.bmp`). Tiles from `map.bmp` are opaque strips selected by `id`. The sky (`mapsky.bmp`) is a vertical atlas: world *n* uses scanlines `[(n-1)*384, n*384)`.

`BeginBatchDraw` / `EndBatchDraw` wrap the playfield. The pause and title UIs mix `FlushBatchDraw` with live mouse-move redraws.

## What is *not* here

- No JSON/XML level files. Worlds are C++ aggregate literals in `Scene::createMap` / `createCoin` / `createFood` and `Role::createEnemy`.
- No fixed-timestep accumulator. `Timer::Sleep` aims at 10 ms but hitches stretch the next frame; physics still steps exactly `TIME`.
- No separate audio mixer. Every cue is an MCI alias; overlapping plays use `play … from 0`.
