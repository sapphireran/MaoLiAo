#!/usr/bin/env python3
"""Corner-inset hits vs full AABB, tile / coin / enemy / world-3 rules."""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao_lib.collision import (  # noqa: E402
    Tile,
    boxes_overlap_full,
    coin_box,
    food_box,
    hero_corners,
    hit_coins,
    hit_enemy,
    hit_food,
    hit_map,
    is_hit,
)
from maoliao_lib.constants import HEIGHT, WIDTH  # noqa: E402
from maoliao_lib.inertia import friction_u  # noqa: E402


def test_flush_stand_on_grass() -> None:
    # Spawn-like pose: 32x32 sprite sitting on a grass row at y=9.
    grass = Tile(0, 9, 1, 15, 1, friction_u(1, 1))
    # Feet at y=256 would be flush with the tile top (9*32). A probe at y+1
    # (Role landing check) puts the bottom corners on y=288+31 = wait:
    # hero y such that bottom = 9*32 → y + 32 = 288 → y = 256.
    y = 8 * HEIGHT  # 256; tile top is 288. Probe y+1 used by the game.
    hit = hit_map(64, y + 1, [grass], xmap=0)
    assert hit is not None
    assert hit.tile.id == 1
    assert not hit.died


def test_air_above_grass_misses() -> None:
    grass = Tile(0, 9, 1, 15, 1, friction_u(1, 1))
    # One full tile above the surface.
    hit = hit_map(64, 7 * HEIGHT, [grass], xmap=0)
    assert hit is None


def test_pipe_mouth_is_2x2() -> None:
    pipe = Tile(36, 7, 10, 1, 1, friction_u(10, 1))
    box = pipe.aabb()
    assert box == ((36 * 32, 7 * 32), (36 * 32 + 64, 7 * 32 + 64))
    # Standing in the extra cell that a 1x1 tile would not occupy.
    hit = hit_map(36 * 32 + 40, 7 * 32 + 8, [pipe], xmap=0)
    assert hit is not None


def test_scenery_is_not_solid() -> None:
    sign = Tile(101, 7, 12, 1, 1, 0.0)
    hit = hit_map(101 * 32, 7 * 32, [sign], xmap=0)
    assert hit is None


def test_world3_pipe_kills_unless_star() -> None:
    pipe = Tile(10, 4, 10, 1, 1, friction_u(10, 3))
    cloud = Tile(10, 8, 2, 4, 1, friction_u(2, 3))
    lethal = hit_map(10 * 32 + 8, 4 * 32 + 8, [pipe], world=3, is_shoot=False)
    assert lethal is not None and lethal.died
    safe_star = hit_map(10 * 32 + 8, 4 * 32 + 8, [pipe], world=3, is_shoot=True)
    assert safe_star is not None and not safe_star.died
    safe_cloud = hit_map(10 * 32 + 8, 8 * 32 + 8, [cloud], world=3, is_shoot=False)
    assert safe_cloud is not None and not safe_cloud.died


def test_coin_and_food_and_enemy() -> None:
    coins = [(10, 5), (0, 0)]
    assert hit_coins(10 * WIDTH + 4, 5 * HEIGHT + 4, coins) == 0
    assert hit_coins(0, 0, coins) is None  # (0,0) slot is unused

    foods = [(14 * WIDTH, 5 * HEIGHT)]
    assert hit_food(14 * WIDTH + 4, 5 * HEIGHT + 4, foods) == 0
    fx, fy = foods[0]
    box = food_box(fx, fy)
    assert box[1][0] - box[0][0] == 52
    assert box[1][1] - box[0][1] == 25

    enemies = [(3 * WIDTH, 8 * HEIGHT, 1), (0, 0, 0)]
    assert hit_enemy(3 * WIDTH + 4, 8 * HEIGHT + 4, enemies) == 0
    assert hit_enemy(0, 0, enemies) is None


def test_corner_only_can_miss_a_full_aabb_overlap() -> None:
    # 32x32 hero at (0,0). A 10x10 box sitting on the hero's center overlaps
    # as a full AABB but contains none of the inset corners.
    corners = hero_corners(0, 0, 0)
    small = ((11, 11), (21, 21))
    hero_box = ((0, 0), (32, 32))
    assert boxes_overlap_full(hero_box, small)
    assert not is_hit(corners, small)


def test_coin_box_is_one_cell() -> None:
    assert coin_box(10, 5) == ((320, 160), (352, 192))


def main() -> int:
    testers = [
        test_flush_stand_on_grass,
        test_air_above_grass_misses,
        test_pipe_mouth_is_2x2,
        test_scenery_is_not_solid,
        test_world3_pipe_kills_unless_star,
        test_coin_and_food_and_enemy,
        test_corner_only_can_miss_a_full_aabb_overlap,
        test_coin_box_is_one_cell,
    ]
    for fn in testers:
        fn()
        print(f"ok  {fn.__name__}")
    print(f"{len(testers)} collision tests passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
