# 00 — Overview

猫里奥 is a compact Super-Mario-inspired course project. The whole runtime is one process, one 512×384 EasyX window, and three C++ objects created in `main`:

- `Control gameCtrl` — menus, HUD, pause, save / load
- `Role gameRole(world)` — cat, enemies, bullets, score
- `Scene gameScene(world)` — tiles, coins, mushrooms, background

Two globals sit next to them:

```cpp
int life = LIFE;   // LIFE expands to `5;` (see constants notes)
int world = 1;
```

`life` and `world` are the only campaign state. Score lives on `Role`. The current world index is what `gameRecord.dat` stores.

## What this project is

- A **Windows desktop** game built against **EasyX** (`graphics.h`) and **WinMM** (`mciSendString`).
- A teaching sample of a 2020-era student engine: sprite sheets, bitmask input, kinematic jump, tile AABB, camera that pins the hero between `XLEFT` and `XRIGHT`.
- A personal archive. Comments, UI strings, and file names mix Chinese and English because that is how the course code was written.

## What this project is not

- It is not a company product and does not contain company code.
- It is not a cross-platform engine. EasyX is Windows-only.
- It is not a data-driven editor. Worlds 1 and 2 are literal `Map` / `POINT` / `Enemy` arrays in `scene.cpp` and `role.cpp`. World 3 randomizes pipe heights with `rand()`.

## Player fantasy

The cat walks right through three themed stages. World 1 is a grassland with clouds, pipes, water, and a victory sign. World 2 is snow with stacked platforms. World 3 is a Flappy-Bird homage: randomized pipe pairs, infinite air jumps, and “touch a pipe and die” unless the weapon mushroom is active.

Five lives. Fall off the bottom (`y > YSIZE`) or walk into an enemy while not stomping and you lose one. Hit a pipe the wrong way in world 3 and you lose one. Clear world 3 and the home art strip plays the all-clear panel.

## Mental model

```
                 ┌──────────── Control ────────────┐
                 │  GetAsyncKeyState → command bits│
                 │  Esc → pause menu / virtual keys│
                 │  HUD: score + world             │
                 └───────────────┬─────────────────┘
                                 │ key
                                 ▼
┌──────── Role ────────┐   action()   ┌──────── Scene ────────┐
│ Hero kinematics      │─────────────▶│ map[] coins[] food[]  │
│ Enemy patrol         │◀─────────────│ xMap / xBg camera     │
│ Bullets + bombs      │   queries    │ tile + scenery blit   │
└──────────────────────┘              └───────────────────────┘
```

Every frame: read keys → `Role::action` → `Scene::action` → death / clear handling → `BeginBatchDraw` / show / `EndBatchDraw` → `Timer::Sleep(TIME * 1000)`.

## Why the docs exist

The original `README.md` was one sentence. The interesting design is buried in comments: friction coefficients derived from `V_MAX / Tn / G`, a camera that never lets the cat leave a 192-pixel horizontal band, world-3 multi-jump, and a four-corner hit test that treats scenery IDs 11–14 as non-solid. These notes keep that knowledge next to the code.
