import math
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao.constants import A_ROLE, G, PIXELS_PER_METRE, REAL_HEIGHT, TIME, V_MAX, friction_u
from maoliao.physics import (
    apex_time_seconds,
    hold_right_until_vmax,
    jump_initial_vy,
    move,
    run_step,
    simulate_jump,
)


class MoveTests(unittest.TestCase):
    def test_zero_accel_is_vt(self):
        disp, v = move(10.0, 0.5, 0.0)
        self.assertEqual(v, 10.0)
        self.assertAlmostEqual(disp, 5.0)

    def test_from_rest(self):
        disp, v = move(0.0, 2.0, 4.0)
        self.assertAlmostEqual(disp, 8.0)
        self.assertAlmostEqual(v, 8.0)

    def test_matches_first_jump_tick(self):
        v0 = jump_initial_vy()
        disp, v1 = move(v0, TIME, G)
        self.assertAlmostEqual(disp, v0 * TIME + 0.5 * G * TIME * TIME)
        self.assertAlmostEqual(v1, v0 + G * TIME)


class JumpTests(unittest.TestCase):
    def test_initial_speed(self):
        self.assertAlmostEqual(jump_initial_vy(), -math.sqrt(210.0))

    def test_scale(self):
        self.assertAlmostEqual(PIXELS_PER_METRE, 101.0 / 3.5)

    def test_apex_time(self):
        self.assertAlmostEqual(apex_time_seconds(), abs(jump_initial_vy()) / G)

    def test_peak_is_about_unreal_height(self):
        samples = simulate_jump(ticks=80)
        min_px = min(s.y_pixels for s in samples)
        self.assertLess(min_px, -90.0)
        self.assertGreater(min_px, -110.0)

    def test_first_pixel_step_hand_calc(self):
        samples = simulate_jump(ticks=1)
        v0 = -math.sqrt(210.0)
        disp = v0 * 0.01 + 0.5 * 30.0 * 0.01 * 0.01
        self.assertAlmostEqual(samples[1].y_pixels, disp * (101.0 / 3.5), places=9)

    def test_real_height_constant(self):
        self.assertEqual(REAL_HEIGHT, 3.5)


class RunTests(unittest.TestCase):
    def test_friction_coefficients(self):
        self.assertAlmostEqual(friction_u("T1"), (8.0 / 0.5) / 30.0)
        self.assertAlmostEqual(friction_u("T2"), (8.0 / 1.2) / 30.0)
        self.assertAlmostEqual(friction_u("T3"), (8.0 / 1.5) / 30.0)

    def test_reaches_vmax(self):
        ticks = hold_right_until_vmax("T2")
        self.assertGreater(ticks, 1)
        self.assertLess(ticks, 200)

    def test_air_has_no_friction_when_coasting(self):
        dx, vx = run_step(4.0, 0.0, on_ground=False, friction_bucket="T2")
        self.assertAlmostEqual(vx, 4.0)
        self.assertAlmostEqual(dx, 4.0 * TIME * PIXELS_PER_METRE)

    def test_does_not_oscillate_through_zero(self):
        _, vx = run_step(0.05, -A_ROLE, True, "T1")
        self.assertGreaterEqual(vx, 0.0)


class ClampTests(unittest.TestCase):
    def test_vmax_clamp(self):
        _, vx = run_step(V_MAX, 20.0, True, "T3")
        self.assertAlmostEqual(vx, V_MAX)


if __name__ == "__main__":
    unittest.main()
