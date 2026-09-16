# Personal notes for 猫里奥

These pages describe the **personal** EasyX game in `MaoLiAo/`. They are study notes, not an API contract. File paths below are relative to the repo root.

## Read order

1. [architecture.md](architecture.md) — three objects plus two helpers
2. [game-loop.md](game-loop.md) — what happens every 10 ms
3. [physics.md](physics.md) — why a jump feels 3.5 m tall
4. [collision.md](collision.md) — four vertices vs tile AABB
5. [levels.md](levels.md) — tile IDs and the three worlds
6. [controls-and-save.md](controls-and-save.md) — bit flags and `gameRecord.dat`
7. [assets.md](assets.md) — where each `putimage` crops from
8. [known-quirks.md](known-quirks.md) — bugs left in the 2020 source

Runnable reconstructions live in [`../examples`](../examples). They compile with a stock `g++` and do not link EasyX.

## Mental model

```
                 ┌──────────── Control ────────────┐
 keyboard/mouse  │ getKey, pause, title, HUD, save │
                 └───────────────┬─────────────────┘
                                 │ int key (bit mask)
                 ┌───────────────▼─────────────────┐
                 │              Role               │
                 │  Hero + enemies + bullets       │
                 │  hits Scene tiles / coins / food│
                 └───────────────┬─────────────────┘
                                 │ Hero.x0 (camera origin)
                 ┌───────────────▼─────────────────┐
                 │              Scene              │
                 │  tiles, coins, food, sky        │
                 └─────────────────────────────────┘
```

`Inertia::move` is a static helper used only by `Role::action`.
`Timer::Sleep` is a static helper used only by `main`.

## Globals

`main.cpp` owns two process-wide integers:

| Name | Start | Meaning |
| --- | ---: | --- |
| `life` | `LIFE` (5) | remaining continues |
| `world` | `1` | current stage, 1..3 |

`Control.cpp` reads `world` when writing or loading `gameRecord.dat`.
`Role` and `Scene` take `world` as a constructor argument and rebuild their arrays.

## What these notes are not

- Not a walkthrough of EasyX itself.
- Not a change log of the original 2020 commits (those messages are in `git log`).
- Not company or course-submission paperwork. Personal archive only.
