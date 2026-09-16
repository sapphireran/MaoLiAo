# Known issues

Personal notes on the current `MaoLiAo/` sources. The portable examples avoid these; the game is left as shipped.

## Undefined behavior

1. **Off-by-one fill loops.** `createEnemy`, `createCoin`, `createFood`, and world 3 `createMap` use `while (i <= sizeof(a)/sizeof(a[0]))`. The last iteration reads past the local stack array. World 1/2 `createMap` use `m[i].id` as the stop condition instead (safer, until `MAP_NUMBER`).
2. **`MAP_NUMBER` overflow.** Worlds 1 and 2 author 32 and 36 tile records; only 30 are stored. World 2’s goal sign and trees are at the end of the table and are the first things dropped.
3. **Unseeded `rand()`** in world 3. Pipe and coin heights are often identical every launch.
4. **Null `FILE*`** on 读档 if `gameRecord.dat` is missing.

## Macros

5. `#define MYDIFINE` does not match `#ifndef MYDEFINE`, so the include guard never arms.
6. `#define LIFE 5;` and `#define F TIME*0.3;` carry stray semicolons.
7. `max` / `random` macros will collide with `<windows.h>` `max` if NOMINMAX is off (EasyX typically survives this).

## Logic / UX

8. Hero constructor sets `y = X0` (64), not `Y0` (96). The `Y0` macro is unused.
9. Pause “退出游戏” goes to the title (`VIR_HOME`); only the title “退出” kills the process.
10. `getKey` updates the bitmask only on `_kbhit()`, so `GetAsyncKeyState` results can stick.
11. Bullet `MAX_DISTANCE` is compared to screen `x`, not world travel.
12. World 3 food at pixel `(10,10)` is easy to confuse with an uninitialized slot.
13. Coin / bomb / food empty-slot sentinel is `(0,0)`, which is also a legal coordinate.
14. `showDied` has no `life == 5` drawing branch (only reached after a decrement, so 4..1).
15. `MessageBox` caption/text for a bad save look reversed.
16. Recursive `gameStart()` on 返回 from 介绍/指导.
17. `Role::show` moves bullets; pausing the debugger on a frame with shots advances them only when drawing continues.
18. Horizontal collision uses `hitMap(..., 1)` even on world 3, which is intentional for kill immunity during wall correction but means wall snaps never apply the world-3 death rule (death still happens from the earlier `y+1` / overlap calls that pass `world`).

## Dead code

19. `CMD_DOWN` and `CMD_ESC` branches in `Role::action` are empty (Esc is handled in `Control`).
20. `BULLET_INTERVAL` is unused.
21. `Scene::setFood` is unused (food is created in `createFood` only).
22. `closegraph()` / `mciSendString("close all")` after `while (true)`.

These are documentation, not a patch list. Fixing the game belongs in a separate change.
