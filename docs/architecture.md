# Architecture

The 2020 source is a single Visual Studio Win32 console project. There is no engine layer: `main` constructs three objects and pumps them.

## Compilation units

| File | Responsibility |
| --- | --- |
| `MaoLiAo/main.cpp` | `initgraph`, window title, MCI aliases, infinite loop |
| `MaoLiAo/define.h` | Screen, tile size, timing, jump, run, command bits |
| `MaoLiAo/control.cpp` | Async keys, title / pause UI, score / level text, interstitials |
| `MaoLiAo/role.cpp` | Hero integration, enemies, bullets, bombs, pickups, death |
| `MaoLiAo/scene.cpp` | Tile list, coins, food, sky scroll, ending distance |
| `MaoLiAo/inertia.cpp` | One function: kinematic step |
| `MaoLiAo/timer.h` | Header-only QPC busy-wait sleep |

Headers use `#pragma once`. `define.h` also wraps an include guard named `MYDEFINE` / `MYDIFINE` (the names do not match — see [known-quirks.md](known-quirks.md)).

`Role` and `Scene` forward-declare each other. The shared `Map` struct is defined in both headers behind `#ifndef _MAP`.

## Types

### `Hero` (`role.h`)

Screen-space integer pose (`x`, `y`) plus sub-pixel doubles (`xx`, `yy`).
`x0` is the **camera origin** (usually ≤ 0). World-space X of the hero is `-x0 + x`.

| Field | Role |
| --- | --- |
| `vX`, `vY` | velocities in the “real meter” space before pixel scale |
| `turn` | `+1` face right, `-1` face left |
| `direction` | this-frame wish (`-1`/`0`/`+1` on x and y) |
| `isFly` | not standing on a solid |
| `isShoot` | weapon / star collected (`food` pickup) |
| `died` / `passed` | `main` checks these after `action` |
| `ending` | past `Scene::isEnding`; auto-walks right |

### `Enemy`, `Bullet`

Zeroed `{x,y,turn}` means “slot free”. `Role` keeps `ENEMY_TOTE = 30` and `BULLET_NUMBER = 30` slots.

### `Map` (`scene.h` / `role.h`)

Tile **in map-grid units**, not pixels:

```
x, y        top-left cell
id          1..14 (see levels.md)
xAmount     width in cells (pipes id 8/10 use 2× cell size when hit-tested)
yAmount     height in cells
u           friction coefficient used by Role::action
```

### `Control`

Holds the last command mask and the title atlas `img_bg` (`res\home.bmp` stretched to `XSIZE × 5*YSIZE`). Interstitials `putimage` that same atlas at different Y offsets.

## Ownership

```
main
 ├─ Control gameCtrl     lives for the process
 ├─ Role    gameRole     reconstructed on death, restart, menu, next world
 ├─ Scene   gameScene    same
 └─ Timer   gameTimer    stateless besides static QPC fields
```

`Role::action` stores the `Scene*` it was last called with so `Role::show` can keep flying bullets (`bullteFlying` — original spelling).

Score lives on `Role`. HUD reads it through `getScore()`.

## Constants that size arrays

From `role.h` / `scene.h`:

| Symbol | Value | Used for |
| --- | ---: | --- |
| `ENEMY_TOTE` | 30 | walking foes |
| `BOMB_NUMBER` | 5 | explosion sprites |
| `BULLET_NUMBER` | 30 | simultaneous shots |
| `MAP_NUMBER` | 30 | tile records per world |
| `COINS_NUMBER` | 70 | coin slots |
| `FOOD_NUMBER` | 5 | weapon / star slots |
| `SCORE_NUMBER` | 5 | floating “+10” pops |

World 1–2 layouts fit comfortably. World 3’s pipe generator writes several `Map` rows per column; the copy loop uses `<= sizeof/sizeof` and can read one past the initializer — documented in quirks.

## Rendering model

EasyX `IMAGE` sheets + two-pass transparency:

1. `putimage(..., SRCAND)` with the mask row
2. `putimage(..., SRCPAINT)` with the color row

Hero sheet `res\role.bmp`: walk frames on row 0, masks on row 1, death frame at column 2.
Enemy / coin / food / bomb / bullet all crop `res\ani.bmp` at different row offsets (see [assets.md](assets.md)).

`BeginBatchDraw` / `EndBatchDraw` wrap the playfield. Pause and title use `FlushBatchDraw` because they block on `GetMouseMsg`.

## Audio

`Winmm.lib` + `mciSendString`. Aliases opened once in `Role` ctor and `main`:

| Alias | File (under `res\`) |
| --- | --- |
| `music_bg` | 背景音乐.mp3 |
| `music_win` | 胜利.mp3 |
| `music_passedAll` | 通关.mp3 |
| `music_end` | 游戏结束.mp3 |
| `music_died` | 死亡1.mp3 |
| `music_jump` | 跳.mp3 |
| `music_coin` | 金币.mp3 |
| `music_tread` | 踩敌人.mp3 |
| `music_getWeapon` | 吃到武器.mp3 |
| `music_bullet` | 子弹.mp3 |
| `music_boom` | 子弹撞墙.mp3 |
| `music_boom2` | 子弹打到敌人.mp3 |

Restarting a cue uses `play ... from 0`. Background loops with `play music_bg repeat`.

## Why examples exist

The game cannot compile on this Linux agent (no EasyX, no Win32). The portable library in `examples/common/` copies the **numbers and predicates**, not the GDI calls, so the notes stay testable.
