#!/usr/bin/env python3
"""Assertions for Inertia::move, jump apex, friction k, and V_MAX run-up."""

from __future__ import annotations

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao_lib.constants import (  # noqa: E402
    A_ROLE,
    G,
    PIXELS_PER_METER,
    REAL_HEIGHT,
    TIME,
    UNREAL_HEIGHT,
    V_MAX,
)
from maoliao_lib.inertia import (  # noqa: E402
    friction_k,
    friction_u,
    integrate_vertical,
    jump_apex_ticks,
    jump_speed,
    powered_run_ticks_to_vmax,
    step,
    to_pixels,
)


def test_step_matches_textbook_kinematics() -> None:
    v, t, a = 3.0, 0.01, -2.0
    disp, new_v = step(v, t, a)
    assert math.isclose(disp, v * t + 0.5 * a * t * t)
    assert math.isclose(new_v, v + a * t)


def test_jump_launch_is_sqrt_two_g_h() -> None:
    expected = -math.sqrt(2.0 * G * REAL_HEIGHT)
    assert math.isclose(jump_speed(), expected)
    assert math.isclose(expected, -math.sqrt(210.0))


def test_pixel_scale() -> None:
    assert UNREAL_HEIGHT == 101
    assert math.isclose(PIXELS_PER_METER, 101 / 3.5)
    assert math.isclose(to_pixels(REAL_HEIGHT), 101.0)


def test_open_air_jump_rises_about_101_pixels() -> None:
    ticks, peak, v_apex = jump_apex_ticks()
    # Energy identity says the peak is REAL_HEIGHT meters = 101 px. Discrete
    # Euler steps overshoot the apex by less than one gravity tick.
    assert ticks == 49  # first tick with vY >= 0 after 48 rising ticks
    assert 100.0 < peak < 101.6
    assert v_apex >= 0
    # Hand-integrated check: after 48 ticks v = -sqrt(210) + 30*0.48
    v = jump_speed()
    y = 0.0
    for _ in range(48):
        y, v = integrate_vertical(y, v)
    assert v < 0
    y, v = integrate_vertical(y, v)
    assert v >= 0


def test_friction_k_is_plus_or_minus_two() -> None:
    assert friction_k(4.0) == -2.0
    assert friction_k(-4.0) == 2.0


def test_friction_u_matches_createMap_switch() -> None:
    assert math.isclose(friction_u(6, 1), (V_MAX / 0.5) / G)
    assert math.isclose(friction_u(1, 1), (V_MAX / 1.2) / G)
    assert math.isclose(friction_u(2, 1), (V_MAX / 1.2) / G)
    assert math.isclose(friction_u(11, 1), (V_MAX / 1.5) / G)
    assert math.isclose(friction_u(1, 3), (V_MAX / 0.5) / G)
    assert math.isclose(friction_u(2, 3), (V_MAX / 1.2) / G)


def test_powered_run_hits_vmax_in_40_ticks() -> None:
    # v += 20 * 0.01 each tick → 0.2 per tick → 8.0 at tick 40.
    assert powered_run_ticks_to_vmax() == 40
    v = 0.0
    for _ in range(40):
        _, v = step(v, TIME, A_ROLE)
    assert math.isclose(v, V_MAX)


def main() -> int:
    testers = [
        test_step_matches_textbook_kinematics,
        test_jump_launch_is_sqrt_two_g_h,
        test_pixel_scale,
        test_open_air_jump_rises_about_101_pixels,
        test_friction_k_is_plus_or_minus_two,
        test_friction_u_matches_createMap_switch,
        test_powered_run_hits_vmax_in_40_ticks,
    ]
    for fn in testers:
        fn()
        print(f"ok  {fn.__name__}")
    print(f"{len(testers)} physics tests passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
