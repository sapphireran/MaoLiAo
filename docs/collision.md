# Collision

Every overlap test in `Role` is the same idea: build the hero's four corners, shrink them by 1 px, and ask whether any corner sits inside the target's axis-aligned box.

```cpp
bool Role::isHit(POINT* p1, POINT* p2)
{
    for (int i = 0; i < 4; i++)
    {
        if (p1[i].x >= p2[0].x && p1[i].y >= p2[0].y
         && p1[i].x <= p2[1].x && p1[i].y <= p2[1].y)
            return true;
    }
    return false;
}
```

`p1` is four corners. `p2` is only **two** points: top-left and bottom-right of the other box. This is a point-in-rect test, not a full AABB vs AABB. A large box can overlap the hero without containing a corner (the hero sitting in the middle of a wide platform still works because the feet corners land inside). A thin spike that crosses the sprite's interior but misses the four inset corners can fail to hit. The 1 px inset also lets the hero slide along a wall without immediately counting as overlapping it.

The portable copy is `maoliao::corner_hits` in `examples/include/maoliao_model.hpp`.

## Hero box

Screen position `(x, y)` is the top-left of a 32×32 sprite. Corners used for tests are converted into **world pixels** by subtracting the camera:

```
r[0] = (-x0 + x + 1,      y + 1)
r[1] = (-x0 + x + 31,     y + 1)
r[2] = (-x0 + x + 1,      y + 31)
r[3] = (-x0 + x + 31,     y + 31)
```

`x0` is negative while scrolling, so `-x0 + x` is the world-space left edge.

## Tiles (`hitMap`)

Walk `Scene::map` while `id > 0 && id < 11` and `i < MAP_NUMBER`. Ids 11–14 are scenery (grass tufts, victory sign, water, trees) and **do not collide**.

Normal tiles:

```
tl = (map.x * 32, map.y * 32)
br = tl + (map.xAmount * 32, map.yAmount * 32)
```

Pipe mouths (`id == 8` or `id == 10`) use **64×64** cells (`2 * WIDTH`, `2 * HEIGHT`) times `xAmount` / `yAmount`. Id 7 (pipe shaft) stays on 32×32 cells even though `show()` blits it two tiles wide — the hit box is narrower than the art.

If a hit occurs and `world == 3` and `id != 2` and `isShoot == false`, the hero dies. Clouds (`id == 2`) stay safe landing pads in the pipe corridor. After a successful pickup, `isShoot` is true and world-3 solids no longer kill (the comment calls the pickup a 无敌金币 / star).

`hitMap` is reused with `world` forced to `1` for:

- wall tests after a horizontal step (so a wall does not count as a world-3 death);
- enemy foot probes (“is there ground in front of this walker?”);
- bullet vs tile.

Those calls still return the `Map*` so friction and rewind work; they just skip the death side effect.

## Coins (`hitCoins`)

Coins are stored in **tile** coordinates. Box is one 32×32 cell. On hit, `Role` plays `music_coin`, adds **10** to `score`, asks the scene to spawn a score-pop at that tile (`setScorePos`), and zeroes the coin slot. `Scene::show` animates four frames of the “points” strip in `ani.bmp` and then clears the pop.

`createCoin` copies with `while (i <= sizeof(p)/sizeof(p[0]))`, one past the last initialiser. The extra iteration reads off the end of the stack array and may write a garbage coin. See [known-quirks.md](known-quirks.md).

## Food / weapon (`hitFood`)

Food positions are **not** a single unit. World 1 stores `{14 * 32, 5 * 32}` (pixels). World 2 stores a pixel position with a one-third-tile nudge. World 3 stores `{10, 10}` — ten **pixels**, not tiles — so the pickup sits near the top-left of the world.

The hit box is `52 × 25.6` (`3*WIDTH/2 + 4` by `4*HEIGHT/5`), matching the blit size in `Scene::show`. On hit: `music_getWeapon`, a score-pop (the pop helper still treats the coordinates as tiles, so the burst may draw in the wrong place), and `myHero.isShoot = true`.

Shooting is gated on that flag. `J` then spawns a `Bullet` with the hero's facing. Hold-to-fire uses `TIME_INTERVAL_BULLET = 0.2` s. A bullet dies when it leaves the window, travels past `MAX_DISTANCE = 480`, hits a tile, or hits an enemy (`+5` and a bomb). Step size is `LEHGTH_INTERVAL_BULLET = 4` pixels per `show()` (note the typo in the constant name). Because `bullteFlying` is called from `show`, bullets only move on frames that draw — the same 10 ms cadence as `main`.

## Enemies (`hitEnemy`)

Active enemies have `turn != 0`. Box is 32×32 at the stored world-pixel origin. `createEnemy` multiplies the authored tile pair by `WIDTH`/`HEIGHT`.

Two gameplay outcomes share this test:

| Situation | Result |
| --- | --- |
| Hero falling (`vY > 0`) overlaps enemy | Stomp: `+5`, bomb, enemy slot cleared |
| Hero overlaps enemy and `vY <= 0` | Death |

Bullets use the same helper. Enemies walk `ENEMY_STEP = 1` world pixel when `(int)(enemy_iframe * 100) % 2 == 0`. They reverse if the body overlaps a tile **or** the probe one step ahead and one pixel down has **no** tile (edge of a platform).

## Bombs

Not colliders. `setBomb` finds a free slot in `bombs[5]` and `show()` plays four frames from `ani.bmp` rows 4–6, then frees the slot. Overlapping explosions reuse the first free index.

## What this is not

- No swept tests. Fast motion can tunnel through a 32 px tile if a future change raised `v` a lot; at the current `V_MAX` and 10 ms step the horizontal pixel step is about `8 * 0.01 * 28.857 ≈ 2.3` px, so tunneling is not the live bug.
- No separate “one-way” platforms. Clouds collide from every direction.
- No hurt boxes vs hit boxes. The same inset square is used for stomps, deaths, and pickups.

`examples/src/aabb_collision.cpp` walks a fixture table of these cases (corner in, corner miss, pipe 64×64, scenery skipped, world-3 death vs cloud).
