# Architecture

The homework is five translation units plus a header-only timer. EasyX (`graphics.h`) owns the window; `Winmm` owns the mp3 aliases.

```
main.cpp
  ├─ Control   home / pause / HUD / interstitial art
  ├─ Role      hero kinematics, enemies, bullets, bombs, score
  ├─ Scene     tiles, coins, food, sky + map camera
  ├─ Inertia   one static integrator
  └─ Timer     QPC-backed Sleep
```

`Role` and `Scene` only know each other through forward-declared pointers (`class Scene;` in `role.h`, `class Role;` in `scene.h`). The `Map` struct is defined once behind `#ifndef _MAP`.

## Globals

`main.cpp` keeps two process-wide integers that the UI also writes:

| Symbol | Start | Who changes it |
|---|---|---|
| `life` | `LIFE` (5) | decremented on death; reset on game over / all-clear / home |
| `world` | `1` | incremented on a clear; set by “读档”; reset to 1 on home / all-clear |

`control.cpp` redeclares `extern int world` so the pause-menu save and the home-menu load can touch the same cell.

## Construction order (`main`)

1. `initgraph(XSIZE, YSIZE)` — 512×384.
2. `SetWindowText(..., "猫里奥 V2.0")`.
3. `Control gameCtrl` then `gameCtrl.gameStart()` (blocking mouse menu).
4. `Role(world)` / `Scene(world)` — both rebuild from the current `world`.
5. Open mp3 aliases (`music_bg`, `music_win`, `music_passedAll`, `music_end`) and loop the BGM.
6. Enter the infinite `while (true)` frame.

A restart reconstructs `Role` and `Scene` by assignment (`gameRole = Role(world)`). There is no heap level object.

## One frame (the `while (true)` in `main.cpp`)

```
key = gameCtrl.getKey()          # GetAsyncKeyState bits; Esc opens pause
if key == VIR_RESTART:           # pause → 重新开始
    rebuild Role + Scene(world); rewind BGM
if key == VIR_HOME:              # pause → 退出游戏 (label is 主菜单 in comments)
    life = 5; world = 1; gameStart(); rebuild; play BGM

gameRole.action(key, &gameScene, world)
gameScene.action(&gameRole)      # copy hero.x0 into xMap; maybe nudge xBg

if hero.died:
    freeze a frame, Sleep(3500), life--
    if life == 0: game-over art + music_end + home
    else: show remaining-life icons + rebuild this world
if hero.passed:
    stop BGM; if world == 3: win + all-clear + home
    else: Sleep(6500), world++, “LEVEL: N” card, rebuild

BeginBatchDraw
  scene.show(); role.show(); HUD score + level
EndBatchDraw
Timer.Sleep(TIME * 1000)         # 10 ms
```

`Role::show` also advances in-flight bullets (`bullteFlying`). Combat is therefore split: spawn / stomp / pickups in `action`, projectile travel in `show`.

## Class duties

### `Control`

- `GetCommand` — OR together `CMD_LEFT=1`, `RIGHT=2`, `UP=4`, `DOWN=8`, `SHOOT=16`, `ESC=32` from `GetAsyncKeyState`.
- `getKey` — if `_kbhit()`, replace `key`; if `CMD_ESC`, run `pauseClick()` which may return `VIR_RETURN=64`, `VIR_RESTART=128`, `VIR_HOME=256`.
- `gameStart` — five-button home (开始 / 介绍 / 指导 / 退出 / 读档).
- HUD: `showScore`, `showLevel`.
- Interstitials: `showDied`, `showGameOver`, `showPassed`, `showPassedAll`. Art comes from vertical strips of `res\home.bmp` (5 × 384 px tall).

### `Role`

Owned state:

- one `Hero` (pixel + double pose, `vX`/`vY`, facing, fly / shoot / died / ending / passed)
- `Enemy[30]`, `Bullet[30]`, `POINT bombs[5]`
- sprite `IMAGE`s from `res\role.bmp` and `res\ani.bmp`
- `score`

Public queries used by `main` and `Scene`: `getHero()`, `isDied()`, `isPassed()`, `getScore()`.

### `Scene`

Owned state:

- `Map map[30]`, `POINT coins[70]`, `POINT food[5]`, floating-score slots
- `xBg` / `xMap` camera
- `world`

`createMap` / `createCoin` / `createFood` run once in the constructor. `action` only follows the hero origin. `show` blits sky (looped), tiles, coins, pickups.

### `Inertia`

One function: `static double move(double& v, double t, double a)`. See [physics.md](physics.md).

### `Timer`

Header-only QPC clock. First `Sleep` latches frequency; later calls add `ms * freq` onto a running `m_oldclk` so a slow frame does not drift the *next* deadline backward — unless the frame already overran, in which case `m_oldclk` snaps to “now”.

## Music aliases

Opened in `main` (looping BGM + three stingers) and again in `Role::Role` (jump, coin, death, stomp, weapon, bullet, wall boom, enemy boom). Paths are `res\<中文>.mp3`. They are never `close`d until the unreachable `close all` after the infinite loop.

## Compile graph

`MaoLiAo.vcxproj` lists:

| Compile | Include |
|---|---|
| `main.cpp` | `timer.h` `define.h` `control.h` `role.h` `scene.h` |
| `control.cpp` | `control.h` `define.h` |
| `role.cpp` | `inertia.h` `define.h` `role.h` |
| `scene.cpp` | `scene.h` `role.h` `define.h` |
| `inertia.cpp` | `inertia.h` |
| `MaoLiAo.rc` | `IDI_ICON1` → `MaoLiAo.ico` |

`timer.h` is include-only. `inertia.cpp` also has `#pragma once` at the top of a `.cpp` (harmless, unusual).
