# Verified numbers

Printed by `cd examples && make test` on this agent (g++ 13.3.0, Ubuntu). Every lab exited 0 and `run_all` printed `ALL EXAMPLE DEMOS PASSED`.

| Check | Value |
|---|---|
| Jump `v0` | `-14.491377` m/s (`-√210`) |
| `UNREAL/REAL` | `28.857143` px/m |
| Discrete apex | `3.499861` m / `100.995984` px in **49** frames |
| First air frame from `y=64` | `yy=59.861488`, `vY=-14.191377` |
| Coast `V_MAX` → 0 on T2 | 60 frames |
| Coast `V_MAX` → 0 on T1 | 25 frames (faster stop) |
| Held-right `vX` | `8` (`V_MAX`) |
| Hero inset at spawn | `[65,65]–[95,95]` |
| Pipe mouth box | `64×64` |
| Food box | `52×25` |
| `RIGHT\|UP` mask | `6` |
| All command bits | `511` |
| Sample save | world `1` |
| Camera pin | `x=192`, `x0=-690` after 400 walk frames |
| Sky step at `V_MAX` | `0.461714` px |
| World 1 tiles | authored **32**, stored **30** (20 solid + 10 scenery) |
| World 2 tiles | authored **36**, stored **30** |
| Corridor patrol | 3 flips |
| World 3 generated tiles | **30** |
| World 1 all coins + stomps + 1 shot | `255` |
| Tick replay | land frame 66 at `y=256`, then pin at `x=192` |

These are the discrete homework step, not the closed-form 3.5 m / 101 px peak.
