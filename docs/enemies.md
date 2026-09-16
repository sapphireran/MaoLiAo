# Enemies

Walkers are the only hostile type. There are no flyers, shooters, or bosses. `Role` stores up to `ENEMY_TOTE = 30` of them. A slot is live when `turn != 0`. Clearing a slot (stomp or bullet) zeroes `x`, `y`, and `turn`.

## Authoring

`createEnemy` lists `{ tileX, tileY, turn }` and multiplies by `WIDTH` / `HEIGHT` so the stored position is already in **world pixels**. `turn` is `+1` (right) or `-1` (left) at spawn.

| World | Count | Notes |
| --- | --- | --- |
| 1 | 10 | From column 3 through 92, mixed facing |
| 2 | 6 | Clouds and snow ledges |
| 3 | 7 | Fixed tiles among the random pipes |

The copy loop uses `i <= sizeof...`, same off-by-one as coins. One extra garbage walker can appear; `turn == 0` slots are skipped in every later loop, so a zeroed overflow is harmless and a non-zero overflow is a ghost sprite.

## Tick

Enemies do **not** use `Inertia::move`. Once per two iframe-quantized beats they step one pixel:

```cpp
if ((int)(enemy_iframe * 100) % 2 == 0)
    enemy.x += enemy.turn * ENEMY_STEP;   // ENEMY_STEP = 1
```

`enemy_iframe` advances by `TIME * 5 = 0.05` each `show()`. `(int)(iframe * 100) % 2` therefore flips often; the patrol is a slow 1 px crawl, not a run.

After the step, two probes decide whether to reverse (`hitMap(..., world=1)` so world 3 does not kill the *hero* during an enemy test):

1. **Body** at the new `(x0 + enemy.x, enemy.y)` — overlapping a solid means the walker walked into a wall.
2. **Foot ahead** at `(x0 + enemy.x + turn * WIDTH, enemy.y + 1)` — *no* tile means the next 32 px in front has no ground, so this is a ledge.

Either condition multiplies `turn` by `-1`. The step already happened, so the sprite sits one pixel into the wall or over the edge until the next beat walks it back.

They do not jump, fall, or notice the hero. A walker whose platform is a single tile still reverses when the ahead-foot probe misses.

## Drawing

World-space `enemy.x` is blit at `x0 + enemy.x` (camera). Two frames from `ani.bmp` rows 0/1, same `SRCAND`/`SRCPAINT` pair as the hero. There is no left-facing strip; the art always faces one way.

## Combat

Shared AABB with the hero (see [collision.md](collision.md)):

| Relative motion | Result |
| --- | --- |
| Hero `vY > 0` (falling) overlaps | Stomp: `+5`, bomb, slot cleared |
| Hero `vY <= 0` overlaps | Hero dies |
| Bullet overlaps | `+5`, bomb, both slots cleared |

A jump that is still going up (`vY < 0`) into a walker is death, not a stomp. World 3 flaps spend most of their time with `vY < 0`, so stomps are the exception.

## Portable example

`examples/src/enemy_patrol.cpp` walks a 5-tile platform and checks that the sprite reverses at both the wall and the ledge, and that a stomp vs a rising jump is scored the way `Role::action` does.
