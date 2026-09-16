# Architecture

`main.cpp` is a single-threaded loop around three objects plus a multimedia
timer. There is no scene graph, no ECS, and no resource manager beyond
`loadimage` / `mciSendString` at construction time.

```
                 ┌─────────────┐
                 │  initgraph  │  512×384, title 猫里奥 V2.0
                 └──────┬──────┘
                        │
              Control.gameStart()     home.bmp strip 0, mouse menu
                        │
         open MCI aliases, play music_bg repeat
                        │
        ┌───────────────┴────────────────┐
        │  while (true)                  │
        │    key = Control.getKey()      │  bits + pause overlay
        │    maybe reconstruct Role/Scene│  VIR_RESTART / VIR_HOME
        │    Role.action(key, scene, w)  │  move, shoot, die, pass
        │    Scene.action(role)          │  camera + parallax
        │    if died → lives / game over │
        │    if passed → world++ / end   │
        │    BeginBatchDraw              │
        │      scene.show(); role.show() │
        │      HUD score + level         │
        │    EndBatchDraw                │
        │    Timer.Sleep(TIME * 1000)    │  10 ms target
        └────────────────────────────────┘
```

## Objects

| Type | File | Lifetime | Job |
| --- | --- | --- | --- |
| `Control` | `control.cpp` | Whole process | Keys, menus, HUD, `gameRecord.dat` |
| `Role` | `role.cpp` | Rebuilt on death, restart, world change | Hero, enemies, bullets, bombs |
| `Scene` | `scene.cpp` | Same rebuild points as `Role` | Tiles, coins, food, sky, scenery |
| `Timer` | `timer.h` | Static QPC state | Busy-wait sleep |
| `Inertia` | `inertia.cpp` | Static helper | One kinematic step |

`Role` and `Scene` point at each other through a forward-declared `Role*` /
`Scene*`. The duplicated `struct Map` in both headers is gated by `_MAP` so
the type is defined once even though `define.h`'s own include guard is broken
(see [quirks.md](quirks.md)).

## Global state in `main.cpp`

```cpp
int life = LIFE;   // 5, from define.h (note the trailing semicolon in the macro)
int world = 1;     // also `extern int world` in control.cpp for save/load
```

`Control` reads and writes `world` when the pause menu saves or the home
menu loads. `life` is local to `main` and is **not** written to
`gameRecord.dat`. A save is only the current world index as ASCII.

## Coordinate spaces

Three x-axes exist at once. Mixing them is the usual source of “why is this
sprite two tiles off?” bugs.

| Space | Origin | Used by |
| --- | --- | --- |
| Screen pixels | Top-left of the 512×384 window | `Hero.x`, `Hero.y`, `putimage` |
| World pixels | `Hero.x0` (camera origin, ≤ 0 as you walk right) | Enemy `x`, coin tile × 32, `hitMap` |
| Tile cells | Integer grid, 32 px | `Map.{x,y,xAmount,yAmount}` |

Walking right past `XRIGHT` (192 px) does **not** move the cat further on
screen. `Role::action` subtracts the overflow from `x0` and pins `x` to
`XRIGHT`. `Scene::action` then copies `x0` into `xMap` and eases `xBg` by
`1/K_MAP_BG`. Drawing a world object at world-x `W` is `putimage(xMap + W, …)`
because `xMap` is already negative.

World y is never scrolled. `yMap` stays 0. The sky strip is selected with
`yBg = -(world - 1) * YSIZE` so each world uses a different 384-px band of
`mapsky.bmp`.

## Ownership of music

`main` opens the loop tracks (`背景音乐`, `胜利`, `通关`, `游戏结束`).
`Role` opens the one-shots (`跳`, `金币`, `死亡1`, …). Aliases are never
`close`d except the unreachable `close all` after the infinite loop. Restart
paths use `play … from 0` instead of reopening.

## Frame budget

`TIME` is 0.01 s. `Timer::Sleep` aims at that using
`QueryPerformanceCounter`. If a frame overruns, `m_oldclk` is snapped forward
so the next wait is not a huge catch-up stall. Sprite iframe counters are
advanced with `TIME * n`, so a slow machine still *counts* the same number of
ticks, it just presents them late.

## What is *not* in the architecture

- No delta-time independent of `TIME`. Holding a low FPS does not stretch
  jumps; it just drops perceived smoothness.
- No separate render thread. `BeginBatchDraw` / `EndBatchDraw` only buffers
  GDI.
- No entity list. Enemies, bullets, bombs, coins, and food are fixed C arrays
  with “slot empty” encoded as `x == 0 && y == 0` (or `turn == 0` for
  enemies). Slot 0 at the origin is therefore unusable, which is why pickups
  are never placed on tile (0, 0).
