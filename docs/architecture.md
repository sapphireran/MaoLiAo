# Architecture

猫里奥 is a single-threaded EasyX program. There is no entity component system, no scene graph, and no resource manager beyond `loadimage` / `mciSendString` in constructors. Three objects plus two globals make up the runtime.

## Objects

```
main.cpp
  life  : int          remaining lives (starts at LIFE, which expands to 5)
  world : int          current stage index 1..3
  Control gameCtrl     input, title, pause, HUD, save/load
  Role    gameRole     hero, enemies, bullets, bombs, score
  Scene   gameScene    tiles, coins, food, parallax background
  Timer   gameTimer    QueryPerformanceCounter-based Sleep
```

`Role` and `Scene` are reconstructed (`gameRole = Role(world)`) on death, restart, home, and stage clear. That reloads bitmaps and MCI aliases and resets positions.

Forward declarations break the `Role` ↔ `Scene` include cycle:

- `scene.h` forward-declares `class Role`
- `role.h` forward-declares `class Scene`
- Both headers define `struct Map` behind `#ifndef _MAP` so the layout is shared

`Role::action` stores `Scene*` in `myScene` so `Role::show` can fly bullets against the same map.

## Coordinate systems

There are three x-axes in play at once:

| Name | Where | Meaning |
| --- | --- | --- |
| Screen `Hero.x` | pixels in the 512-wide window | Clamped to `[XLEFT, XRIGHT]` except during the clear walk-off |
| Subpixel `Hero.xx` / `Hero.yy` | double | Integration target; `x` / `y` are truncated copies |
| Camera `Hero.x0` | double, usually ≤ 0 | World origin relative to the window. When the hero is pushed past `XRIGHT`, `x0` decreases so the map slides left |
| Tile `Map.x` / `Map.y` | tile indices | Multiplied by `WIDTH` / `HEIGHT` (32) |

`Scene::xMap` is a snapshot of `Hero.x0` used when drawing tiles. Coins are stored as **tile** coordinates; food on worlds 1–2 is stored as **pixels** (world 3 food is `{10, 10}` pixels — see [known-quirks.md](known-quirks.md)).

Enemies store **world pixel** x (tile × 32 at spawn) and are drawn at `myHero.x0 + enemy.x`.

## Class responsibilities

### `Control`

- `GetCommand()` OR-combines `GetAsyncKeyState` bits (`CMD_LEFT` … `CMD_ESC`)
- `getKey()` latches the last command while a key is down (`_kbhit`) and opens the pause overlay on `CMD_ESC`
- `gameStart()` is a blocking mouse loop for the title / intro / guide / load screens
- HUD: `showScore`, `showLevel` in Cooper; death / clear interstitials blit slices of `res\home.bmp`

The pause overlay is **not** a separate scene object. It draws a green 90×120 panel and blocks in `GetMouseMsg` until a virtual key (`VIR_RETURN`, `VIR_RESTART`, `VIR_HOME`) is stored in `Control::key`.

### `Scene`

Holds:

- `Map map[MAP_NUMBER]` with `MAP_NUMBER = 30`
- `POINT coins[COINS_NUMBER]` (`COINS_NUMBER = 70`)
- `POINT food[FOOD_NUMBER]` (`FOOD_NUMBER = 5`)
- `POINT score[SCORE_NUMBER]` — short-lived “+points” sparkle positions
- Background `img_bg` (`mapsky.bmp` scaled to `XSIZE × 4*YSIZE`, one strip per world)
- Tile sheet `map.bmp`, scenery `scenery.bmp`, animations `ani.bmp`

`createMap(world)` fills `map[]` from a local C array, then assigns friction `u` from tile id. `action(Role*)` only updates parallax (`xBg`) when the hero is pinned to `XRIGHT` and moving right.

### `Role`

Holds:

- one `Hero`
- `Enemy myEnemy[ENEMY_TOTE]` (`ENEMY_TOTE = 30`)
- `Bullet bullets[BULLET_NUMBER]` (`BULLET_NUMBER = 30`)
- `POINT bombs[BOMB_NUMBER]` (`BOMB_NUMBER = 5`)
- `score`

Empty slots are `{0,0}` (and `turn == 0` for enemies/bullets). That sentinel collides with a genuine origin, so spawned objects are placed away from `(0,0)`.

### `Inertia`

Stateless helper: `Inertia::move(v, t, a)` updates `v` in place and returns displacement. Used for both vertical gravity and horizontal run.

### `Timer`

Header-only static QPC clock. `Timer::Sleep(ms)` busy-waits with `::Sleep(1)` until the performance counter catches up. The gameplay frame uses `Sleep((int)(TIME * 1000))` → 10 ms, i.e. a **100 Hz** target.

## Draw order (one frame)

From `main.cpp`:

1. `BeginBatchDraw()`
2. `gameScene.show()` — sky, tiles, scenery, coins, score pops, food
3. `gameRole.show()` — hero, enemies, bombs, bullets (bullets also **simulate** here)
4. `gameCtrl.showScore` / `showLevel`
5. `EndBatchDraw()`

Bullet motion (`bullteFlying`) runs inside `Role::show`, not inside `action`. A frame that skips `show` (the death freeze uses show once, then sleeps) still had `action` run that tick.

## Ownership and lifetime

Nothing is heap-allocated except what EasyX does internally. Arrays are fixed-size. Reconstructing `Role` / `Scene` by assignment runs constructors that `loadimage` again. MCI aliases (`music_died`, `music_jump`, …) are opened in `Role::Role` and **not** closed in the destructor; `main` only `mciSendString("close all")` after the infinite loop, which is unreachable.

## Globals

```cpp
int life = LIFE;   // LIFE is `5;` so this is `int life = 5;`
int world = 1;
```

`control.cpp` uses `extern int world` so the pause-menu save and the title-screen load can read/write the same stage index.
