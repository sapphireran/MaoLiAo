# Scoring, lives, and clear conditions

## Points

`Role::score` starts at 0 every time a `Role` is constructed (new world,
restart, death respawn). It is **not** written to `gameRecord.dat`.

| Event | Points | Side effects |
| --- | --- | --- |
| Touch a coin | +10 | coin slot cleared, score-pop sprite, `music_coin` |
| Stomp an enemy (`vY > 0`) | +5 | enemy cleared, bomb sprite, `music_tread` |
| Bullet hits an enemy | +5 | enemy cleared, bomb sprite, `music_boom2` |
| Eat the flower | 0 | `isShoot = true`, `music_getWeapon` |
| Bullet hits a tile / max range | 0 | bomb sprite, `music_boom` |
| Die | 0 | `music_died`, then the life screen |

A world 1 perfect (20 coins, 10 stomps, no shots) is 200 + 50 = 250. Shooting
an enemy you could have stomped is the same 5 points. There is no time
bonus and no leftover-life bonus.

`Scene::score[]` is not the point total. It is a five-slot ring of
“where should the sparkle play.” `setScorePos` finds the first (0,0) slot.

## Lives

`LIFE` is written as `#define LIFE 5;` — the trailing semicolon is part of
the replacement list. `int life = LIFE;` still compiles because it expands
to `int life = 5;;`. Do not write `LIFE + 1` in a larger expression if you
copy that macro; use the numeric 5 or the portable `kLifeCount` in the
examples header.

`main` decrements after the 3.5 s death pose:

```
life--
if life == 0:  Game Over → title, life = 5, world = 1
else:          showDied(life), rebuild current world
```

Pause → 退出游戏 also resets `life = LIFE` and `world = 1` before
`gameStart()`. Pause → 重新开始 does **not** restore lives; it only
rebuilds `Role` and `Scene` for the current world.

## World clear

Two flags on `Hero`:

1. `ending` — `Scene::isEnding(distance)` became true. Distance is
   `-(int)x0 + x`, the hero’s world-space x. From then on `CMD_RIGHT` is
   forced and jump is blocked (except the world 3 flap still looks at
   `CMD_UP && world == 3` first).
2. `passed` — screen `x > 512` during that auto-walk.

`main` then:

- World 1 or 2: play victory, wait 6.5 s, `world++`, show `LEVEL: N`,
  construct a new Role/Scene.
- World 3: play victory, wait 6.5 s, play the credits track, show the
  credits strip of `home.bmp` for 7.8 s, reset life and world, return to
  the title.

There is no unlock gate. Starting from a save file of `3` jumps you
straight into the pipe gauntlet.

## HUD and overlays

| Function | Asset strip | Duration |
| --- | --- | --- |
| `gameStart` | `home.bmp` y-band 0 | until a start / load click |
| `showDied` | `home.bmp` y-band 2 + cat icons | 2 s |
| `showPassed` | `home.bmp` y-band 2 + “LEVEL:” | 2 s |
| `showGameOver` | `home.bmp` y-band 1 | 6.5 s |
| `showPassedAll` | `home.bmp` y-band 3 | 7.8 s |

`home.bmp` is loaded as 512 × (5 × 384) so each band is one screen tall.
Band 4 is unused by the current calls.

## Music versus score

Audio does not affect the counter. The only coupling is that `Role::action`
and `bullteFlying` fire an MCI alias on the same branch that adds points.
If MCI fails (missing MP3), scoring still happens.

## Portable table

`examples/include/maoliao/scoring.hpp` exposes:

```
kCoinPoints          10
kStompPoints          5
kBulletEnemyPoints    5
kLifeCount            5
kDeathPoseMs       3500
kVictoryWaitMs     6500
kCreditsWaitMs     7800
kPassedLevelWaitMs 2000
kDiedBannerMs      2000
```

`demo_scoring.cpp` runs a scripted pickup sequence (7 coins, 2 stomps, 1
shot, 1 flower) and asserts the total is `70 + 10 + 5 + 0 = 85`.
`test_scoring.cpp` also walks the life machine down to Game Over.
