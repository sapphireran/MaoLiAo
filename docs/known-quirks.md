# Known quirks

These are behaviors that are in the sources today. The examples either reproduce them on purpose (so a port stays honest) or isolate them behind a flag. They are not a cleanup patch — this pass only documents them.

## Include guard does not guard

```cpp
// MaoLiAo/define.h
#ifndef MYDEFINE
#define MYDIFINE
```

The macro that is defined is not the macro that is tested. `MYDEFINE` stays undefined, so every translation unit that includes `define.h` twice will re-see every `#define`. The project still links because each `.cpp` includes the header once.

## Trailing semicolons in macros

```cpp
#define F TIME*0.3;
#define LIFE 5;
```

`int life = LIFE;` becomes `int life = 5;;`, which is harmless. `scenery_iframe += F;` becomes `scenery_iframe += TIME*0.3;;`, also harmless. Using `F` or `LIFE` in a larger expression would break. The examples store `5` and `0.003` as real numbers, not as statement macros.

## `Y0` is unused

`define.h` sets `Y0` to `3 * HEIGHT` (96). `Role::Role` writes `myHero.x = X0; myHero.y = X0;` so the cat always spawns at **(64, 64)**, not (64, 96). Worlds 1 and 2 still land on the grass because gravity drops the sprite onto the first solid tile.

## `MAP_NUMBER` truncates authored maps

`MAP_NUMBER` is 30. World 1 lists 32 tiles; world 2 lists 36. The copy loop is `i < MAP_NUMBER`, so decorations at the end of those lists never appear. [levels.md](levels.md) lists exactly which rows are dropped. World 3 fits 30 tiles and then overruns the stack array (next item).

## Off-by-one `<= sizeof` loops

`createEnemy`, `createCoin`, `createFood`, and world 3 `createMap` iterate `i <= sizeof(arr)/sizeof(arr[0])`. The last iteration reads one past the local array. On a debug allocator this can crash; on a typical VS release build it often writes a zero-ish extra slot into `myEnemy` / `coins` / `food` / `map`.

The examples iterate `i < count` and keep a `faithful_off_by_one` flag in comments only. Tests assert the authored counts, not the overrun slot.

## Pause “退出游戏” is the title

The pause row labeled 退出游戏 returns `VIR_HOME`. `main` then resets lives, resets the world to 1, and calls `gameStart()`. Only the title row 退出 calls `exit(0)`.

## Latched keyboard mask

`getKey` refreshes the bit mask only when `_kbhit()` is true. `GetAsyncKeyState` is sampled in that moment and then reused. Combined with EasyX's console `_kbhit`, a frame can apply a key that is no longer down, and `Esc` can be missed if the console buffer did not see it.

## `CMD_DOWN` is a no-op

`S` sets `CMD_DOWN`. `Role::action` has an empty `if (KEY & CMD_DOWN)` block.

## Bullet simulation lives in `show`

`bullteFlying` is called from `Role::show`, not from `action`. If something skipped a draw (the death hold uses `show` once, then `Sleep`) shots freeze for that pause. The function name is misspelled in the header and the `.cpp`.

## `MAX_DISTANCE` is screen-space

`p->x > 480` uses the bullet's framebuffer x. After the camera has scrolled, a shot still dies around the middle-right of the window, not 480 px from the muzzle.

## World 3 death flag is mixed into `hitMap`

`hitMap(..., world)` may set `myHero.died` when `world == 3`. Callers that only wanted a blocking rectangle inherit that side effect. Landing probes pass the real world index; horizontal correction and bullets pass `1` and skip the lethal branch.

## Food in world 3 is 10×10 pixels

Worlds 1 and 2 place food in pixel coordinates near a cloud. World 3 uses `{10, 10}` — ten pixels from the top-left of the world, easy to walk past, and the hit box is still 52×25.

## Coin `(0, 0)` is both origin and “empty”

Slots initialized to `(0, 0)` are treated as unused. A designer cannot place a coin on tile (0, 0).

## `inertia.cpp` starts with `#pragma once`

The pragma is harmless on a `.cpp` but suggests the file was copied from a header. The class constructor / destructor are empty and unused; `move` is static.

## Character set split

`MaoLiAo.vcxproj` uses MultiByte for Debug and Unicode for Release. The UI strings are unprefixed `"开始"` / `"黑体"` literals. Release / Unicode builds are the configuration most likely to garble the menus.

## `F` scenery rate is tiny

`F` is `TIME * 0.3 = 0.003` frames per tick. Trees and water take `2 / 0.003 ≈ 667` ticks (6.7 s) to finish a two-frame loop. That is slow on purpose or an accidental extra zero — the sources do not say.

## Reconstruction re-opens MCI aliases

Every `Role(world)` call `open`s the SFX aliases again. EasyX / MCI usually replaces the alias; some runtimes leak a handle. `close all` in `main` is after `while (true)` and is not reached.

## `isHit` is corner-only

A full AABB overlap can exist without any of the actor's four inset corners sitting inside the target. Thin pipes in world 3 plus a 32×32 cat usually still connect; a smaller pickup or a 1-px ledge can miss. Documented in [collision.md](collision.md).

## Score sparkle uses tile math on pixel food

`setScorePos` is shared by coins (tile x/y) and food (pixel x/y). `Scene::show` always multiplies sparkle coordinates by 32. A food pickup therefore flashes the sparkle far to the right of the mushroom. Coins look correct.

## No `srand`

World 3 calls `random(1, 7)` / `random(3, 7)` with an unseeded `rand()`. The pipe field is stable for a given CRT but not authored.

The examples that need a pipe field use an explicit seed (`examples/maoliao_lib/worlds.py`, `seed=1` by default) so tests stay deterministic.
