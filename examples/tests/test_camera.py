from __future__ import annotations

import unittest

from examples.maoliao_lab.camera import Camera, is_ending, is_passed, world_x
from examples.maoliao_lab.constants import ENDING_DISTANCE, K_MAP_BG, PIXELS_PER_REAL, TIME, XLEFT, XRIGHT, XSIZE, V_MAX


class CameraPin(unittest.TestCase):
    def test_left_clamp(self) -> None:
        cam = Camera()
        self.assertEqual(cam.pin_hero_screen_x(-10, -1, ending=False), XLEFT)
        self.assertEqual(cam.x0, 0)

    def test_right_overflow_feeds_x0(self) -> None:
        cam = Camera()
        x = cam.pin_hero_screen_x(XRIGHT + 40, 8, ending=False)
        self.assertEqual(x, XRIGHT)
        self.assertEqual(cam.x0, -40)
        self.assertAlmostEqual(world_x(x, cam.x0), XRIGHT + 40)

    def test_ending_does_not_pin(self) -> None:
        cam = Camera()
        x = cam.pin_hero_screen_x(XRIGHT + 40, 8, ending=True)
        self.assertEqual(x, XRIGHT + 40)
        self.assertEqual(cam.x0, 0)


class Parallax(unittest.TestCase):
    def test_sky_moves_only_on_right_edge(self) -> None:
        cam = Camera()
        cam.step_parallax(8, screen_x=100, previous_x0=0)
        self.assertEqual(cam.x_bg, 0.0)
        cam.x0 = -10
        cam.step_parallax(8, screen_x=XRIGHT, previous_x0=0)
        expected = abs(8) * TIME * PIXELS_PER_REAL / K_MAP_BG
        self.assertAlmostEqual(cam.x_bg, -expected)

    def test_wrap(self) -> None:
        cam = Camera(x_bg=-512.5)
        cam.wrap_sky(512)
        self.assertEqual(cam.x_bg, 0.0)


class Ending(unittest.TestCase):
    def test_world_thresholds(self) -> None:
        self.assertFalse(is_ending(ENDING_DISTANCE[1], 1))
        self.assertTrue(is_ending(ENDING_DISTANCE[1] + 1, 1))
        self.assertTrue(is_ending(ENDING_DISTANCE[2] + 1, 2))
        self.assertGreater(ENDING_DISTANCE[2], ENDING_DISTANCE[1])

    def test_passed_is_off_the_window(self) -> None:
        self.assertFalse(is_passed(XSIZE))
        self.assertTrue(is_passed(XSIZE + 1))


if __name__ == "__main__":
    unittest.main()
