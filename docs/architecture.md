# Architecture

The 2020 source is a single Win32 process. One window, one game loop, three
long-lived objects, two globals.

```
main()
  initgraph(512, 384)
  Control gameCtrl
  gameCtrl.gameStart()          // blocking home UI
  Role  gameRole(world)
  Scene gameScene(world)
  open / loop MCI aliases
  Timer gameTimer
  while (true)
      key = gameCtrl.getKey()
      maybe reconstruct Role + Scene
      gameRole.action(key, &gameScene, world)
      gameScene.action(&gameRole)
      death / clear / pass branches
      BeginBatchDraw
          gameScene.show()
          gameRole.show()
          HUD score + level
      EndBatchDraw
      gameTimer.Sleep(TIME * 1000)   // 10 ms
```

`TIME` is `0.01` seconds. That value is both the physics step and the
nominal frame time. `Timer::Sleep` uses `QueryPerformanceCounter` so a slow
frame still tries to keep wall-clock step size close to 10 ms.

## Globals

`main.cpp` owns two integers that every other file treats as process state:

| Name | Initial | Meaning |
| --- | --- | --- |
| `life` | `LIFE` (5) | remaining continues |
| `world` | `1` | current stage, 1..3 |

`control.cpp` declares `extern int world` so the pause-menu save and the
home-screen load can write / read the same integer.

Death: `life--`. At zero, play `music_end`, show the game-over slice of
`home.bmp`, reset `life` and `world`, return to `gameStart()`.

Clear: increment `world` (or reset after world 3) and reconstruct both
`Role` and `Scene` with the new index. Reconstruction is assignment,
not a heap reset:

```cpp
gameRole  = Role(world);
gameScene = Scene(world);
```

That re-runs the constructors: sprite loads, enemy tables, map tables, and
a second `mciSendString("open ...")` for the SFX aliases.

## Objects

```
Control     input + menus + HUD          no pointer to Role / Scene
Role        hero, enemies, bullets       holds Scene* only during action/show
Scene       tiles, coins, food, sky      reads Hero* through Role::getHero()
Inertia     one static function          no members
Timer       static QPC sleep             no instance state worth keeping
```

`Role` and `Scene` forward-declare each other. `struct Map` is defined in
both headers behind `#ifndef _MAP`. The include graph is:

```
main.cpp
  timer.h
  define.h
  control.h  -> graphics.h
  role.h     -> define.h, graphics.h, forward Scene
  scene.h    -> define.h, graphics.h, forward Role

role.cpp
  inertia.h -> graphics.h, define.h, scene.h
  role.h

scene.cpp
  scene.h, role.h, define.h

control.cpp
  control.h, define.h
```

`inertia.h` includes `scene.h` even though `Inertia::move` never touches a
`Scene`. That include is leftover from the 2020 refactor.

## Frame duties

### Control::getKey

1. If `_kbhit()`, replace `key` with a bit-or of `GetAsyncKeyState` results.
2. If `CMD_ESC` is set, enter `pauseClick()` and block on `GetMouseMsg`.
3. Return the last stored `key`.

Because `key` is a member, a held `D` continues to apply on frames where
`_kbhit()` is false. Releasing a key does **not** clear the bit until
another `_kbhit()` sample sees the new `GetAsyncKeyState` mask. That is
why the hero can keep walking after the key-up until something else is
pressed. Documented further in [controls-and-save.md](controls-and-save.md).

### Role::action

Order inside one call:

1. Record `this->myScene`.
2. Jump impulse if `CMD_UP` and (grounded, or `world == 3`).
3. Integrate vertical motion; snap to tiles; stomp; fall-death.
4. Horizontal acceleration from `CMD_LEFT` / `CMD_RIGHT` / ending auto-walk.
5. Friction from the tile under the hero (`map->u`).
6. `Inertia::move` on `vX`, scale by `UNREAL_HEIGHT / REAL_HEIGHT`.
7. Camera clamp (`XLEFT` / `XRIGHT`) and `x0` scroll.
8. Spawn bullets if armed.
9. Step enemies; collect coins / food; side-hit death.

### Role::show

Draws the hero, then enemies, then bomb frames, then bullets. Bullet
**flight** (`bullteFlying`) runs here, not in `action`. A bullet therefore
moves once per rendered frame, after the hero has already integrated.

### Scene::action

Copies `hero.x0` into `xMap`. If the hero is pinned to `XRIGHT` and still
has positive `vX`, the sky bitmap slides left by

```
|vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG
```

`K_MAP_BG` is 5, so the sky lags the tile map.

### Scene::show

1. Wrap `xBg` and blit two copies of `mapsky.bmp`.
2. Walk `map[]` and blit tiles / scenery.
3. Animate coins, pickup flashes, and the weapon sprite.

## Capacity constants

From `role.h` and `scene.h`:

| Symbol | Value | Used for |
| --- | --- | --- |
| `ENEMY_TOTE` | 30 | `Enemy myEnemy[]` |
| `BOMB_NUMBER` | 5 | simultaneous explosions |
| `BULLET_NUMBER` | 30 | live bullets |
| `MAP_NUMBER` | 30 | tile records |
| `COINS_NUMBER` | 70 | coin slots |
| `SCORE_NUMBER` | 5 | "+score" flash slots |
| `FOOD_NUMBER` | 5 | weapon pickups |

World 1 places 10 enemies, 19 coins, 1 food, and 32 map records. World 2
places 6 enemies and 36 map records. World 3 fills the map array from a
random pipe recipe (see [levels.md](levels.md)).

## Music aliases

`main.cpp` opens four loop / sting aliases that live for the process:

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | `res\背景音乐.mp3` | looping during play |
| `music_win` | `res\胜利.mp3` | world clear |
| `music_passedAll` | `res\通关.mp3` | after world 3 |
| `music_end` | `res\游戏结束.mp3` | lives exhausted |

`Role::Role` opens a second set of one-shot aliases (`music_died`,
`music_jump`, `music_coin`, `music_tread`, `music_getWeapon`,
`music_bullet`, `music_boom`, `music_boom2`). Reconstructing `Role`
re-opens those names.

## What is *not* here

There is no entity component system, no separate renderer thread, no
level file format beyond the C arrays in `createMap` / `createEnemy` /
`createCoin` / `createFood`. World 3 is the only procedural piece, and
it only randomizes pipe gap heights.
