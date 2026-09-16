from __future__ import annotations

import unittest

from examples.maoliao_lab.constants import (
    CMD_LEFT,
    CMD_RIGHT,
    CMD_SHOOT,
    CMD_UP,
    F,
    G,
    HEIGHT,
    LIFE,
    MAP_NUMBER,
    PIXELS_PER_REAL,
    REAL_HEIGHT,
    T1,
    T2,
    T3,
    TIME,
    U_DEFAULT,
    U_HIGH,
    U_NORMAL,
    UNREAL_HEIGHT,
    V_MAX,
    WIDTH,
    X0,
    XRIGHT,
    XSIZE,
    Y0,
    YSIZE,
    friction_u,
    jump_launch_vy,
    pixels_to_real,
    real_to_pixels,
)


class ConstantsMatchDefineH(unittest.TestCase):
    def test_window_is_16_by_12_tiles(self) -> None:
        self.assertEqual(XSIZE, 16 * WIDTH)
        self.assertEqual(YSIZE, 12 * HEIGHT)
        self.assertEqual(WIDTH, 32)
        self.assertEqual(HEIGHT, 32)

    def test_spawn_macros(self) -> None:
        self.assertEqual(X0, 64)
        self.assertEqual(Y0, 96)
        self.assertEqual(XRIGHT, 192)

    def test_time_and_gravity(self) -> None:
        self.assertEqual(TIME, 0.01)
        self.assertEqual(G, 30.0)
        self.assertEqual(V_MAX, 8.0)
        self.assertEqual(REAL_HEIGHT, 3.5)
        self.assertEqual(UNREAL_HEIGHT, 101)
        self.assertAlmostEqual(PIXELS_PER_REAL, 101 / 3.5)

    def test_friction_coefficients(self) -> None:
        self.assertAlmostEqual(U_HIGH, (8.0 / 0.5) / 30.0)
        self.assertAlmostEqual(U_NORMAL, (8.0 / 1.2) / 30.0)
        self.assertAlmostEqual(U_DEFAULT, (8.0 / 1.5) / 30.0)
        self.assertAlmostEqual(friction_u(T1), U_HIGH)
        self.assertAlmostEqual(friction_u(T2), U_NORMAL)
        self.assertAlmostEqual(friction_u(T3), U_DEFAULT)

    def test_jump_launch_is_sqrt_two_g_h(self) -> None:
        self.assertAlmostEqual(jump_launch_vy(), -(210.0 ** 0.5), places=9)

    def test_life_and_map_cap(self) -> None:
        self.assertEqual(LIFE, 5)
        self.assertEqual(MAP_NUMBER, 30)
        self.assertAlmostEqual(F, 0.003)

    def test_command_bits_are_powers_of_two(self) -> None:
        self.assertEqual(CMD_LEFT | CMD_RIGHT | CMD_UP | CMD_SHOOT, 1 + 2 + 4 + 16)

    def test_real_pixel_roundtrip(self) -> None:
        self.assertAlmostEqual(pixels_to_real(real_to_pixels(3.5)), 3.5)
        self.assertAlmostEqual(real_to_pixels(REAL_HEIGHT), UNREAL_HEIGHT)


if __name__ == "__main__":
    unittest.main()
