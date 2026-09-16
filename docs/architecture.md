# Architecture

MaoLiAo is a single-process EasyX game. One window, one loop, three cooperating objects.

The original 2020 coursework split the engine along the same lines a 2D platformer usually does: input and UI, the actor that can die, and the scrolling world those actors stand on.

```
                    +------------------+
                    |   main.cpp loop  |
                    |  100 Hz (TIME)   |
                    +--------+---------+
                             |
           +-----------------+-----------------+
           |                 |                 |
           v                 v                 v
    +------------+    +------------+    +-------------+
    |  Control   |    |    Role    |    |    Scene    |
    | input + UI |    | hero/enemy |    | map + camera|
    +------------+    +------------+    +-------------+
           |                 |                 |
           |                 +--------+--------+
           |                          |
           v                          v
    GetAsyncKeyState           Inertia::move
    pause / save / HUD         AABB vs tiles
```

## Source map

| File | Responsibility |
| --- | --- |
| `MaoLiAo/main.cpp` | Window setup, MCI music, life/world globals, the game loop |
| `MaoLiAo/define.h` | Screen size, physics constants, command bit flags |
| `MaoLiAo/control.cpp` | Home menu, pause menu, HUD, death/clear screens, save/load |
| `MaoLiAo/role.cpp` | Hero kinematics, enemies, coins, food, bullets, death |
| `MaoLiAo/scene.cpp` | Tile maps, coins/food placement, parallax draw |
| `MaoLiAo/inertia.cpp` | One kinematic step: `x = vt + ½at²`, then `v += at` |
| `MaoLiAo/timer.h` | Multimedia-timer busy wait used as the frame cap |

Headers include each other through forward declarations. `Role` holds a `Scene*`. `Scene::action` only reads the hero so the camera can follow.

## Game loop (one frame)

`main` is an infinite `while (true)` after `gameStart()` returns. Each tick:

1. `Control::getKey()` samples WASD / JK / Esc through `GetAsyncKeyState`. Esc opens the pause overlay and may return a virtual command (`VIR_RESTART`, `VIR_HOME`).
2. Restart rebuilds `Scene` and `Role` for the current `world`. Home resets `life` and `world` and returns to the title screen.
3. `Role::action(key, &scene, world)` integrates motion, resolves collisions, awards score, may set `died` or `passed`.
4. `Scene::action(&role)` copies `hero.x0` into `xMap` and eases `xBg` when the hero is pressed against the right camera rail.
5. If the hero died: freeze a frame, decrement `life`, show remaining lives or Game Over, then reconstruct the level.
6. If the hero passed: play the clear jingle. World 3 is the last stage; otherwise `world++` and rebuild.
7. `BeginBatchDraw` → `scene.show()` → `role.show()` → score/level HUD → `EndBatchDraw`.
8. `Timer::Sleep(TIME * 1000)` — 10 ms, so the design rate is 100 Hz.

Death and clear paths stop background music, sleep for several seconds, then restart BGM from the beginning.

## Object lifetimes

`Control`, `Role`, and `Scene` are stack objects in `main`. They are not leaked; they are **replaced** by assignment:

```cpp
gameScene = Scene(world);
gameRole  = Role(world);
```

Each reconstruction reloads bitmaps and re-opens MCI aliases. That is why death and stage changes hitch: constructors do I/O.

Globals `life` and `world` live in `main.cpp`. `control.cpp` reads `world` as `extern` so the pause menu can write `gameRecord.dat`.

## Coordinate systems

Three x-axes are in play at once. Mixing them up is the usual source of collision bugs.

| Space | Who stores it | Meaning |
| --- | --- | --- |
| Screen pixels | `Hero.x`, `Hero.y` | Top-left of the 32×32 sprite in the 512×384 window |
| Subpixel screen | `Hero.xx`, `Hero.yy` | Same origin, `double`, so integration does not quantize every frame |
| Camera origin | `Hero.x0` | How far the world has scrolled; usually `<= 0` |
| World pixels | `x_world = -x0 + x` | Used by `hitMap` / `hitCoins` |
| Tile indices | `Map.x`, `Map.y` | World pixels divided by `WIDTH`/`HEIGHT` (32) |

The hero is not allowed past `XLEFT = 0` or, until the ending rail, past `XRIGHT = 6 * 32 = 192`. Extra rightward motion is subtracted from `x0`, which slides the map left. Enemies and pickups are stored in world pixels or tile indices and drawn at `xMap + local`.

`y` is screen-down. Jumping therefore starts with a **negative** `vY`.

## Class notes

### Control

Pure UI and input. It owns the stacked `home.bmp` sheet and draws menus with EasyX rectangles plus `drawtext`. Hover swaps fill color and English labels; idle state uses Chinese. That bilingual hover is cosmetic only.

`getKey` latches the last command while a key is held. Because it only refreshes when `_kbhit()` is true, a key that is still physically down can keep producing the previous bitmask after `_kbhit` goes false. The loop is fast enough that this rarely matters.

### Role

Owns the hero, up to 30 enemies, 30 bullets, and 5 simultaneous explosion sprites. `createEnemy(world)` fills the enemy table. Collision helpers (`hitMap`, `hitCoins`, `hitFood`, `hitEnemy`) all build a slightly inset 4-corner hero quad and test it against an axis-aligned box.

Shooting is gated by `Hero.isShoot`, which becomes true after collecting the stage food/weapon sprite.

### Scene

`Map map[MAP_NUMBER]` with `MAP_NUMBER = 30` is the hard cap on collidable-or-drawn tiles. Worlds 1 and 2 author **more than 30** tiles in the initializer; extras are silently dropped. World 3 generates 30 tiles (7 pipe pairs plus a cloud runway and a goal sign).

Decorative ids `>= 11` are skipped by `hitMap` (`id < 11`), so grass, the goal plaque, water, and trees are scenery unless their id slipped into the collision range.

### Inertia

A static helper, not a simulation object. See [physics.md](physics.md).

## Music aliases

`main` opens looping BGM plus win / all-clear / game-over stingers. `Role` opens jump, coin, stomp, weapon, bullet, and death one-shots. Aliases are never closed until process exit (`mciSendString("close all")` is after the infinite loop, so it is unreachable). Reconstructing `Role` calls `open` again on the same alias names.

## What this folder does not change

The 2020 EasyX sources stay as they were submitted. Portable re-implementations of the math, maps, and AABB tests live under [`examples/`](../examples/README.md) so they can compile on Linux without `graphics.h`.
