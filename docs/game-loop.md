# Game loop

All of this is `MaoLiAo/main.cpp`. There is no scene-stack object: `main` is the state machine.

## Startup

```
initgraph(XSIZE, YSIZE)          // 512 × 384
SetWindowText(..., "猫里奥 V2.0")
Control::gameStart()             // blocks until 开始 or a valid 读档
Role(world)
Scene(world)
open + play music_bg repeat
```

`life` starts at `LIFE` (5). `world` starts at 1 unless 读档 overwrote it inside `gameStart`.

## Per-frame body

```
key = gameCtrl.getKey()

if key == VIR_RESTART:   rebuild Scene + Role for current world; rewind bgm
if key == VIR_HOME:      stop bgm; life=5; world=1; title; rebuild; play bgm

gameRole.action(key, &gameScene, world)
gameScene.action(&gameRole)

if gameRole.isDied():    freeze frame 3.5s; life--; game over or continue
if gameRole.isPassed():  stop bgm; world 3 => all-clear else world++ interstitial

BeginBatchDraw
  scene.show(); role.show(); showScore; showLevel
EndBatchDraw

Timer::Sleep(TIME * 1000)        // 10 ms target
```

`getKey` still returns the previous mask when no new `_kbhit` happened, so holding a key (or the pause virtual keys) latches.

## Death branch

1. Stop BGM, draw one last batched frame (corpse pose is `role.bmp` column 2).
2. `Sleep(3500)`.
3. `life--`.
4. If `life == 0`: play `music_end`, `showGameOver` (home.bmp strip at `y = -YSIZE`, 6.5 s), reset life and world, title, rebuild.
5. Else: `showDied(life)` draws 1–4 hero icons and waits 2 s, then rebuild the **same** world.

`showDied` has no branch for `life == 5` (you never land there: death already decremented). Four icons is the maximum drawn.

## Pass-level branch

`Role::action` sets `ending` when world-space X exceeds `Scene::isEnding` (94, 104, or 94 tiles × 32 px). Then it forces rightward acceleration until `x > XSIZE` (walks off the 512 px window) and sets `passed`.

- Worlds 1 and 2: play `music_win`, wait 6.5 s, increment `world`, `showPassed` (home.bmp at `-2*YSIZE` plus `LEVEL: N` text, 2 s), rebuild.
- World 3: play `music_win`, wait 6.5 s, play `music_passedAll`, `showPassedAll` (home.bmp at `-3*YSIZE`, 7.8 s), reset life and world, rebuild, **then** title.

Lives are **not** refilled when moving 1 → 2 → 3. They refill on all-clear or game over.

## Pause virtual keys

`Control::pauseClick` overwrites `key` with:

| Click | Value | `main` reaction |
| --- | ---: | --- |
| 返回游戏 | `VIR_RETURN` (64) | none (loop continues) |
| 重新开始 | `VIR_RESTART` (128) | rebuild current world |
| 退出游戏 | `VIR_HOME` (256) | title + reset life/world |
| 进行存档 | writes `world`, then `VIR_RETURN` | none |

The pause label says 退出游戏 / hover “The menu”, but the handler is the home-menu path, not `exit(0)`. Title “退出” **does** call `exit(0)`.

## Timing

`TIME = 0.01` is both the physics step and the frame budget. `Timer::Sleep` uses `QueryPerformanceCounter` and then `::Sleep(1)` in a spin until the deadline. If a frame overruns, it snaps `m_oldclk` forward so the next frame does not try to catch up — physics stays “one `TIME` per displayed frame”, not a fixed accumulator.

Sprite frame clocks (`enemy_iframe`, `coin_iframe`, …) also advance by `TIME * k` inside `show`, so animation speed is tied to the same step.

## Music lifetime

Aliases opened in `Role::Role` are never closed in the destructor. `main` only `close all` after the infinite `while (true)`, which never exits except via title 退出 / `exit(0)`. Reconstructing `Role` therefore `open`s the same aliases again; MCI usually tolerates that.

## Mapping to examples

`examples/06_side_scroller_sim` walks a tiny world with the same order: command → role step → camera → death/pass flags. It has no MCI and no batch draw.
