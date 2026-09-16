#!/usr/bin/env python3
"""World tables, MAP_NUMBER truncation, ending distances, pickups."""

from __future__ import annotations

import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[0]))
sys.path.insert(0, str(HERE))

from maoliao_lib.constants import MAP_NUMBER, WIDTH  # noqa: E402
from maoliao_lib.worlds import (  # noqa: E402
    ENDING_DISTANCE,
    WORLD1_COINS,
    WORLD1_ENEMIES,
    WORLD1_FOOD,
    WORLD1_MAP_AUTHORED,
    WORLD2_MAP_AUTHORED,
    WORLD3_FOOD,
    authored_map,
    coins_for,
    dropped_map,
    enemies_for,
    food_for,
    is_ending,
    loaded_map,
    world3_pipe_field,
)


def test_authored_counts() -> None:
    assert len(WORLD1_MAP_AUTHORED) == 32
    assert len(WORLD2_MAP_AUTHORED) == 36
    assert len(world3_pipe_field(seed=1)) == 30
    assert len(WORLD1_COINS) == 20
    assert len(WORLD1_ENEMIES) == 10
    assert WORLD1_FOOD == ((448, 160),)
    assert WORLD3_FOOD == ((10.0, 10.0),)


def test_map_number_drops_world1_water_and_world2_decor() -> None:
    dropped1 = dropped_map(1)
    assert len(loaded_map(1)) == MAP_NUMBER
    assert [(t.x, t.y, t.id) for t in dropped1] == [(71, 10, 13), (75, 10, 13)]

    dropped2 = dropped_map(2)
    assert len(loaded_map(2)) == MAP_NUMBER
    ids = [(t.x, t.y, t.id) for t in dropped2]
    assert ids[0] == (42, 8, 11)
    assert ids[1] == (111, 1, 12)
    assert [row[2] for row in ids[2:]] == [14, 14, 14, 14]


def test_world3_fits_exactly() -> None:
    assert dropped_map(3, seed=1) == []
    assert len(loaded_map(3, seed=1)) == 30


def test_ending_thresholds() -> None:
    assert ENDING_DISTANCE == {1: 94 * WIDTH, 2: 104 * WIDTH, 3: 94 * WIDTH}
    assert not is_ending(1, 94 * WIDTH)
    assert is_ending(1, 94 * WIDTH + 1)
    assert is_ending(2, 104 * WIDTH + 1)


def test_friction_on_world1_strip() -> None:
    tiles = authored_map(1)
    high = [t for t in tiles if t.id == 6]
    assert len(high) == 1
    assert high[0].x == 67 and high[0].x_amount == 4
    grass = next(t for t in tiles if t.id == 1)
    assert high[0].u > grass.u


def test_enemy_pixels() -> None:
    e1 = enemies_for(1)
    assert e1[0] == (3 * 32, 8 * 32, 1)
    assert e1[-1] == (92 * 32, 6 * 32, 1)
    assert all(turn in (-1, 1) for _, _, turn in e1)


def test_world2_food_is_nudged_off_the_cloud() -> None:
    (fx, fy), = food_for(2)
    assert abs(fx - (39 * 32 - 32 / 3)) < 1e-9
    assert abs(fy - (3 * 32 + 32 / 5)) < 1e-9


def test_ascii_fixtures() -> None:
    from dump_layouts import FIXTURE_DIR, report

    for world in (1, 2, 3):
        path = FIXTURE_DIR / f"world{world}.txt"
        assert path.is_file(), path
        assert report(world, 1) == path.read_text(encoding="utf-8")


def test_seeded_world3_is_stable() -> None:
    a = world3_pipe_field(seed=1)
    b = world3_pipe_field(seed=1)
    c = world3_pipe_field(seed=2)
    assert a == b
    assert a != c
    assert coins_for(3, seed=1) == coins_for(3, seed=1)
    assert coins_for(3, seed=1) != coins_for(3, seed=2)


def main() -> int:
    testers = [
        test_authored_counts,
        test_map_number_drops_world1_water_and_world2_decor,
        test_world3_fits_exactly,
        test_ending_thresholds,
        test_friction_on_world1_strip,
        test_enemy_pixels,
        test_world2_food_is_nudged_off_the_cloud,
        test_ascii_fixtures,
        test_seeded_world3_is_stable,
    ]
    for fn in testers:
        fn()
        print(f"ok  {fn.__name__}")
    print(f"{len(testers)} level tests passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
