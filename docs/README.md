# MaoLiAo design notes

These notes describe the **personal** 猫里奥 tree as it exists in this repository. They are written from the C++ sources, not from memory of a later rewrite.

Read them in this order if you are new to the project:

1. [architecture.md](architecture.md) — who owns the loop
2. [physics.md](physics.md) — why the cat slides and hops the way it does
3. [collision.md](collision.md) — the four-corner test used everywhere
4. [levels.md](levels.md) — tile IDs and the three worlds
5. [controls-and-save.md](controls-and-save.md) — keys, menus, `gameRecord.dat`
6. [resources.md](resources.md) — bitmaps, MP3 aliases, `home.bmp` slices
7. [known-quirks.md](known-quirks.md) — bugs the sources already contain

Runnable counterparts live in [`../examples`](../examples/README.md). When a formula here and a function there disagree, treat the C++ in `MaoLiAo/` as the source of truth and file a note.

## Source files and responsibilities

| File | Responsibility |
| --- | --- |
| `MaoLiAo/main.cpp` | `initgraph`, MCI playlist, `life` / `world` globals, death and clear flow |
| `MaoLiAo/define.h` | Screen size, timestep, jump meters, command bits |
| `MaoLiAo/control.cpp` | `GetAsyncKeyState` bits, title UI, pause UI, HUD, save/load |
| `MaoLiAo/role.cpp` | Hero kinematics, enemy patrol, bullets, bombs, coin/food/enemy hits |
| `MaoLiAo/scene.cpp` | Tile / coin / food arrays, parallax, sprite-sheet blit |
| `MaoLiAo/inertia.cpp` | One kinematic step; updates `v` by reference |
| `MaoLiAo/timer.h` | Multimedia-timer `Sleep` used as the frame cap |

## Numbers that everything else assumes

Copied from `define.h` (and used by the examples as `examples/maoliao_lib/constants.py`):

| Macro | Value | Role |
| --- | --- | --- |
| `XSIZE` / `YSIZE` | 512 / 384 | Window |
| `WIDTH` / `HEIGHT` | 32 / 32 | Tile and sprite cell |
| `TIME` | 0.01 s | Fixed step |
| `G` | 30.0 | Gravity used by jump and friction |
| `REAL_HEIGHT` | 3.5 | “Meters” a jump should feel like |
| `UNREAL_HEIGHT` | 101 | Pixel height of that jump (`3 * 32 + 5`) |
| `V_MAX` | 8.0 | Horizontal speed cap (pre-scale) |
| `A_ROLE` | 20.0 | Run acceleration (pre-scale) |
| `XRIGHT` | 192 | Camera lock; further motion scrolls the world |
| `LIFE` | 5 | Starting lives (macro is written `5;` — see quirks) |

## What these notes are not

- Not a Windows installer guide beyond the README build section.
- Not a rewrite of the EasyX game.
- Not company documentation. This tree is Sapphire's personal course project.
