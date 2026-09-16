# 01 — Architecture

The solution is a single Visual C++ project (`MaoLiAo.vcxproj`) inside `MaoLiAo.sln`. There is no engine layer, no scene graph, and no third-party game framework beyond EasyX and the Windows multimedia API.

## Compilation units

```
MaoLiAo.sln
└── MaoLiAo/
    ├── main.cpp          entry + loop
    ├── define.h          macros
    ├── control.h/.cpp    input + UI
    ├── scene.h/.cpp      world geometry + pickups
    ├── role.h/.cpp       actors + combat
    ├── inertia.h/.cpp    kinematic step
    ├── timer.h           QPC sleep (header-only statics)
    ├── resource.h        IDI_ICON1
    ├── MaoLiAo.rc        icon resource
    └── gameRecord.dat    last saved world (plain text integer)
```

Headers use `#pragma once`. `Map` is guarded by `_MAP` and is declared in **both** `scene.h` and `role.h` so either include order works.

`Role` and `Scene` forward-declare each other. They only store or pass **pointers**, never embed the other type. That is the entire dependency inversion in the project.

## Ownership

| Object | Created | Recreated when |
| --- | --- | --- |
| `Control gameCtrl` | once in `main` | never (home art stays loaded) |
| `Role gameRole` | after home screen | death, clear, restart, home |
| `Scene gameScene` | after home screen | same as Role |
| `Timer gameTimer` | once | never |

Assignment `gameRole = Role(world)` relies on the compiler-generated copy. Images and MCI aliases are opened again in the new `Role` constructor. Old aliases are not explicitly closed on reconstruction; WinMM is treated as process-lifetime.

## Data that crosses objects

`Role::action` writes `this->myScene = myScene` so `Role::show` can fly bullets against the current map. `Scene::action` reads `Hero::x0` and `Hero::vX` to scroll the camera and the sky.

Pickups are mutated in place:

- `hitCoins` / `hitFood` return a `POINT*` into `Scene`'s arrays.
- The role zeros that point and calls `Scene::setScorePos` so the next blit can play a 4-frame sparkle.

Enemies and bullets live only on `Role`. The scene never knows they exist.

## Globals

```cpp
// main.cpp
int life = LIFE;
int world = 1;

// control.cpp
extern int world;   // pause-menu save + home-screen load
```

`life` is not saved. Loading a record restores only the world index.

## Coordinate systems

Three x-spaces show up in the same functions. Mixing them up is the usual source of “why is this hitbox wrong?” bugs.

| Space | Origin | Used for |
| --- | --- | --- |
| Screen | top-left of the 512×384 window | `Hero::x`, `Hero::y`, blits of the cat |
| World | scrolling origin `Hero::x0` (usually ≤ 0) | enemy `x`, coin tile coords × `WIDTH` |
| Tile | integer columns / rows | `Map::{x,y,xAmount,yAmount}`, coin cells |

Conversion used everywhere in `Role`:

```text
worldX = -hero.x0 + screenX
screenX =  hero.x0 + worldX
```

`Scene::show` draws tiles at `xMap + map.x * WIDTH`, and `xMap` is assigned from `hero.x0` each frame. Enemies are stored in world pixels and drawn at `hero.x0 + enemy.x`.

## Image / mask convention

Sprites are blitted twice: `SRCAND` (mask, usually the lower strip) then `SRCPAINT` (color, upper strip). That is EasyX's classic “cheap transparency” pattern. `role.bmp` and `ani.bmp` are laid out as color row + mask row pairs. See [05-scene-and-maps.md](05-scene-and-maps.md).

## Capacity constants

These are compile-time array sizes, not “how many exist in a level.”

| Constant | File | Meaning |
| --- | --- | --- |
| `MAP_NUMBER` 30 | `scene.h` | tile runs per world |
| `COINS_NUMBER` 70 | `scene.h` | coin slots |
| `SCORE_NUMBER` 5 | `scene.h` | simultaneous sparkle FX |
| `FOOD_NUMBER` 5 | `scene.h` | mushroom slots |
| `ENEMY_TOTE` 30 | `role.h` | enemy slots |
| `BOMB_NUMBER` 5 | `role.h` | explosion FX |
| `BULLET_NUMBER` 30 | `role.h` | live bullets |

A slot is “empty” when its coordinates (and, for enemies / bullets, `turn`) are zero. World-3 pipe generation writes up to `MAP_NUMBER` entries; the loop `while (i <= sizeof(m)/sizeof(m[0]))` is off-by-one and can read one past the local array. Documented in [design-notes.md](design-notes.md).

## Layering rule of thumb

If you are changing **feel** (accel, jump, friction), start in `define.h` + `inertia.cpp` + `Role::action`.

If you are changing **layout**, start in `Scene::createMap` / `createCoin` / `createFood` and `Role::createEnemy`.

If you are changing **flow** (lives, clear, save), start in `main.cpp` and `Control`.
