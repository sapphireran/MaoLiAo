# Architecture

猫里奥 is a single-threaded fixed-step loop. There is no scene graph, no entity component system, and no asset packer. Three objects plus two globals carry the whole run.

```
main()
  Control gameCtrl
  Role    gameRole(world)
  Scene   gameScene(world)
  Timer   gameTimer
  loop
      key = gameCtrl.getKey()
      maybe reconstruct Role/Scene on VIR_RESTART / VIR_HOME
      gameRole.action(key, &gameScene, world)
      gameScene.action(&gameRole)
      death?  -> lose a life, maybe title
      clear?  -> world++, maybe credits
      BeginBatchDraw
          gameScene.show()
          gameRole.show()
          gameCtrl.showScore / showLevel
      EndBatchDraw
      gameTimer.Sleep(TIME * 1000)
```

`Role` and `Scene` point at each other through a forward-declared `Scene*` / `Role*`. Headers stay acyclic because only pointers and references cross the boundary.

## Globals

`main.cpp` owns two process-wide integers:

| Global | Start | Who writes it |
| --- | --- | --- |
| `life` | `LIFE` (5) | `main` on death / game over / return to title |
| `world` | 1 | `main` on clear; `Control::gameStart` on 读档; `Control::pauseClick` on 存档 reads the current value |

`control.cpp` declares `extern int world` so the pause-menu save and the title-menu load can see the same integer `main` uses to reconstruct `Role` and `Scene`.

There is no score global. `Role::score` is the only tally; the HUD reads it each frame with `gameRole.getScore()`.

## The three objects

### `Control`

Input and chrome. It does **not** move the cat.

- `GetCommand()` ORs `GetAsyncKeyState` bits into the `CMD_*` mask from `define.h`.
- `getKey()` only refreshes that mask when `_kbhit()` is true, then opens the pause overlay if `CMD_ESC` is set. Because the mask is stored on the object, a held key keeps applying after the keyboard buffer goes quiet — until another `_kbhit()` sample replaces it.
- `gameStart()` is a blocking mouse loop for 开始 / 介绍 / 指导 / 退出 / 读档.
- `pauseClick()` is a blocking mouse loop for 返回游戏 / 重新开始 / 退出游戏 / 进行存档.
- `showScore`, `showLevel`, `showDied`, `showGameOver`, `showPassed`, `showPassedAll` are immediate-mode blits.

Virtual result bits (`VIR_RETURN`, `VIR_RESTART`, `VIR_HOME`) are stuffed into the same `key` integer the gameplay loop already reads.

### `Role`

Everything that walks or explodes.

| Member | Capacity | Notes |
| --- | --- | --- |
| `Hero myHero` | 1 | Position, velocity, facing, fly/shoot/dead/ending/passed |
| `Enemy myEnemy[]` | `ENEMY_TOTE` (30) | `turn == 0` means unused |
| `Bullet bullets[]` | `BULLET_NUMBER` (30) | `(0, 0)` means unused |
| `POINT bombs[]` | `BOMB_NUMBER` (5) | Spawned on stomp, shot, or wall hit |
| `score` | int | Coins and kills |

`action()` is the simulation tick: jump, gravity, run, friction, camera lock, enemy patrol, coin/food/enemy tests, shoot gate. `show()` is the sprite tick: walk frames, enemy frames, bomb frames, and it is also where `bullteFlying()` advances live bullets (simulation mixed into draw).

### `Scene`

Everything that does not walk.

| Member | Capacity | Notes |
| --- | --- | --- |
| `Map map[]` | `MAP_NUMBER` (30) | Solid and decorative tiles |
| `POINT coins[]` | `COINS_NUMBER` (70) | Tile coordinates; `(0, 0)` is unused **and** is a real cell, so a coin cannot sit on tile (0, 0) |
| `POINT food[]` | `FOOD_NUMBER` (5) | Pixel coordinates, not tiles |
| `POINT score[]` | `SCORE_NUMBER` (5) | Short-lived “+points” sparkle after a pickup |
| `xBg`, `xMap` | doubles / ints | Parallax sky and camera origin |

`createMap` / `createCoin` / `createFood` run once in the constructor from hard-coded tables. `action()` only slides `xMap` / `xBg` from `Hero::x0` and `vX`. `show()` blits sky, tiles, coins, sparkles, food.

## Frame contract

One iteration of `main`'s `while (true)` is one tick of `TIME` seconds.

1. **Input** — possibly blocking (pause / title).
2. **Simulate hero and enemies** — `Role::action`.
3. **Simulate camera** — `Scene::action`.
4. **Resolve life / world** — reconstruct both objects on death or clear. Reconstruction re-runs constructors, which reload bitmaps and MCI aliases.
5. **Draw** — scene under role under HUD, inside `BeginBatchDraw` / `EndBatchDraw`.
6. **Cap** — `Timer::Sleep(10)` using `QueryPerformanceCounter`.

Death holds the last frame for 3.5 s, then either `showDied` (2 s) or `showGameOver` (6.5 s) plus a new title loop. A mid-game clear holds 6.5 s of victory audio before `showPassed`. Clearing world 3 plays the short win sting, then the long 通关 track under `showPassedAll`.

## Reconstruction instead of reset

There is no `Role::reset()`. `main` writes:

```cpp
gameRole = Role(world);
gameScene = Scene(world);
```

That is why pause **重新开始** and a lost life feel identical: both throw the objects away. Score returns to 0. Enemies and coins respawn. The weapon flag `isShoot` is lost unless the player picks the food up again.

Returning to the title (`VIR_HOME` or game over) also resets `life = LIFE` and `world = 1` (game over / home) except 读档, which overwrites `world` before the objects are built.

## Draw order and transparency

EasyX has no alpha channel in this project. Every sprite that needs a hole uses the two-blit trick:

1. `SRCAND` the mask row (black silhouette, white background).
2. `SRCPAINT` the color row.

`role.bmp` stores mask rows under color rows. `ani.bmp` and `scenery.bmp` do the same with larger cells for bombs, food, trees, and water.

The sky (`mapsky.bmp`) is an opaque blit, stretched to `XSIZE x 4 * YSIZE` so each world can pick a vertical slice: `yBg = -(world - 1) * YSIZE`.

## Audio

`main` opens four looping / sting aliases (`music_bg`, `music_win`, `music_passedAll`, `music_end`). `Role`'s constructor opens the gameplay SFX aliases (`music_died`, `music_jump`, `music_coin`, …). Aliases are never closed until process exit (`mciSendString("close all")` is after an infinite loop, so it is only reached if someone later breaks the loop). Reconstructing `Role` calls `open` again on the same alias names.

## Why the examples exist

The loop above is bound to `graphics.h`, `windows.h`, and MCI. `examples/maoliao_lib` lifts the numbers and the `action` formulas into plain functions so the architecture can be unit-tested without a framebuffer. The class split (Control / Role / Scene / Inertia / Timer) is unchanged in spirit: input bits in, kinematic step, AABB tests, world tables, HUD-less score.
