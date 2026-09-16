# Architecture

MaoLiAo is a single-window, single-threaded EasyX loop. There is no scene graph, no resource manager, and no data-driven loader: `main` constructs `Control`, `Role`, and `Scene`, then ticks them in a fixed 10 ms `Timer::Sleep`.

## Frame loop

`main.cpp` owns two globals: `life` (starts at `LIFE`, which is 5) and `world` (starts at 1). After `initgraph(512, 384)` and the title menu, the loop is:

```text
key = Control::getKey()
if key is VIR_RESTART  -> rebuild Scene(world), Role(world); rewind BGM
if key is VIR_HOME     -> reset life=5, world=1; title menu; rebuild both; rewind BGM
Role::action(key, scene, world)
Scene::action(role)          # camera: xMap = hero.x0; parallax xBg
if hero.died:
    show last frame 3.5 s
    life--
    if life==0: game-over art, title menu, world=1
    else: show remaining lives, rebuild current world
if hero.passed:
    if world==3: victory + all-clear, title menu, world=1
    else: world++, interstitial, rebuild
BeginBatchDraw / scene.show / role.show / HUD / EndBatchDraw
Timer::Sleep(TIME * 1000)    # 10 ms
```

`Role` and `Scene` are value-reassigned (`gameRole = Role(world)`), so every rebuild reloads bitmaps and re-opens the MCI aliases declared in `Role::Role`.

## Types

### `Control`

Input and UI only. `GetCommand` OR-s `GetAsyncKeyState` bits (`CMD_LEFT` … `CMD_ESC`). `getKey` latches the last combo while `_kbhit()` is true, then opens the pause overlay when Esc is down. Mouse hit-tests on the title and pause menus are hard-coded rectangles around `(256±45, 128+30k)`.

Virtual bits that never come from the keyboard:

| Bit | Name | Source |
| --- | --- | --- |
| 64 | `VIR_RETURN` | Pause → 返回游戏 / 进行存档 |
| 128 | `VIR_RESTART` | Pause → 重新开始 |
| 256 | `VIR_HOME` | Pause → 退出游戏 (label says exit; it is the title menu) |

HUD (`showScore`, `showLevel`) draws into the same back-buffer as the world.

### `Scene`

Owns:

- `Map map[MAP_NUMBER]` with `MAP_NUMBER = 30`
- `POINT coins[70]`, `food[5]`, floating-score slots `[5]`
- background / tile / scenery / coin / flower `IMAGE`s
- `xMap`, `xBg` for the camera

`createMap` / `createCoin` / `createFood` fill those arrays from literals (worlds 1–2) or `rand()` (world 3 pipes and coins). `show` walks `map[]` while `id > 0`, draws each block `xAmount × yAmount` times, then coins and the flower. IDs 1–7 are 32×32 cells from `map.bmp`. IDs 8 and 10 are 64×64 pipe mouths. IDs 11+ are 96×64 scenery frames with a mask+paint blit.

`isEnding(distance)` is a world-specific pixel threshold: `94*WIDTH`, `104*WIDTH`, `94*WIDTH`. When the hero’s world-x crosses it, `Role` sets `ending` and auto-walks right until `x > XSIZE` (off the 512 px window), which sets `passed`.

### `Role`

Owns the `Hero` struct (screen x/y, world origin `x0`, velocities, facing, flags), `Enemy[30]`, `Bullet[30]`, and `POINT bombs[5]`. Collision tests build a 4-point hero diamond inset by 1 px and test it against axis-aligned tile / coin / food / enemy boxes (`Role::isHit`).

`hitMap` only considers tiles with `0 < id < 11` (terrain and pipes). IDs 11–14 are decorative: no collision. That is why background grass, the victory sign, water, and trees never block or kill.

### `Inertia`

One static helper:

```text
X = v*t + 0.5*a*t*t
v = v + a*t
return X
```

Vertical motion scales that “meter” result by `UNREAL_HEIGHT / REAL_HEIGHT` so a 3.5 m jump becomes 101 px. Horizontal motion uses the same scale, which is why `V_MAX = 8` does not mean “8 px per tick.”

### `Timer`

`QueryPerformanceCounter` busy-wait with `::Sleep(1)` inside the spin. First call calibrates frequency. The class keeps static `m_oldclk`, so the 10 ms frame budget is measured from the previous wake-up, not from “now + 10 ms” after drawing.

## Data that is *not* loaded from disk

Levels are compiled in. `fixtures/levels/*.json` is a readable copy of those literals, not a format `Scene` reads. The only runtime file is `gameRecord.dat` (one integer: the world index).

## Camera

The hero is a screen-space sprite. World scroll is `Hero.x0` (named like an origin; it is a negative offset). When `x` would pass `XRIGHT` and the run is not in the ending auto-walk:

```text
x0 -= (x - XRIGHT)
x = XRIGHT
```

`Scene::action` sets `xMap = (int)hero.x0` and, if the hero is pressed against `XRIGHT` with `vX > 0`, nudges `xBg` left by

```text
|vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG
```

so the sky crawls at 1/5 the ground speed. `show` wraps `xBg` when it passes `-img_bg.width`.

## Rebuild vs. mutate

Death, clear, restart, and “home” all destroy and reconstruct `Role`/`Scene`. Mid-world state (score, flower, enemy deaths) is therefore per-life. Score is *not* written to `gameRecord.dat` — only `world`.
