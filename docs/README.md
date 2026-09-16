# Docs index

Notes for the personal 2020 EasyX build. They describe the code that is
already in `MaoLiAo/`, not a redesign.

| Note | Question it answers |
| --- | --- |
| [architecture.md](architecture.md) | Who owns the window, the hero, and the map? |
| [constants.md](constants.md) | What do the macros in `define.h` mean in SI-ish units? |
| [physics.md](physics.md) | How does one tick of `Inertia::move` change `v` and `x`? |
| [levels.md](levels.md) | What is actually placed in worlds 1–3? |
| [collision.md](collision.md) | When does a 32×32 sprite count as overlapping a tile? |
| [input-and-ui.md](input-and-ui.md) | How do `GetAsyncKeyState` bits become pause / save / HUD? |
| [sprites-and-audio.md](sprites-and-audio.md) | Which BMP row is the coin, and which MCI alias is jump? |
| [quirks.md](quirks.md) | Why can `define.h` be included twice, and why is World 2's goal sign missing? |
| [build.md](build.md) | How to open the `.sln` and where `res/` must sit. |

Runnable counterparts live under [`../examples`](../examples). When a formula
in these notes disagrees with `examples/`, trust the C++ source first, then
fix the note.
