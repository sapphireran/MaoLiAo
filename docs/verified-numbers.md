# Verified numbers

Values below come from `cd examples && make test` on this tree (g++ 13, C++17). They are the portable model’s output, not a Windows play session.

## Integrator

| Quantity | Value |
| --- | --- |
| Takeoff `vY` | `-sqrt(210)` = **−14.4913767462** m/s |
| Metre→pixel scale `101 / 3.5` | **28.85714286** |
| Apex tick | **48** (`t = 0.48 s`) |
| Apex height | **−3.499861 m** / **−100.996 px** (target −3.5 m / −101 px) |
| First jump tick | **−0.143414 m** / **−4.139 px** |
| Landing tick (back through 0) | **97** |

`u = (V_MAX / T) / G`

| Tile time | `u` |
| --- | --- |
| `T1 = 0.5` (pipe / world-3 solids) | **0.5333333333** |
| `T2 = 1.2` (grass, snow, clouds) | **0.2222222222** |
| `T3 = 1.5` (default) | **0.1777777778** |

Friction helper `k` is **−2** when `vX > 0` and **+2** when `vX < 0`, so a coast from `V_MAX` on grass uses `a1 = −13.333…`. First run tick from rest: `v = 0.2`, `dx = 0.001` m (**0.02886 px**). At `V_MAX` with net `a = 0`, a tick is **2.30857 px**.

## Camera

Hold `D` from spawn `(64, ·)`:

| Event | Tick / value |
| --- | --- |
| First rail + scroll | tick **91** |
| After 400 ticks | `screen_x = 192`, `x0 ≈ −620`, `v = 8`, distance **812** |
| Sky step at cap on the rail | **−0.461714** px (`V_MAX * TIME * 101/3.5 / 5`) |
| World 1 finish identity | `x=192`, `x0=−2816` → distance **3008** |

## Maps and score

| World | Authored tiles | Loaded (`MAP_NUMBER=30`) | Coins | Enemies |
| --- | --- | --- | --- | --- |
| 1 | 32 | 30 (2 water dropped) | 20 | 10 |
| 2 | 36 | 30 (sign + trees dropped) | 11 | 6 |
| 3 | 30 (plus an off-by-one write in the game) | 30 | 7 random | 7 |

World 1 coin+stomp sweep: `20*10 + 10*5` = **250**. World 2 coins+shots: `11*10 + 6*5` = **140**. Lives: **5**.

## Collision

Grass platform `tile (0,9) × 15×1` is the pixel box `[0, 480] × [288, 320]`. A hero at `(64, 256)` does **not** hit until the game’s `(x, y+1)` ground probe. Pipe mouths (`id` 8, 10) are **64×64**; id 7 shafts stay **32** px wide. Ids 11–14 never enter `hitMap`. World 3 kills on every colliding id except clouds, unless `isShoot` is set.

## How to reproduce

```bash
cd examples && make test
```

Every binary prints `name: ok`. Python repeats the jump apex and the world-1/2 ASCII maps.
