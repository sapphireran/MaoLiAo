# Architecture

猫里奥 V2.0 is a single-threaded EasyX loop. Three objects share the frame:

```text
Control  →  key bits, menus, HUD, fade screens
Role     →  hero, enemies, bullets, bombs, score
Scene    →  sky, tiles, coins, food, scenery animation
```

`Role` and `Scene` point at each other through forward declarations. `Role::action` stores the
current `Scene*` so `Role::show` can keep flying bullets after the update step.

## Frame order (`main.cpp`)

1. `Control::getKey()` — `GetAsyncKeyState` bits; `Esc` opens the pause overlay.
2. Restart / home virtual keys rebuild `Role` and `Scene`.
3. `Role::action(key, &scene, world)` — jump, walk, shoot, stomp, pickups, deaths.
4. `Scene::action(&role)` — camera `xMap` follows `hero.x0`; sky shifts at 1/5 map speed.
5. Death: freeze a frame, lose a life, or show game-over and reset to world 1.
6. Clear: world 3 plays the full ending; otherwise `world++` and rebuild both objects.
7. `BeginBatchDraw` → scene → role → score → level → `EndBatchDraw`.
8. `Timer::Sleep(TIME * 1000)` with `TIME = 0.01` (100 Hz target).

Lives and the current world are **globals** (`life`, `world`). Menus and the save file read/write
`world` directly. That is why pause-save and home-load do not need to serialize the hero.

## Coordinate systems

There are three X values on the hero:

| Field | Meaning |
| --- | --- |
| `x` / `xx` | On-screen pixel position (int / double) |
| `x0` | Camera origin. Becomes more negative as the level scrolls right |
| World X | `-(int)x0 + x` — used for walk frames, ending checks, and tile tests |

The hero is clamped to `[XLEFT, XRIGHT]` = `[0, 192]`. Crossing 192 while not in the ending zone
subtracts the overflow from `x0` and snaps `x` back to 192. The map and coins are drawn at
`xMap + tile * 32` with `xMap = (int)hero.x0`, so the world slides under a mostly-fixed cat.

Vertical motion uses screen Y downward. Jump velocity is stored as a *physics* Y (up is negative)
and then scaled into pixels by `UNREAL_HEIGHT / REAL_HEIGHT`.

## Object pools

| Pool | Capacity | Sentinel |
| --- | --- | --- |
| Enemies | 30 | `turn == 0` (also `x == y == 0` after death) |
| Bullets | 30 | `x == y == 0` |
| Bombs | 5 | `x == y == 0` |
| Map tiles | 30 | `id == 0` |
| Coins | 70 | `x == 0` |
| Food | 5 | `x == 0` |
| Score pops | 5 | `x == y == 0` |

Empty slots are found by scanning for a zeroed entry. World 1 and 2 actually *author* more than 30
map tiles; `MAP_NUMBER` silently drops the tail. See [known-quirks.md](known-quirks.md).

## Sprite blit

EasyX draws masked sprites with two `putimage` calls: `SRCAND` on the black silhouette row, then
`SRCPAINT` on the color row. `role.bmp` holds the cat (walk / death / mirror). `ani.bmp` holds
enemies, bullets, bombs, coins, score pops, and the weapon. `map.bmp` is a vertical strip of tile
IDs; `scenery.bmp` is the animated grass / tree / water sheet.

## Music lifetime

`main` opens looping background plus win / all-clear / game-over. `Role` opens jump, coin, stomp,
weapon, bullet, and two explosion cues. Aliases are never closed until process exit
(`mciSendString("close all")` is after the infinite loop, so it is only reached if the loop is
broken). Pause and death stop or rewind `music_bg` by alias.

## What the portable examples cover

The EasyX window cannot run in this Linux environment. The `examples/` programs re-implement the
pure systems: inertia step, command bits, AABB vertex tests, save text, tile friction, walk-frame
parity, and the camera / ending math. They are the regression net for the numbers in `define.h`.
