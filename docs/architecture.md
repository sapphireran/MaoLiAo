# Architecture

MaoLiAo is a single-threaded EasyX loop. There is no scene graph, no entity component system, and no separate render thread. Each frame: read keys → move the hero and enemies → scroll the camera → draw.

## Types

```
main.cpp
  ├─ Control   input + menus + HUD
  ├─ Role      Hero, Enemy[], Bullet[], bombs, score
  ├─ Scene     Map[], coins[], food[], sky + tiles
  ├─ Inertia   static move(v, t, a)
  └─ Timer     QueryPerformanceCounter Sleep
```

Globals in `main.cpp`:

- `life` — starts at `LIFE` (5). Decrement on death; title + world 1 when it hits 0.
- `world` — 1, 2, or 3. `Control` writes / reads it through `gameRecord.dat`.

`Role` holds a `Scene*` set at the start of `Role::action` so `Role::show` can fly bullets against the current map.

## Outer loop (`main`)

```
initgraph(512, 384)
Control.gameStart()          // blocks on the title mouse loop
Role(world), Scene(world)
open + loop background music
while true:
    key = Control.getKey()   // may block in pauseClick()
    if VIR_RESTART: rebuild Role + Scene for this world
    if VIR_HOME:    reset life/world, title, rebuild
    Role.action(key, &Scene, world)
    Scene.action(&Role)      // camera / sky
    if Role.isDied():
        freeze 3.5s, life--, Game Over or "lives left"
    if Role.isPassed():
        if world == 3: credits + title
        else: world++, intermission
    batch: Scene.show, Role.show, score, level
    Timer.Sleep(TIME * 1000) // 10 ms
```

`BeginBatchDraw` / `EndBatchDraw` wrap the HUD frame so EasyX presents once. Pause and title use their own batch / mouse loops.

## Command bits (`define.h`)

`Control::GetCommand` ORs `GetAsyncKeyState` results:

| Bit | Macro | Default key |
| --- | ---: | --- |
| 1 | `CMD_LEFT` | A |
| 2 | `CMD_RIGHT` | D |
| 4 | `CMD_UP` | W or K |
| 8 | `CMD_DOWN` | S (read, unused in `Role::action`) |
| 16 | `CMD_SHOOT` | J |
| 32 | `CMD_ESC` | Esc |
| 64 | `VIR_RETURN` | pause → resume |
| 128 | `VIR_RESTART` | pause → rebuild this world |
| 256 | `VIR_HOME` | pause → title (label says 退出游戏) |

`getKey` only refreshes the bitmask when `_kbhit()` is true, then still treats a latched `CMD_ESC` as “open pause”. Held WASD therefore depends on a recent console key event plus `GetAsyncKeyState`.

## `Role::action` order

1. Jump if `CMD_UP` and (grounded, or `world == 3`).
2. If airborne: integrate `vY` with gravity, snap to tiles, stomp, fall-off-screen death.
3. Horizontal accel from A/D (or auto-run when `ending`).
4. Friction `a1` from the tile underfoot (`map->u`).
5. `Inertia::move` on `vX`; camera lock at `XRIGHT` unless finishing.
6. `Scene::isEnding` → `ending`; `x > XSIZE` → `passed`.
7. Shoot if `isShoot` and `CMD_SHOOT`, with `TIME_INTERVAL_BULLET` while held.
8. Clamp `XLEFT`; when `x > XRIGHT` and not ending, subtract the overflow from `x0` (camera).
9. Step enemies; pick coins / food; side-hit enemies kill if `vY <= 0`.

`Role::show` draws sprites **and** advances bullets (`bullteFlying`). A frame that dies or passes still drew the previous show, then `main` draws again after the branch.

## `Scene`

`createMap` / `createCoin` / `createFood` fill fixed-size arrays (`MAP_NUMBER` 30, `COINS_NUMBER` 70, `FOOD_NUMBER` 5). Tile records are:

```text
x, y, id, xAmount, yAmount, u
```

`x`/`y` are tile coordinates. Pixel box is `xAmount * WIDTH` by `yAmount * HEIGHT`, except ids 8 and 10 (pipe mouths) which use 2× tile size. `u` is assigned from `V_MAX`, `T1`/`T2`/`T3`, and `G` — see [physics.md](physics.md).

`Scene::action` copies `hero.x0` into `xMap` and, when the hero is pinned at `XRIGHT` and moving right, subtracts a scaled step from `xBg`.

## Camera model

The hero’s **screen** `x` stays in `[XLEFT, XRIGHT]` (`0` … `192`) until the end fanfare. World `x` is `x - x0` (`x0` is ≤ 0 while scrolling). Enemies and tiles are stored in world space and drawn at `xMap + worldX` with `xMap == x0`.

World 1 and 3 end when world `x > 94 * 32`. World 2 uses `104 * 32`. After that, `ending` forces rightward accel until `screen x > 512`.

## Drawing

Sprites use EasyX mask pairs: `SRCAND` on the black-mask row, then `SRCPAINT` on the color row (`role.bmp`, `ani.bmp`, `scenery.bmp`). Solid tiles come from `map.bmp` at `y0 = (id - 1) * HEIGHT`.

Sky: `mapsky.bmp` is one bitmap, four screens tall. `yBg = -(world - 1) * YSIZE` picks the band. Horizontal wrap when `xBg <= -width`.

## Audio aliases

Opened once in `main` or `Role` constructor; replayed with `play … from 0`. Aliases are listed in [resources.md](resources.md).

## What is not here

No pause of simulation independent of UI, no delta-time (fixed `TIME = 0.01`), no serialization of score or position, no editor. Worlds are C arrays in `scene.cpp` / `role.cpp`.
