# Architecture

MaoLiAo is a single-window, single-threaded EasyX program. There is no scene graph, no entity component system, and no resource manager. Four objects plus two globals own the whole game.

```
main.cpp
  life (int, default LIFE == 5)
  world (int, 1..3)
  Control gameCtrl     menus, keys, HUD, save/load
  Role    gameRole     hero, enemies, bullets, score
  Scene   gameScene    sky, tiles, coins, food, parallax
  Timer   gameTimer    high-resolution Sleep
```

`Role` and `Scene` are rebuilt from `world` whenever a life is lost, a level is cleared, the player restarts, or the start menu runs again. Nothing persists across those resets except `life`, `world`, and the contents of `gameRecord.dat`.

## Object roles

### `Control` (`control.h` / `control.cpp`)

- Polls `GetAsyncKeyState` into a bit mask (`CMD_LEFT`, `CMD_RIGHT`, `CMD_UP`, `CMD_SHOOT`, `CMD_ESC`, …).
- Draws the start menu and the Esc pause sheet with EasyX rectangles and `drawtext`.
- Writes `world` to `gameRecord.dat` and reads it back on 读档.
- Blits HUD strings (`得分`, `关卡`) and the between-level / death / credits screens from slices of `res\home.bmp`.

`Control` does not move the hero. It only returns a command word each frame.

### `Scene` (`scene.h` / `scene.cpp`)

- Owns one `Map map[MAP_NUMBER]` (`MAP_NUMBER == 30`).
- Owns `POINT coins[70]`, `POINT food[5]`, and a short-lived `score[]` list used only for the “+points” sparkle.
- Loads `mapsky.bmp`, `map.bmp`, `scenery.bmp`, `ani.bmp`.
- `createMap` / `createCoin` / `createFood` hard-code the three shipped worlds.
- `action(Role*)` copies the hero origin `x0` into `xMap` and eases the sky by `K_MAP_BG`.
- `show()` walks tiles, coins, pickups, and decorative scenery. Tile ids `>= 8` use larger blit rectangles; ids `> 10` are scenery (trees, water, the goal sign) and are **not** solid.

`Scene` never applies gravity. It is a camera + tile list.

### `Role` (`role.h` / `role.cpp`)

- Owns `Hero myHero`, `Enemy myEnemy[30]`, `Bullet bullets[30]`, `POINT bombs[5]`, and `score`.
- `action(key, scene, world)` is the entire simulation step: jump, integrate, collide, walk enemies, collect, shoot.
- `show()` chooses sprite frames and, as a side effect, advances in-flight bullets (`bullteFlying`).

Collision helpers (`hitMap`, `hitCoins`, `hitFood`, `hitEnemy`) all share the same “hero inset quad vs axis-aligned box” test (`isHit`).

### `Inertia`

A one-function helper:

```text
X = v * t + 0.5 * a * t * t
v = v + a * t
```

Used for both vertical jumps and horizontal run-up. See [physics.md](physics.md).

### `Timer`

Multimedia / `QueryPerformanceCounter` sleeper. `main` calls `Timer::Sleep((int)(TIME * 1000))` with `TIME == 0.01`, so the design frame is 10 ms. Death and clear screens use much longer sleeps (2–7.8 s) and block the loop.

## Data flow (one live frame)

```
getKey()
    │
    ├─ VIR_RESTART  → reconstruct Scene(world), Role(world)
    └─ VIR_HOME     → reset life/world, gameStart(), reconstruct
    │
Role.action(key, scene, world)
    │  jump / integrate / collide / enemies / coins / food / bullets-ready
    │
Scene.action(role)
    │  xMap = hero.x0 ; maybe slide xBg
    │
if died  → cut music, show last frame, lose a life or Game Over
if passed → world++ or credits, reconstruct
    │
BeginBatchDraw
    scene.show()
    role.show()          ← also steps bullets
    showScore / showLevel
EndBatchDraw
Sleep(10 ms)
```

The important coupling: **the camera origin lives on the hero** (`Hero::x0`), not on `Scene`. `Scene::xMap` is a copy of that origin used while blitting. Collision in `Role` converts screen-space hero pixels back into map space with `-x0 + x`.

## Coordinate spaces

There are three spaces in play at once. Mixing them is the usual source of bugs when you add content.

| Space | Who uses it | Unit | Origin |
| --- | --- | --- | --- |
| Screen | `Hero::x,y`, bullets, HUD | pixels | window top-left, 512×384 |
| Map / camera | `Hero::x0`, `Scene::xMap` | pixels | slides left as you run right |
| Tile | `Map.{x,y}`, coins, enemy spawn | 32×32 cells | top-left of the level |

Food pickups are stored as **pixels**, not tiles (world 1 writes `14 * WIDTH, 5 * HEIGHT`). World 3’s food `{10, 10}` is already pixels.

Enemies are stored as **map pixels** after spawn (`tile * WIDTH/HEIGHT`) and are drawn at `x0 + enemy.x`.

## Capacity constants

These are real array sizes, not soft guidelines.

| Constant | File | Value | Used for |
| --- | --- | --- | --- |
| `MAP_NUMBER` | `scene.h` | 30 | solid + scenery tiles |
| `COINS_NUMBER` | `scene.h` | 70 | coins |
| `FOOD_NUMBER` | `scene.h` | 5 | weapon pickups |
| `SCORE_NUMBER` | `scene.h` | 5 | floating score pops |
| `ENEMY_TOTE` | `role.h` | 30 | walkers |
| `BULLET_NUMBER` | `role.h` | 30 | simultaneous shots |
| `BOMB_NUMBER` | `role.h` | 5 | explosion sprites |

World 1’s `createMap` lists **32** tiles and world 2 lists **36**. The copy loops stop at `MAP_NUMBER`, so trailing scenery is silently dropped. The toolkit flags this. Details in [known-quirks.md](known-quirks.md).

## What is *not* in the engine

- No JSON / text map loader. Worlds are C++ array literals.
- No pause-independent simulation. `GetMouseMsg` in menus **blocks**.
- No jump buffer, coyote time, or variable jump height (except world 3’s infinite `CMD_UP`).
- No enemy gravity. Walkers only reverse when `hitMap` says they walked into a wall or stepped off a ledge.
- No projectile vs hero test. Only hero vs enemy, hero vs tile, bullet vs enemy/tile.

If you want data-driven levels, start from `examples/levels/*.json` and the exporter in `examples/toolkit/maoliao_levels.py`. The game still needs a C++ paste (see [adding-a-level.md](adding-a-level.md)).
