# MaoLiAo documentation

These notes describe the **personal** 猫里奥 V2.0 sources in `MaoLiAo/`. They are written from the C++ that is in this repository, not from a design spec.

If a comment in the code disagrees with a function body, the **function body** wins. Several comments are leftover from earlier map layouts.

## Map of the notes

| Document | What it covers |
| --- | --- |
| [architecture.md](architecture.md) | Classes, global `life` / `world`, draw order |
| [game-loop.md](game-loop.md) | `main` loop, death, clear, music aliases |
| [physics.md](physics.md) | `Inertia::move`, jump velocity, friction `u` |
| [collision.md](collision.md) | `isHit`, `hitMap` / coins / food / enemies |
| [level-design.md](level-design.md) | `Map {x, y, id, xAmount, yAmount, u}` |
| [tile-catalog.md](tile-catalog.md) | Sprite rows and collision vs decoration |
| [controls.md](controls.md) | `CMD_*` bits, pause menu, title screen |
| [save-format.md](save-format.md) | `gameRecord.dat` |
| [resources.md](resources.md) | `res\` files and sprite-sheet coordinates |
| [build.md](build.md) | Visual Studio, EasyX, character set |
| [known-quirks.md](known-quirks.md) | Bugs that affect tools and ports |

Runnable counterparts live in [`../examples`](../examples). When you change a constant in `define.h` or a `createMap` table, update `examples/data/` and re-run the tests.

## Source of truth

```
define.h          macros (TIME, G, V_MAX, LIFE, command bits)
main.cpp          life machine and batch draw
control.cpp       GetAsyncKeyState + mouse UI
scene.cpp         createMap / createCoin / createFood / show
role.cpp          action(), hit-tests, bullets
inertia.cpp       one kinematic step
timer.h           high-resolution Sleep
```

World 3 maps are **not** fully static: pipe gap heights use `random(1, 7)` at scene construction. The examples expose that generator so you can freeze a seed and print a layout.
