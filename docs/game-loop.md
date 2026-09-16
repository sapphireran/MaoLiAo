# Game loop

`main` is an infinite `while (true)` after the title screen. There is no
fixed-timestep accumulator: each iteration sleeps about 10 ms and treats
that as `TIME`.

## Startup

```
initgraph(512, 384)
SetWindowText("猫里奥 V2.0")
Control::gameStart()          // blocks on the home menu
Role(world), Scene(world)     // world starts at 1
open + play music_bg repeat
Timer gameTimer
```

`gameStart` returns only after **开始** or a valid **读档**. **退出**
calls `exit(0)`. Introduction / help screens recurse back into
`gameStart`.

## Per-frame body

```
key = gameCtrl.getKey()

if key == VIR_RESTART:
    rebuild Role + Scene for current world
    rewind music_bg
elif key == VIR_HOME:
    stop music, life = LIFE, world = 1
    gameStart(), rebuild Role + Scene, play music_bg

Role.action(key, &Scene, world)
Scene.action(&Role)

if Role.isDied():
    freeze a frame, sleep 3500 ms, life--
    if life == 0: game-over reel, reset to world 1, title
    else: show remaining lives, rebuild current world

if Role.isPassed():
    if world == 3: win + credits, reset, title
    else: sleep, world++, "LEVEL: N" card, rebuild

batch-draw Scene, Role, score, level
Timer.Sleep(10)
```

Pause virtual keys are not cleared after handling. `getKey` keeps
returning the last bitset until a new `_kbhit()` arrives. After
`VIR_RESTART` / `VIR_HOME`, the next frames may still see those bits
until the player presses a movement key. In practice the rebuild already
happened, so the leftover bit is mostly harmless.

## Death

`Hero.died` is set when:

- falling below `YSIZE` (384)
- touching an enemy while `vY <= 0` (not a stomp)
- world 3: `hitMap` on a solid that is not tile id 2, and `isShoot`
  is still false (the weapon/star pickup is the "safe" flag)

`main` does **not** immediately rebuild. It draws one more composed
frame, waits 3.5 s, then either shows remaining-life icons or the
game-over strip of `home.bmp`.

Lives are not drawn during play — only on the death interstitial
(`Control::showDied`). That function draws 1–4 hero sprites; a 5-life
start therefore never shows five icons after the first death (`life`
is already decremented).

## Clear / ending

Two flags, both on `Hero`:

| Flag | Set when | Effect |
| --- | --- | --- |
| `ending` | world-X of hero exceeds the world's finish distance | Input ignored except the forced right walk |
| `passed` | `hero.x > XSIZE` (walked off the right of the window) | `main` advances the world |

Finish distances (`Scene::isEnding`):

| World | Distance (px) | Tiles |
| --- | --- | --- |
| 1 | `94 * 32` = 3008 | Goal sign sits near tile x = 101 |
| 2 | `104 * 32` = 3328 | Goal sign at tile (111, 1) |
| 3 | `94 * 32` = 3008 | Cloud runway + sign at tile (101, 4) |

While `ending` is true, `CMD_RIGHT` is forced and left/jump (except
world 3's always-on jump test) are suppressed. The hero auto-walks
off-screen.

World 3 also plays `music_win` then `music_passedAll` before
`showPassedAll`.

## Restart vs home vs save

| Pause item | Virtual key | `main` reaction |
| --- | --- | --- |
| 返回游戏 | `VIR_RETURN` (64) | None; loop continues |
| 重新开始 | `VIR_RESTART` (128) | Rebuild current world |
| 退出游戏 | `VIR_HOME` (256) | Title, world 1, full lives |
| 进行存档 | writes `gameRecord.dat`, then `VIR_RETURN` | None |

Save format is a single ASCII integer: the current `world`. Load is on
the title **读档** button. Values outside 1–3 pop a `MessageBox`.

The pause label says 退出游戏 / "The menu" but the handler is the same
`VIR_HOME` path as "return to title", not process exit. Process exit is
only the title **退出** button.

## Timing

`TIME` is 0.01 s. `Timer::Sleep` uses `QueryPerformanceCounter` and
drifts `m_oldclk` forward so late frames skip the wait instead of
catching up with extra simulation steps. Physics therefore **slows
down** if a frame overruns, rather than spiraling.

`Control::showDied` / `showPassed` use the Win32 `Sleep`, not `Timer`.
Those waits are wall-clock presentation, not simulation.

## Why examples simulate a subset

A full `main` cannot run here (no EasyX). The portable runner in
`examples/` ticks `HeroState` with the same `TIME`, integrator, jump
impulse, and camera pin so jump height, slide-to-stop, and finish
distance can be asserted without a window.
