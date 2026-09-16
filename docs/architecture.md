# Architecture

V2.0 is four objects plus two globals, stepped on a fixed `TIME = 0.01` s tick.

```
main
 ├─ Control   input, home / pause / HUD, save file
 ├─ Role      hero, enemies, bullets, bombs, score
 ├─ Scene     tiles, coins, flower, parallax sky
 └─ Timer     QueryPerformanceCounter sleep
```

Globals in `main.cpp`:

- `life` starts at `LIFE` (the macro is `#define LIFE 5;`, so the declaration is `int life = 5;`).
- `world` is `1`, `2`, or `3`. Pause-save writes only this integer.

## Startup

`main` opens a 512×384 EasyX window titled `猫里奥 V2.0`, shows `Control::gameStart()`,
then constructs `Role(world)` and `Scene(world)` and opens the MCI aliases
(`music_bg`, `music_win`, `music_passedAll`, `music_end`). Role itself opens the
SFX aliases (death, jump, coin, stomp, weapon, bullet, two boom sounds).

## Tick

Each iteration of the `while (true)` loop:

1. `key = gameCtrl.getKey()` — OR of `GetAsyncKeyState` bits, plus Esc opening the pause overlay.
2. `VIR_RESTART` rebuilds the current world. `VIR_HOME` resets `life` and `world` and returns to the title.
3. `gameRole.action(key, &gameScene, world)` — jump, walk, camera rail, pickups, enemy patrols, death.
4. `gameScene.action(&gameRole)` — copy `hero.x0` into `xMap`; slide the sky when the hero is pinned at `XRIGHT`.
5. If `isDied()`: freeze a frame, wait 3.5 s, decrement `life`. Zero lives plays the game-over strip and resets to world 1. Otherwise `showDied(life)` and respawn the same world.
6. If `isPassed()`: world 3 plays the clear fanfare and returns to the title; worlds 1–2 increment `world` and rebuild.
7. `BeginBatchDraw` / `scene.show` / `role.show` / score / level / `EndBatchDraw`.
8. `Timer::Sleep((int)(TIME * 1000))` — 10 ms, with a leftover-compensation clock so the tick does not drift as fast as a naive `Sleep`.

There is no pause flag on the simulation itself. Esc blocks inside `pauseClick()` on `GetMouseMsg()`.

## Ownership

`Role` keeps a `Scene*` written at the start of `action` and reused from `show` when bullets fly.
`Scene` never owns a `Role`; `action` only reads `Hero::x0` and `vX`.

`Map` is defined in both `scene.h` and `role.h` behind `#ifndef _MAP`. The two copies must stay identical.

## Coordinates

Three x values travel with the hero:

| Field | Meaning |
| --- | --- |
| `x` / `xx` | On-screen pixel position, pinned to `[XLEFT, XRIGHT]` = `[0, 192]` until the ending rail. |
| `x0` | Camera origin. Starts at 0 and goes **negative** as the stage scrolls right. |
| world x | `-x0 + x`. Used for walk-cycle frames, ending distance, and enemy blit. |

Y is screen-down. Jumping decreases `y`. Falling off `YSIZE` (384) is a death.

## Class map

```
Control
  GetCommand()          A/D/W|K/S/J/Esc → bitset
  getKey()              latches last bitset; Esc → pauseClick
  gameStart()           title, intro, help, exit, load
  pauseClick()          resume / restart / home / fwrite world
  showScore/Level/Died/GameOver/Passed/PassedAll

Role
  createEnemy(world)
  action / show
  hitMap / hitCoins / hitFood / hitEnemy / isHit
  setBomb / setBullet / bullteFlying   (typo is original)

Scene
  createMap / createCoin / createFood
  isEnding(distance)    94 / 104 / 94 tiles
  action                camera + sky
  show                  sky, tiles, coins, score puffs, flower

Inertia::move(v, t, a)  x = vt + ½at²; v += at
Timer::Sleep(ms)        QPC busy-wait remainder
```

## What the examples copy

`examples/kit` and `examples/cpp/maoliao_kit.hpp` reimplement the numbers and
the hero tick without EasyX, MCI, or Win32. They are a notebook, not a port
of the renderer.
