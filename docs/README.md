# MaoLiAo personal notes

These files describe the **checked-in source**, not a rewrite of the game. Paths are from the repository root. Line numbers shift; search the symbol names.

## Read order

1. [../README.md](../README.md) — play, build, layout
2. [architecture.md](architecture.md) — `main` loop and the four types
3. [physics.md](physics.md) — `Inertia::move`, friction `u`, jump, camera
4. [levels.md](levels.md) — tile IDs, the three `createMap` worlds
5. [controls-and-ui.md](controls-and-ui.md) — bits, menus, `gameRecord.dat`
6. [resources.md](resources.md) — `loadimage` / `mciSendString` names
7. [quirks.md](quirks.md) — include-guard typo, `MAP_NUMBER` overflow, save UI
8. [frame-walkthrough.md](frame-walkthrough.md) — one grounded walk frame
9. [../examples/README.md](../examples/README.md) — portable programs

## Source map

| Type | Header | Implementation | Job |
| --- | --- | --- | --- |
| entry | — | `MaoLiAo/main.cpp` | `initgraph`, music, die / pass / HUD |
| `Control` | `control.h` | `control.cpp` | async keys, title, pause, overlays |
| `Role` | `role.h` | `role.cpp` | hero, enemies, bullets, score |
| `Scene` | `scene.h` | `scene.cpp` | tiles, coins, food, draw |
| `Inertia` | `inertia.h` | `inertia.cpp` | one Euler step |
| macros | `define.h` | — | 512×384, `TIME`, `G`, command bits |
| `Timer` | `timer.h` | (inline in header) | high-resolution `Sleep` |

`Role` and `Scene` only forward-declare each other. `Map` is defined in both headers behind `#ifndef _MAP`.
