# 02 — Game loop

All campaign flow is in `main.cpp`. There is no state machine enum. Flags on `Hero` (`died`, `passed`, `ending`) plus the return value of `Control::getKey()` decide what happens.

## Startup

```text
initgraph(XSIZE, YSIZE)          // 512×384
SetWindowText(..., "猫里奥 V2.0")
Control::gameStart()             // blocking home UI
Role(world) / Scene(world)
open MCI aliases, play music_bg repeat
Timer gameTimer
loop forever
```

`gameStart()` does not return until the player clicks 开始 or a valid 读档. 退出 calls `exit(0)`.

## One frame

```text
key = gameCtrl.getKey()
if key == VIR_RESTART:  rebuild Role + Scene for current world; rewind BGM
if key == VIR_HOME:     life=5, world=1, home screen, rebuild, play BGM
Role::action(key, &scene, world)
Scene::action(&role)
if role.isDied():       freeze frame 3.5s → lose a life → die UI or game over
if role.isPassed():     stop BGM → win sting → next world or all-clear
BeginBatchDraw
  scene.show()
  role.show()
  hud score + level
EndBatchDraw
Timer::Sleep(TIME * 1000)        // 10 ms target
```

`TIME` is `0.01` seconds. The sleep uses `QueryPerformanceCounter`, not `::Sleep` alone, so the frame time stays close to 10 ms when the machine can keep up.

## Death branch

1. Stop background music.
2. Draw one last batched frame (scene + role + HUD) so the death pose is visible.
3. Sleep 3500 ms.
4. `life--`.
5. If `life == 0`: play `music_end`, `showGameOver()` (6500 ms of the home strip), reset `life` and `world` to defaults, return to `gameStart()`.
6. Else: `showDied(life)` draws remaining-life cat icons (1–4) for 2000 ms, then rebuilds Role + Scene for the **same** world.

Score is not preserved across a death. A new `Role(world)` starts at 0.

## Clear branch

1. Stop background music.
2. If `world == 3`: play `music_win` (6500 ms), then `music_passedAll`, `showPassedAll()` (7800 ms), reset life + world, rebuild, `gameStart()`.
3. Else: play `music_win` (6500 ms), `world++`, `showPassed(world)` (2000 ms, prints the **next** level number), rebuild Role + Scene.

`isPassed()` becomes true when `Hero::x > XSIZE` after `Scene::isEnding` has set `ending` and the auto-walk has carried the cat off the right edge. Details in [10-levels.md](10-levels.md).

## Restart / home from pause

`Control::getKey()` keeps the last command bits in `Control::key`. Esc opens `pauseClick()`, which blocks on `GetMouseMsg()` and overwrites `key` with a virtual bit:

| Menu row | Stored key | `main` reaction |
| --- | --- | --- |
| 返回游戏 | `VIR_RETURN` (64) | ignored; loop continues |
| 重新开始 | `VIR_RESTART` (128) | new Scene + Role, BGM from 0 |
| 退出游戏 | `VIR_HOME` (256) | full reset to home |
| 进行存档 | writes `world`, then `VIR_RETURN` | resume |

`GetCommand()` uses `GetAsyncKeyState` and **ors** bits, so A+D+J can be true in one integer. `Role::action` looks at each bit independently.

## Draw order

Back to front, every frame:

1. Sky (`mapsky.bmp`), tiled horizontally, vertical offset `-(world-1)*YSIZE`
2. Tile runs + scenery animations
3. Coins, sparkle FX, mushrooms
4. Enemies, explosions, bullets, hero
5. HUD text (score top-left, world top-right)

`BeginBatchDraw` / `EndBatchDraw` wrap only the gameplay present. Home, pause, death, and clear screens do their own drawing (pause uses `FlushBatchDraw` inside a mouse loop).

## Why `Role::show` also steps bullets

`bullteFlying` is called from `show()`, not `action()`. The simulation tick for bullets is therefore tied to a successful present. If you ever skip drawing, bullets freeze. The portable example in `examples/08_score_rules` keeps combat stepping in the update path on purpose.
