# Architecture

The game is a single-threaded EasyX loop. Three objects share one frame:

```
Control gameCtrl;          // input + menus + HUD
Role    gameRole(world);   // hero, enemies, bullets, bombs
Scene   gameScene(world);  // tiles, coins, food, sky
```

`world` is a global `int` (1, 2, or 3). `life` is a second global, reset
to `LIFE` (5) on a full game-over.

## Frame loop (`main.cpp`)

```
initgraph(512, 384)
gameCtrl.gameStart()          // blocking title until "开始" or a valid save
open / loop background music
while true:
    key = gameCtrl.getKey()   // GetAsyncKeyState bits; Esc opens pause
    if VIR_RESTART: rebuild Role + Scene for the current world
    if VIR_HOME:    reset life & world, return to title
    gameRole.action(key, &gameScene, world)
    gameScene.action(&gameRole)          // camera / parallax only
    if died:  play death, life--, Game Over or "lives remaining"
    if passed:
        if world == 3: credits + reset to world 1
        else:          world++, interstitial, rebuild Role + Scene
    BeginBatchDraw
        scene.show(); role.show(); HUD score + level
    EndBatchDraw
    Timer::Sleep(TIME * 1000)            // TIME = 0.01 s  →  10 ms
```

`Role::show` also advances bullets (`bullteFlying`). That is why shooting
is coupled to the draw path, not only `action`.

## Module map

```
main.cpp        window, music aliases, life/world, loop
define.h        screen, physics macros, CMD_* / VIR_* bits
control.*       title, pause, HUD, GetAsyncKeyState
role.*          Hero / Enemy / Bullet, hit tests, scoring
scene.*         Map[] / coins[] / food[], blit + animation
inertia.*       one static integrator: x = vt + ½at², then v += at
timer.h         QueryPerformanceCounter busy-wait Sleep
```

Forward declarations keep the include cycle to pointers only:

- `scene.h` forward-declares `class Role`
- `role.h`  forward-declares `class Scene`
- both files define `struct Map` behind the same `#ifndef _MAP` guard

`Role` stores a `Scene*` written every `action()` call so `show()` can
still test bullets against tiles.

## Coordinate systems

There are three X axes that must stay in sync:

| Symbol | Meaning |
| --- | --- |
| `Hero.x` / `Hero.xx` | Sprite left edge **on the 512-pixel window** |
| `Hero.x0` | Camera origin: world-space shift applied to tiles / enemies |
| `Scene.xMap` | Copy of `x0` used when blitting the map |

World X of the hero is `hero.x - hero.x0` (because `x0` is ≤ 0 while
scrolling right). `Scene::isEnding` compares that distance to a per-world
finish line (94 or 104 tiles × 32 px).

The hero is clamped to `[XLEFT, XRIGHT]` = `[0, 192]`. Past 192 px the
extra displacement is subtracted from `x0`, which slides the world left
under a fixed sprite — a classic camera-window.

Y is window space only. There is no vertical camera. `yBg` selects which
band of `mapsky.bmp` to show (`-(world-1) * 384`).

## Data that is not in files

World layouts are **compiled C arrays** inside `Scene::createMap`,
`createCoin`, `createFood`, and `Role::createEnemy`. There is no external
level format. `gameRecord.dat` stores a single integer: the last saved
`world`.

`MAP_NUMBER` is 30. Several authored arrays are longer than 30, so the
copy loops drop the tail. The ASCII preview in `examples/04_level_preview`
prints both the authored list and the 30-slot load cap. See
[known-issues.md](known-issues.md).

## Drawing

EasyX `putimage` with `SRCAND` then `SRCPAINT` is the transparency
path (mask row + color row). Sprite sheets:

| Image | Used by |
| --- | --- |
| `res/role.bmp` | Hero walk / death frames |
| `res/ani.bmp` | Enemies, coins, score pop, food, bullets, bombs |
| `res/map.bmp` | 32×32 tiles, id 1–10 |
| `res/scenery.bmp` | Animated grass / water / trees (id ≥ 11) |
| `res/mapsky.bmp` | Parallax sky, three stacked 384-px bands |
| `res/home.bmp` | Title and interstitial strips |

Animation clocks are doubles advanced by `TIME` (or `F = TIME*0.3` for
scenery) and wrapped when they hit a frame count.

## Music

`Winmm.lib` + `mciSendString`. Aliases are opened once in `main` and
`Role`'s constructor. Restart / death / clear stop or seek `music_bg`.
Aliases are listed in [resources.md](resources.md).

## Persistence

Pause → "进行存档" writes `gameRecord.dat` as `"%d"` of the global
`world`. Title → "读档" reads it back. Values outside `1..3` show a
message box and stay on the title screen.
