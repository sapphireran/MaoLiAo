from __future__ import annotations

import unittest

from examples.maoliao_lab.constants import G, PIXELS_PER_REAL, TIME, UNREAL_HEIGHT, jump_launch_vy
from examples.maoliao_lab.inertia import Body, step
from examples.maoliao_lab.jump import apex, arc, expected_peak_px


class InertiaStep(unittest.TestCase):
    def test_zero_accel_is_vt(self) -> None:
        dx, v = step(8.0, 0.01, 0.0)
        self.assertAlmostEqual(dx, 0.08)
        self.assertAlmostEqual(v, 8.0)

    def test_matches_hand_computed_first_jump_tick(self) -> None:
        v0 = jump_launch_vy()
        dx, v = step(v0, TIME, G)
        self.assertAlmostEqual(dx, v0 * TIME + 0.5 * G * TIME * TIME)
        self.assertAlmostEqual(v, v0 + G * TIME)

    def test_body_integrates_both_axes(self) -> None:
        body = Body(vx=8.0, vy=0.0)
        body.integrate(0.01, ax=0.0, ay=30.0)
        self.assertAlmostEqual(body.x, 0.08)
        self.assertAlmostEqual(body.vy, 0.3)
        self.assertAlmostEqual(body.y, 0.0015)


class JumpArc(unittest.TestCase):
    def test_peak_is_about_three_tiles(self) -> None:
        peak = apex()
        self.assertLess(peak.y_px, -95)
        self.assertGreater(peak.y_px, -105)
        self.assertAlmostEqual(expected_peak_px(), -UNREAL_HEIGHT)
        # Discrete Euler is a hair short of the closed-form peak.
        self.assertLess(abs(peak.y_px - expected_peak_px()), 2.0)

    def test_returns_to_ground(self) -> None:
        samples = arc()
        self.assertGreater(samples[-1].y_px, -1.0)
        self.assertGreater(samples[-1].vy, 0)
        self.assertTrue(samples[0].rising)
        self.assertFalse(samples[-1].rising)

    def test_apex_time_near_v_over_g(self) -> None:
        peak = apex()
        closed_form = abs(jump_launch_vy()) / G
        self.assertAlmostEqual(peak.t, closed_form, delta=TIME)

    def test_pixel_scale_on_first_tick(self) -> None:
        v0 = jump_launch_vy()
        dy, _ = step(v0, TIME, G)
        self.assertAlmostEqual(dy * PIXELS_PER_REAL, dy * 101 / 3.5)


if __name__ == "__main__":
    unittest.main()
