# Architecture

The Windows build is a single-threaded, fixed-timestep loop around three objects
and two globals.

```
life = 5, world = 1
Control gameCtrl
  gameStart()                 # blocking title menu
Role   gameRole(world)
Scene  gameScene(world)
open MCI aliases, play music_bg

loop forever at TIME = 0.01 s
  key = gameCtrl.getKey()     # may block inside the pause menu
  handle VIR_RESTART / VIR_HOME
  gameRole.action(key, &gameScene, world)
  gameScene.action(&gameRole) # camera / parallax
  if died  -> lose a life, maybe Game Over, rebuild Role + Scene
  if passed -> next world or credits, rebuild Role + Scene
  BeginBatchDraw
    scene.show(); role.show(); HUD score + level
  EndBatchDraw
  Timer::Sleep(10)
```

`Role` and `Scene` are value-reassigned (`gameRole = Role(world)`) whenever the
run resets. That reloads bitmaps and MCI aliases each time.

## Object graph

```
main
 ├── Control     input + menus + HUD
 ├── Role        hero, enemies, bullets, bombs, score
 │     └── Scene*  borrowed each tick in Role::action
 └── Scene       tiles, coins, food, score-pop sprites, camera
       └── Role* borrowed each tick in Scene::action
```

Headers avoid a include cycle with forward declarations (`class Role;` in
`scene.h`, `class Scene;` in `role.h`). Both files also define `struct Map`
behind the same `_MAP` guard so either translation unit can see the tile record.

## Tick rate

`define.h` sets `TIME` to `0.01`. `Timer::Sleep` converts that to milliseconds
and busy-waits on `QueryPerformanceCounter` after a 1 ms `::Sleep` spin. The
intent is a 100 Hz simulation that stays aligned to the performance counter
instead of drifting through ordinary `Sleep`.

Sprite-frame accumulators also use `TIME`:

| Accumulator | Rate | Source |
| --- | --- | --- |
| coin / food flash | `TIME * 7` | `Scene::show` |
| score-pop | `TIME * 8` | `Scene::show` |
| scenery sway | `F = TIME * 0.3` | `Scene::show` |
| enemy walk | `TIME * 5` | `Role::show` |
| bomb / bullet flash | `TIME * 10` | `Role::show` |
| fire cooldown | `TIME_INTERVAL_BULLET = 0.2` | `Role::action` |

## Camera model

The hero is trapped in a horizontal window `[XLEFT, XRIGHT] = [0, 192]`.
When the sprite tries to walk past 192 px and the level is not in the ending
auto-walk, `Role::action` subtracts the overflow from `Hero::x0`:

```
x0 -= (x - XRIGHT)
x   = XRIGHT
```

`x0` is the world-space origin of the screen. `Scene::action` copies it into
`xMap` and, if the hero is pinned to the right edge and still has `vX > 0`,
shifts the looping sky by

```
bgStep = |vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG
```

`K_MAP_BG` is 5, so the sky creeps at one-fifth the ground speed. The sky
bitmap is 512 px wide and 4×384 tall; `yBg = -(world - 1) * YSIZE` selects
which vertical band world 1 / 2 / 3 uses.

Tiles, coins, and enemies are drawn at `xMap + local`. Local coordinates are
stored in tiles (maps, coins) or pixels (food, enemies, bullets).

## Life and world state

`life` and `world` are globals in `main.cpp`. `control.cpp` reaches `world`
through `extern int world` so the pause-menu save button can write the current
index. `Role` and `Scene` take `world` as a constructor argument and again as
a parameter on `action` / `hitMap` (world 3 death-on-touch).

Death path:

1. Freeze a batch-drawn frame for 3.5 s.
2. `life--`.
3. If `life == 0`, play `music_end`, show the Game Over strip of `home.bmp`,
   reset `life = 5` and `world = 1`, return to the title.
4. Otherwise show remaining-life cat icons and rebuild the current world.

Clear path:

1. Stop the BGM, play `music_win`, wait 6.5 s.
2. World 3 also plays `music_passedAll` and the credits strip, then resets.
3. Worlds 1 and 2 increment `world`, show “LEVEL: N”, rebuild.

## Drawing

Every gameplay frame is wrapped in `BeginBatchDraw` / `EndBatchDraw`. Sprites
use the classic EasyX two-pass trick:

```
putimage(..., mask row, SRCAND);
putimage(..., color row, SRCPAINT);
```

`role.bmp` stacks mask under color. `ani.bmp` and `scenery.bmp` do the same
for coins, food, enemies, bombs, bullets, grass, and water.

## What is not a subsystem

There is no entity list, no scene graph, no resource manager, and no data-driven
map file. Each world is a brace-initialized C array inside `createMap`,
`createCoin`, `createFood`, and `createEnemy`. Capacity is compile-time:

| Buffer | Capacity | Owner |
| --- | --- | --- |
| `Map map[]` | 30 | Scene |
| `POINT coins[]` | 70 | Scene |
| `POINT score[]` | 5 | Scene (coin-pop FX) |
| `POINT food[]` | 5 | Scene |
| `Enemy myEnemy[]` | 30 | Role |
| `POINT bombs[]` | 5 | Role |
| `Bullet bullets[]` | 30 | Role |

Empty slots are `(0, 0)` or `turn == 0`. That is also why a coin at tile `(0, *)`
can never exist: `Scene::show` skips `coins[j].x != 0`.
