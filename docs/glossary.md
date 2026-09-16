# Glossary

Short names as they appear in the 2020 sources and the portable examples.

| Term | Meaning |
| --- | --- |
| **猫里奥 / MaoLiAo** | The game and the hero. Course project, 2020. |
| **world** | Integer 1–3. Global in `main.cpp`, copied into `Scene`. |
| **life** | Remaining continues. Starts at `LIFE` (5). Not saved. |
| **Hero** | Player struct: screen `x/y`, doubles `xx/yy`, camera `x0`, `vX/vY`, flags. |
| **x0** | Camera / “origin.” World X is `-x0 + x`. |
| **XRIGHT** | 192 px. Right rail of the walk box. Overflow feeds `x0`. |
| **ending** | Auto-run flag after `isEnding(distance)`. |
| **passed** | `hero.x > 512` after ending. Triggers world advance. |
| **Map / tile** | `{x,y,id,xAmount,yAmount,u}` in cells. Ids 1–10 solid. |
| **u** | Friction factor written by `createMap`. |
| **isFly** | Airborne. Set on jump or walking off a ledge. |
| **isShoot** | Food collected. Enables `J` and world-3 pipe immunity. |
| **turn** | Facing: `+1` right, `-1` left. Enemy `turn == 0` means dead. |
| **iframe** | Fractional animation index (`hero_iframe`, `coin_iframe`, …). |
| **TIME** | 0.01 s tick. |
| **REAL_HEIGHT / UNREAL_HEIGHT** | 3.5 “meters” vs 101 px jump. |
| **Inertia::move** | `x = vt + ½at²`, updates `v`. |
| **CMD_*** | Key bits from `GetAsyncKeyState`. |
| **VIR_*** | Pause-menu results stuffed into the same `key` int. |
| **EasyX** | The Win32 graphics library (`graphics.h`, `putimage`). |
| **SRCAND / SRCPAINT** | Two-pass transparent blit (mask then color). |
| **MCI** | `mciSendString` aliases for mp3 playback. |
| **gameRecord.dat** | One integer: saved world. |
| **MAP_NUMBER** | 30. Hard cap on `Scene::map`. Worlds 1–2 overflow it. |
| **PWB** | Credit on the in-game introduction screen. |

See also [architecture.md](architecture.md) and [levels.md](levels.md).
