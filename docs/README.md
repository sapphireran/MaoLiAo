# MaoLiAo documentation index

Personal notes for the 猫里奥 V2.0 sources. Nothing here is company documentation.

Read these in order if you are new to the tree:

1. [Architecture](architecture.md) — modules, main loop, coordinate spaces
2. [Physics](physics.md) — gravity, jump, friction, speed cap
3. [Collision](collision.md) — four-corner AABB and death rules
4. [Levels](levels.md) — worlds 1–3, tiles, enemies, coins, food
5. [Input and UI](input-and-ui.md) — keys, pause, title, HUD
6. [Assets](assets.md) — bitmaps, sprite rows, MCI music aliases
7. [Save format](save-format.md) — `gameRecord.dat`
8. [Build](build.md) — Visual Studio + EasyX
9. [Known issues](known-issues.md) — header typos, off-by-one loops, overflow
10. [Adding content](adding-content.md) — how to place a platform, coin, or enemy
11. [Glossary](glossary.md) — names as they appear in the 2020 sources

Companion code that you can compile on Linux or Windows without EasyX:

- [`../examples/`](../examples/README.md)
