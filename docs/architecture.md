# Architecture

MaoLiAo is a single-threaded EasyX loop. There is no scene graph, no ECS, and no asset packer. Three objects live for the life of a run and are reconstructed whenever the player dies, restarts, changes world, or returns to the title screen.

```
main
 ├── Control  gameCtrl     input, title, pause, HUD, interstitials
 ├── Role     gameRole     hero, enemies, bullets, bombs, score
 ├── Scene    gameScene    tiles, coins, food, score pops, parallax
 ├── Timer    gameTimer    QueryPerformanceCounter sleep
 └── globals  life, world
```

`Role` and `Scene` point at each other only through function arguments and a `Role::myScene` cache set at the start of `Role::action`. Headers use incomplete types (`class Scene;` / `class Role;`) so the two `.h` files do not include each other.

## Compilation units

| File | Responsibility |
| --- | --- |
| `main.cpp` | Window, MCI playlist, outer `while (true)` |
| `define.h` | Screen, tile, physics, command macros |
| `control.cpp` | `GetAsyncKeyState` bits, mouse menus, HUD text |
| `role.cpp` | Hero integration, enemy AI, bullets, pickups |
| `scene.cpp` | Map tables, coin/food tables, blit + scroll |
| `inertia.cpp` | `X = v t + ½ a t²`, then `v += a t` |
| `timer.h` | Header-only QPC waiter (statics defined in the header) |
| `resource.h` / `MaoLiAo.rc` | Icon `IDI_ICON1` |

EasyX headers (`graphics.h`) and `Winmm.lib` are required only by the game, not by `examples/`.

## Ownership

### Control

- Stores the last command bitmask in `key`.
- Loads `res\home.bmp` stretched to `XSIZE × 5*YSIZE` so different vertical slices can be used as title / death / pass / ending art.
- Does **not** own the hero or the map. `main` asks it to draw score and level each frame.

### Role

- One `Hero` struct: screen `x,y`, subpixel `xx,yy`, camera origin `x0`, velocities, facing `turn`, flags (`isFly`, `isShoot`, `died`, `ending`, `passed`).
- Fixed arrays: `Enemy[30]`, `Bullet[30]`, `POINT bombs[5]`.
- Sprite sheets: `role.bmp` (hero), `ani.bmp` (enemies, bombs, bullets).
- Score is an `int` on `Role`, not a global.

A slot is “empty” when coordinates are `(0,0)` (and for enemies, `turn == 0`). That sentinel is also a legal world-space origin, which is why unused enemies are left at zero with `turn == 0` and hit tests skip `turn == 0`.

### Scene

- `Map map[30]` solid and decorative tiles.
- `POINT coins[70]` in **tile** coordinates.
- `POINT food[5]` in **pixel** coordinates (world 1 and 2 multiply by `WIDTH`/`HEIGHT` at spawn; world 3 stores `{10,10}` pixels).
- `POINT score[5]` short-lived “+points” burst positions.
- Background `mapsky.bmp` is four stacked 384-pixel skies; `yBg = -(world-1)*YSIZE` picks the strip.

### Globals

```cpp
int life = LIFE;  // 5, reset on game over or full clear
int world = 1;    // 1..3, written to gameRecord.dat
```

`Control.cpp` uses `extern int world` so the pause-menu save button and the title-screen load button can read/write the same integer `main` uses to reconstruct `Role` and `Scene`.

## Coordinate spaces

There are three X axes in play at once:

| Space | Who stores it | Meaning |
| --- | --- | --- |
| Screen | `Hero.x` | Pixels in the 512-wide window. Clamped to `[XLEFT, XRIGHT]` = `[0, 192]` until the ending auto-walk. |
| Camera | `Hero.x0` | Negative world offset. When the hero tries to walk past `XRIGHT`, `x0` decreases and the hero stays at 192. |
| World | `enemy.x`, `map.x * 32`, `coins.x * 32` | Absolute pixels or tiles. Enemies are drawn at `(int)x0 + enemy.x`. |

`Scene::xMap` is a copy of `Hero.x0` updated in `Scene::action`. Background `xBg` moves at `1/K_MAP_BG` of the hero’s horizontal step so the sky parallax-scrolls.

Y is screen-down in EasyX. Jump velocity is negative; gravity `G` is a positive scalar added to `vY`.

## Reconstruction

`Role` and `Scene` have no `reset()` method. `main` assigns new objects:

```cpp
gameRole = Role(world);
gameScene = Scene(world);
```

Constructors reload BMPs and reopen several MCI aliases. That is why death / restart hitch: the same `loadimage` / `mciSendString("open ...")` path runs again.

## Data flow in one frame

1. `Control::getKey()` — keyboard bits; may block inside `pauseClick()`.
2. `main` handles `VIR_RESTART` / `VIR_HOME` by reconstructing objects.
3. `Role::action(key, &gameScene, world)` — integrate hero, walk enemies, collect, shoot.
4. `Scene::action(&gameRole)` — copy `x0` into `xMap`, nudge `xBg`.
5. Death / pass branches in `main` (audio + interstitial + reconstruct).
6. `BeginBatchDraw` → `Scene::show` → `Role::show` → HUD → `EndBatchDraw`.
7. `Timer::Sleep(TIME * 1000)` ≈ 10 ms.

`Role::show` also advances bullets (`bullteFlying`). Projectile motion is therefore tied to the draw path, not only to `action`.
