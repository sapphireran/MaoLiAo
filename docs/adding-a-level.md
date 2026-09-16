# Worked example: what a fourth world would touch

This is a personal design note, not an implemented feature. It lists every site a new `world == 4`
would have to mention, so a future evening session does not miss a global.

## 1. Loop and ending

`main.cpp` treats `world == 3` as the credits. A fourth stage needs that test to become
`world == 4` (or `>= kLastWorld`). Until then, clearing world 3 still resets to 1.

`Scene::isEnding` needs a `case 4:` distance. Reuse 94 tiles or pick a new length; the ending
walk itself (`ending` → forced right → `x > XSIZE`) does not care which world it is.

## 2. Authoring tables

| Function | What to add |
| --- | --- |
| `Scene::createMap` | `else if (world == 4)` tile list, `u` switch |
| `Scene::createCoin` | tile-space points |
| `Scene::createFood` | pixel-space weapon |
| `Role::createEnemy` | `{tx, ty, turn}` triples |

Stay under `MAP_NUMBER` (30), `COINS_NUMBER` (70), `FOOD_NUMBER` (5), `ENEMY_TOTE` (30). Prefer
`< sizeof` loops, not `<=`.

Sky: `yBg = -(world-1)*YSIZE` already indexes band 3 of `mapsky.bmp` (the bitmap is `4 * YSIZE`
tall, so world 4 is the last strip). A fifth world would wrap into empty memory.

## 3. Save validation

Home load rejects `flag >= 4`. That ceiling must become `>= 5` or the new file will be reported
as missing. Pause-save already writes whatever `world` is.

## 4. Ruleset

Decide which of world 3’s specials apply:

- Mid-air jump: `CMD_UP && world == 3` in `Role::action`.
- Lethal tiles: `world == 3 && id != 2 && !isShoot` in `hitMap`.

A “normal” world 4 should **omit** both so clouds and pipes behave like world 1.

## 5. HUD and cards

`showLevel` already prints any integer. `showPassed(world)` shows the **upcoming** world number
(`world++` happens first). After world 4 you want `showPassedAll`, not `showPassed(5)`.

## 6. Portable catalog

If the world is added, extend `examples/levels/world_catalog.cpp` with tile counts, ending
distance, and friction so Linux can keep checking the tables without EasyX.

Until then the catalog’s last world remains 3, matching the shipped course project.
