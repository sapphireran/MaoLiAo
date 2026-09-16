# Game loop

All of this is `MaoLiAo/main.cpp`. The function never returns: the `closegraph()` lines after the loop are dead.

## Startup

```
initgraph(512, 384)
SetWindowText(..., "猫里奥 V2.0")
Control::gameStart()          // blocking title menu
Role(world) / Scene(world)
open MCI: music_bg, music_win, music_passedAll, music_end
play music_bg repeat
```

`life` starts at `LIFE` (5). `world` starts at 1 unless 读档 overwrote it inside `gameStart()` (that function assigns the global before `Role`/`Scene` are constructed in `main` — except on the first boot, `gameStart` runs *before* those constructors, so a successful load does apply).

## Per-frame body

```
key = gameCtrl.getKey()

if key == VIR_RESTART:
    reconstruct Role+Scene for current world
    rewind music_bg
elif key == VIR_HOME:
    stop music_bg
    life = 5, world = 1
    gameStart()
    reconstruct Role+Scene
    play music_bg

Role.action(key, &scene, world)
Scene.action(&role)

if role.isDied():
    freeze a frame, sleep 3500 ms, life--
    if life == 0: music_end, showGameOver, full reset to title
    else: showDied(life), reconstruct same world

if role.isPassed():
    stop music_bg
    if world == 3:
        music_win, sleep 6500, music_passedAll, showPassedAll
        full reset to title
    else:
        music_win, sleep 6500, world++, showPassed(world)
        reconstruct next world

batch: scene.show, role.show, score, level
Timer.Sleep(10)
```

## Death

`Role` sets `myHero.died` when:

- the hero’s feet go below `YSIZE` (384) while falling
- the hero overlaps an enemy and `vY <= 0` (not a stomp)
- world 3 tile contact with `id != 2` and `isShoot == false` (handled inside `hitMap`)

`main` then draws one more composed frame (so the death sprite is visible), waits 3.5 seconds, and decrements `life`. `showDied` blits 1–4 hero faces from `role.bmp` depending on remaining lives. There is no drawing branch for `life == 5` on that screen because the player only sees it after a death (`life` is already decremented). After four deaths you have 1 life left and see a single face; the fifth death is game over.

## Clear

`Role` sets `ending` when `Scene::isEnding(-(int)x0 + x)` is true:

| World | Distance threshold |
| --- | --- |
| 1 | `94 * 32` = 3008 px |
| 2 | `104 * 32` = 3328 px |
| 3 | `94 * 32` = 3008 px |

While `ending` is true, left input is ignored and right acceleration is forced (`KEY & CMD_RIGHT || ending`). Jump is also blocked except in world 3. When `Hero.x > XSIZE` (walks off the right of the 512 px window) `passed` becomes true.

World 3 is the last stage. Clearing it plays the long victory + 通关 sequence and returns to the title with `life` and `world` reset.

## Pause vs title

Both UIs are modal `while` loops that call `GetMouseMsg()`. The outer game loop does not tick during those menus. Esc sets `CMD_ESC`; `getKey` then calls `pauseClick()`, which overwrites `key` with `VIR_RETURN`, `VIR_RESTART`, or `VIR_HOME`. Saving writes `world` and returns `VIR_RETURN` so play continues.

The pause label “退出游戏” returns `VIR_HOME`, which `main` treats as “back to title”, not `exit(0)`. Only the title-screen “退出” calls `exit(0)`.

## Timing

`TIME` is `0.01` seconds. `Timer::Sleep` uses `QueryPerformanceCounter` and busy-waits with `::Sleep(1)` until the target tick. The first call initializes frequency. If a frame overruns, `m_oldclk` is snapped forward so the next wait is not extra-long (no catch-up spiral).

Physics uses `TIME` as `dt`. Sprite frame counters add `TIME * n` each display call (`enemy_iframe += TIME * 5`, coins `* 7`, bombs `* 10`).
