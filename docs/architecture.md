# Architecture

MaoLiAo is a small EasyX Win32 loop, not an engine. Four types plus two helpers cover the whole game.

```
                    +------------------+
                    |     main.cpp     |
                    |  life, world     |
                    |  MCI aliases     |
                    +--------+---------+
                             |
           +-----------------+-----------------+
           |                 |                 |
           v                 v                 v
     +-----------+     +-----------+     +-----------+
     |  Control  |     |   Role    |     |   Scene   |
     | keys, UI  |     | hero/NPC  |     | map/coins |
     +-----------+     +-----+-----+     +-----+-----+
                             |                 |
                             +--------+--------+
                                      |
                                      v
                               +------------+
                               |  Inertia   |
                               |  Timer     |
                               +------------+
```

## Source map

| File | Responsibility |
| --- | --- |
| `MaoLiAo/main.cpp` | `initgraph(512, 384)`, title bar, construct objects, infinite loop, death / clear transitions |
| `MaoLiAo/control.h/.cpp` | Async keyboard bits, pause overlay, title screens, score / level HUD, death splash |
| `MaoLiAo/role.h/.cpp` | `Hero`, `Enemy[]`, `Bullet[]`, bombs, score, hit tests, shooting |
| `MaoLiAo/scene.h/.cpp` | Tile list, coins, food, sky scroll, animated scenery |
| `MaoLiAo/inertia.h/.cpp` | Static `move(v, t, a)` integrator |
| `MaoLiAo/timer.h` | Header-only QPC sleep |
| `MaoLiAo/define.h` | Screen size, timestep, speeds, command bits |
| `MaoLiAo/resource.h`, `MaoLiAo.rc` | Window icon `IDI_ICON1` |

`Role` and `Scene` forward-declare each other. `Map` is defined in both headers behind `#ifndef _MAP` so either include order works.

## Main loop

From `main.cpp`, after `gameCtrl.gameStart()` returns:

1. `key = gameCtrl.getKey()` — may be a held WASD/JK mask or a virtual pause result (`VIR_RESTART`, `VIR_HOME`).
2. Restart or return-to-title reconstructs `Role` and `Scene` for the current `world`.
3. `gameRole.action(key, &gameScene, world)` — integrate hero, move enemies, collect, shoot.
4. `gameScene.action(&gameRole)` — copy `hero.x0` into `xMap` and nudge the sky.
5. If `isDied()`: freeze a frame, wait 3.5 s, decrement `life`. At 0, play game-over, reset to world 1 and the title. Otherwise show remaining lives and rebuild the current world.
6. If `isPassed()`: play victory music. World 3 goes to the all-clear splash and title. Worlds 1–2 increment `world` and rebuild.
7. `BeginBatchDraw` → `scene.show()` → `role.show()` → HUD → `EndBatchDraw`.
8. `gameTimer.Sleep((int)(TIME * 1000))` → 10 ms.

The `while (true)` never reaches `closegraph()`. Closing the window is the OS killing the process.

## Two coordinate spaces

The hero is pinned to a **screen** box `[XLEFT, XRIGHT]` = `[0, 192]`. World motion is stored on `Hero::x0` (named “origin” in comments).

- Screen position: `Hero::x`, `Hero::y` (also kept as doubles `xx`, `yy`).
- World / camera offset: `Hero::x0`. When the hero tries to walk past `XRIGHT` and the level is not in the auto-run ending, `x0` decreases by the overflow and `x` is clamped. That makes the map draw at `xMap + tileX * 32`.
- World X used for “have we reached the flag?” is `-(int)x0 + x`.

`Scene::action` sets `xMap = (int)hero.x0` and, when the hero is glued to the right rail and still moving right, shifts the looping sky by a slower parallax (`K_MAP_BG = 5`).

Enemies and coins are stored in **world pixels** (enemies) or **tile indices** (coins). Drawing adds `xMap` / `x0` so they slide with the camera.

## Object lifetimes

`Role` and `Scene` are values on `main`’s stack. Restart / next world / death uses assignment:

```cpp
gameScene = Scene(world);
gameRole  = Role(world);
```

Each constructor reloads bitmaps and (for `Role`) re-opens MCI aliases. Destructors are empty; MCI handles are not closed per reconstruction. `main` only `mciSendString("close all")` on the unreachable path after the loop.

## Tick rate

`TIME` is `0.01` seconds. Almost every animation frame counter is `iframe += TIME * k`. Enemy patrols only step when `(int)(enemy_iframe * 100) % 2 == 0`, so they update on even hundredths of the walk cycle.

`Timer::Sleep` accumulates a QPC deadline so a slow frame shortens the next sleep instead of drifting unboundedly — until a frame overruns, at which point `m_oldclk` snaps forward.

## Music ownership

Background / win / all-clear / game-over aliases are opened in `main`. Jump, coin, stomp, death, weapon, bullet, and boom aliases are opened in `Role`’s constructor. Overlapping `open` calls after a reconstruct can leave stale aliases; the play commands use `from 0` to rewind.

## Why the split looks like this

The 2020 course version grew from “draw a map” + “move a sprite.” `Control` absorbed every modal screen. `Scene` absorbed every static placement. `Role` absorbed every moving actor and every hit test. There is no entity list, no scene graph, and no resource manager. That is enough for three short worlds and is the reason new content is still “append another `{x, y, id, w, h}` row.”
