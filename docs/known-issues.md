# Known issues

Personal notes on bugs and sharp edges in the 2020 sources. None of these are “company tickets.” The portable examples avoid the same pitfalls where they re-implement a formula.

## Header / macro

1. **`define.h` include guard is misspelled.** `#ifndef MYDEFINE` then `#define MYDIFINE`. Relies on `#pragma once`.
2. **`#define LIFE 5;` and `#define F TIME*0.3;` include a trailing semicolon.** `int life = LIFE;` expands to `int life = 5;;` (harmless). `scenery_iframe += F;` becomes `scenery_iframe += TIME*0.3;;` (also harmless). Do not write `if (LIFE)` style uses.
3. **`max` / `random` macros** will break `std::max` and any identifier named `random`.
4. **`LEHGTH_INTERVAL_BULLET` and `bullteFlying`** are misspelled but consistent. Rename both together.

## Map capacity

5. **`MAP_NUMBER` is 30; worlds 1 and 2 initialize more than 30 tiles.** The `i < MAP_NUMBER` clause stops the copy, so the last decorations may never appear.
6. **World 3 copy uses `while (i <= sizeof(m)/sizeof(m[0]))`.** That is one iteration past the array (and also reads `m[i].id` on that iteration). Same off-by-one appears in `createCoin`, `createFood`, and `createEnemy` (`i <= sizeof(...)`).
7. **World 2 comment `//???存在问题`** already flags the loop. The `id < 15` condition is what keeps unused zeroed slots from being treated as real tiles after a short copy — except world 3 overwrites by index including the overflow step.

## Gameplay / logic

8. **Sticky keys.** `getKey` only samples when `_kbhit()` is set, so a held chord can persist a frame too long and a quiet frame may keep the previous mask if no keyboard event arrives.
9. **World 3 death vs `world` argument.** Vertical / generic `hitMap(..., world)` can kill on pipe touch; the horizontal resolver passes `1` so that pass never applies the world-3 rule.
10. **Food in world 3 is at pixels (10, 10)** — almost off the top-left, easy to treat as “broken.”
11. **`srand` is never called.** World 3 “random” pipes are deterministic per CRT.
12. **Bullet `MAX_DISTANCE` is screen-space.** You cannot snipe far ahead of the camera.
13. **`showDied` has no sprite for `life == 5`.** You never see that splash with the default `LIFE` of 5 (first death goes to 4).
14. **Pause “退出游戏” is `VIR_HOME`, not process exit.** Title “退出” is `exit(0)`. The labels disagree.
15. **Stomp vs body hit in the same frame.** A falling overlap stomps; a later `hitEnemy` with `vY <= 0` can still kill if `vY` was zeroed by landing on a tile in the same `action` call. Rare but possible on tight geometry.

## Audio / resources

16. **`Role` re-`open`s MCI aliases every reconstruct** without `close`. After many deaths this can leak aliases or fail silently.
17. **No check that `loadimage` / `mciSendString` succeeded.** A missing `res/` yields a black window and no sound.
18. **Release + Unicode** will garble every Chinese `outtextxy` string (see [build.md](build.md)).

## Save file

19. **Null `FILE*` on failed open** in both save and load.
20. **`MessageBox` caption/text swapped** on bad save data.
21. **`fclose` skipped** on the validation-failure branch.

## Unreachable / dead code

22. **`main` never leaves the `while (true)`**, so `closegraph()` and `close all` do not run on a normal quit. Title “退出” uses `exit(0)`.
23. **`CMD_DOWN` and `CMD_ESC` branches in `Role::action` are empty.** Esc is handled in `Control`.
24. **`Scene::setFood` is never called** after the initial `createFood`.
25. **`Inertia` constructor/destructor are unused** (all calls are `Inertia::move`).

## Examples vs game

The `examples/` programs copy the **intended** formulas (integrator, AABB, command bits, save integer, world-1 table). They do not reproduce the off-by-one copy loops. If a demo and the exe disagree, trust the exe for “what the 2020 build does” and the demo for “what the comment said.”
