# One frame, in order

Assume world 1, hero on grass at screen `(64, 256)`, `x0 = 0`, `vX = 4`, `vY = 0`, `isFly = false`, no keys except D. `TIME = 0.01`.

## 1. Input

`_kbhit` true, `GetCommand` sees D → `key = CMD_RIGHT` (2). Esc is not set, so pause does not run.

## 2. `Role::action`

- Jump skipped (`CMD_UP` clear).
- Not flying: `hitMap(x, y+1)` finds the grass strip `{0,9,id 1}`. Stay grounded.
- `CMD_RIGHT` → `a = +20`, `direction.x = +1`.
- `vX` and `a` have the same sign, so `a1 = 0`.
- `H = integrate(vX, 0.01, 20) * 101/3.5`
  - `vX` becomes `4.2`
  - `H ≈ (4*0.01 + 0.5*20*0.0001) * 28.857 ≈ 1.157 px`
- `xx += H`. No wall. Distance `64` is far below 3008, so `ending` stays false.
- `|vX| = 4.2 < 8`. No shoot. `x` still `< 192`, so `x0` unchanged.
- Enemies that pass the iframe ticker move 1 px; a walker at a ledge flips `turn`.
- Coin / food / side-enemy tests use the new `x,y`.

## 3. `Scene::action`

`xMap = x0 = 0`. Sky does not step (hero is not pinned at `XRIGHT`).

## 4. Draw

`BeginBatchDraw`: sky band 0, tiles, coins, food, then hero frame (walk cycle from `(-x0+x)/STEP`), HUD score/level. `EndBatchDraw`.

## 5. Sleep

`Timer::Sleep(10)` aims at a 100 Hz logical rate. Overlays elsewhere use CRT `Sleep`.

## Death / clear (not this frame)

If `died`: freeze 3500 ms, `life--`, show remaining faces or Game Over, rebuild `Role`/`Scene`.  
If `passed` and `world < 3`: win sting 6500 ms, `world++`, intermission.  
If `world == 3`: extra credits track, reset to title.
