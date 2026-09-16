# Personal notes for 猫里奥

This folder documents the 2020 personal course build. It does not change the EasyX game binary.
Read in this order if you are coming back to the project after a long break:

1. [architecture.md](architecture.md) — objects, game loop, camera origin.
2. [physics.md](physics.md) — `Inertia::move`, jump, friction, speed cap.
3. [levels.md](levels.md) — tile IDs, world layouts, ending distances, pickups.
4. [input-and-ui.md](input-and-ui.md) — command bits, home / pause hit tests.
5. [save-and-audio.md](save-and-audio.md) — `gameRecord.dat` and MCI aliases.
6. [build.md](build.md) — Visual Studio configurations and EasyX charset notes.
7. [known-quirks.md](known-quirks.md) — include-guard typo, off-by-one loops, map cap.
8. [adding-a-level.md](adding-a-level.md) — worked example of how a fourth world would plug in.
9. [design-journal.md](design-journal.md) — personal notes on camera, friction, and world 3.

Runnable counterparts live under [`../examples`](../examples/README.md).
