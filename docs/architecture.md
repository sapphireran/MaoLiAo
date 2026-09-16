# Architecture

MaoLiAo is a single-threaded EasyX application. There is no entity component system and no scene graph. Four objects live for the life of a run, and `main` wires them together.

```
                 ┌─────────────┐
                 │   Control   │  keys, title, pause, HUD, save
                 └──────┬──────┘
                        │ command bits
                        ▼
┌──────────┐  action()  ┌──────────┐  camera follow  ┌──────────┐
│   Role   │───────────▶│  Scene   │◀────────────────│   Role   │
│ hero     │  hit tests │  tiles   │   xMap = x0     │  x0, vX  │
│ enemies  │◀───────────│  coins   │                 └──────────┘
│ bullets  │            │  food    │
└──────────┘            └──────────┘
        │
        │  Inertia::move(v, TIME, a)
        ▼
   position += v t + ½ a t²
```

## Objects

### `Control` (`control.h` / `control.cpp`)

Owns the current command bitmask and the title-screen background (`res\home.bmp`, stretched to `512 × 1920` so five 384-pixel pages sit in one image). Public surface:

- `getKey()` — OR of `GetAsyncKeyState` bits; `Esc` opens the pause overlay and may replace the bitmask with a virtual command (`VIR_RETURN`, `VIR_RESTART`, `VIR_HOME`).
- `gameStart()` — blocking title loop (start / intro / guide / exit / load).
- `showScore` / `showLevel` — HUD in the playfield corners.
- `showDied` / `showGameOver` / `showPassed` / `showPassedAll` — full-screen pages sliced from `home.bmp`.

`Control` never moves the hero. It only reports intent.

### `Role` (`role.h` / `role.cpp`)

Owns the hero, up to 30 enemies, 5 bomb flashes, 30 bullets, sprite sheets, and the integer score. `action(KEY, Scene*, world)` is the entire gameplay tick:

1. Jump if `CMD_UP` and (grounded, or world 3).
2. Integrate vertical motion with gravity `G = 30`.
3. Integrate horizontal motion with run accel `A_ROLE = 20` plus friction from the tile underfoot.
4. Clamp to the 192-pixel right rail and slide `x0` when the camera should scroll.
5. Step enemies, then resolve coins, food, and enemy contact.

`show()` picks walk frames from `role.bmp` / `ani.bmp` using `SRCAND` + `SRCPAINT` (EasyX's two-pass color-key blit).

### `Scene` (`scene.h` / `scene.cpp`)

Owns the tile array (`MAP_NUMBER = 30`), coin slots (`70`), score-pop slots (`5`), food slots (`5`), and the sky / tile / scenery bitmaps. `createMap` / `createCoin` / `createFood` hard-code each world. `action(Role*)` copies `hero.x0` into `xMap` and nudges the looping sky. `show()` walks tiles, then coins, then the short-lived “+score” burst, then food.

`isEnding(distance)` is the only win-distance check. `main` does not know tile counts; it asks the scene whether the hero's world-space `x` has crossed the finish line, then waits until the on-screen `x` exceeds `XSIZE` before flipping `passed`.

### `Inertia` (`inertia.h` / `inertia.cpp`)

A stateless helper with one public function:

```cpp
double Inertia::move(double& v, double t, double a)
{
    double X = v * t + 1.0 / 2.0 * a * t * t;
    v = v + a * t;
    return X;
}
```

Both axes go through this integrator. Jump and run are the same equation with different `a`.

### `Timer` (`timer.h`)

Header-only QPC spin/sleep. `main` calls `Timer::Sleep((int)(TIME * 1000))` so each iteration aims at **10 ms**. Death and clear screens use longer sleeps (3.5 s / 6.5 s) on the same helper.

## Coordinate systems

There are three x-axes. Mixing them up is the usual way to misread a hit test.

| Name | Stored on | Meaning |
| --- | --- | --- |
| Screen `x`, `y` | `Hero.x`, `Hero.y` | Pixels in the 512×384 window. `y` grows downward. |
| Subpixel `xx`, `yy` | `Hero.xx`, `Hero.yy` | Same space, `double`, so the 10 ms integrator does not quantize away. |
| World origin `x0` | `Hero.x0` | Camera. Starts at `0`. Becomes **negative** as the stage scrolls right. Distance traveled is `hero.x - (int)hero.x0`. |
| Tile grid | `Map.x`, `Map.y` | Integer tiles. Pixel box is `(x * 32, y * 32)` by `(xAmount * 32, yAmount * 32)`, except pipe mouths (`id` 8 and 10) which use 64×64 cells. |
| World pixels (enemies, food) | `Enemy.x`, some `food[i].x` | World-space pixels. Drawn at `x0 + enemy.x`. Coins are stored in **tiles**, food in world 1–2 is already in **pixels**. |

`Scene::xMap` is an `int` copy of `x0`. Drawing a tile at column `c` uses `xMap + c * WIDTH`.

Vertical camera is not a scroll. The sky strip is `4 * YSIZE` tall; `yBg = -(world - 1) * YSIZE` picks which band (grass / snow / pipe sky).

## Data ownership

```
main
 ├── int life          // starts at LIFE (5); not stored in Role
 ├── int world         // 1..3; written to gameRecord.dat
 ├── Control gameCtrl
 ├── Role   gameRole   // reconstructed on death, restart, world change
 └── Scene  gameScene  // reconstructed with the same events
```

`Role` holds a `Scene*` only for the duration of `action` / bullet flight. It is assigned at the top of `action` and used again from `show` when bullets move. After a restart, `main` assigns new stack objects; the old pointer is gone.

Lives are **not** inside `Role`. `main` decrements `life` after the death pose, then either shows remaining faces (`showDied`) or the game-over page.

## Header coupling

`role.h` and `scene.h` both need `Map` and a pointer to the other class.

- Each header forward-declares the other class.
- `Map` is defined once, guarded by `#ifndef _MAP`.
- Methods that need the full type (`Role::action`, `Scene::action`) live in the `.cpp` files, which include both headers.

That is why `getMap()`, `getCoins()`, `getFood()`, and `getHero()` return raw pointers: the headers can mention `Map` / `POINT` / `Hero` without pulling the other translation unit's internals.

## Music

`main` opens four looping/one-shot aliases (`music_bg`, `music_win`, `music_passedAll`, `music_end`). `Role`'s constructor opens the gameplay ones (jump, coin, stomp, shot, boom). All go through `mciSendString` and `Winmm.lib`. There is no mixer object; each call names an alias.

## What is *not* in the architecture

- No delta-time from a clock — the integrator always uses `TIME = 0.01`. If a frame is late, motion still assumes 10 ms.
- No resource manager — every constructor `loadimage`s the same BMPs again after a restart.
- No map file format — worlds are C arrays in `createMap`.
- No separate camera class — `x0` *is* the camera.

The portable model in `examples/include/maoliao_model.hpp` keeps this same split: constants, integrator, AABB, tiles, camera rail.
