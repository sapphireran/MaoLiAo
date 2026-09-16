from __future__ import annotations

import unittest

from examples.maoliao_lab.constants import A_ROLE, G, U_HIGH, U_NORMAL, V_MAX
from examples.maoliao_lab.friction import cap_vx, friction_accel, k_from_vx, skim_to_stop, walk_accel
from examples.maoliao_lab.tiles import friction_for


class FrictionFudge(unittest.TestCase):
    def test_k_is_plus_or_minus_two(self) -> None:
        self.assertEqual(k_from_vx(8), -2)
        self.assertEqual(k_from_vx(-8), 2)
        self.assertEqual(k_from_vx(0), 0)

    def test_a1_opposes_velocity_when_coasting(self) -> None:
        a1 = friction_accel(8.0, walk_accel=0.0, u=U_NORMAL, airborne=False)
        self.assertLess(a1, 0)
        a1_left = friction_accel(-8.0, walk_accel=0.0, u=U_NORMAL, airborne=False)
        self.assertGreater(a1_left, 0)

    def test_no_friction_in_air_or_when_accelerating_with_velocity(self) -> None:
        self.assertEqual(friction_accel(8, A_ROLE, U_NORMAL, airborne=True), 0)
        self.assertEqual(friction_accel(8, A_ROLE, U_NORMAL, airborne=False), 0)

    def test_magnitude_is_two_g_u(self) -> None:
        a1 = friction_accel(8.0, 0.0, U_HIGH, airborne=False)
        self.assertAlmostEqual(abs(a1), 2 * G * U_HIGH)

    def test_high_friction_stops_sooner(self) -> None:
        self.assertLess(skim_to_stop(V_MAX, U_HIGH), skim_to_stop(V_MAX, U_NORMAL))

    def test_cap(self) -> None:
        self.assertEqual(cap_vx(8), 8)
        self.assertAlmostEqual(cap_vx(12), V_MAX)
        self.assertAlmostEqual(cap_vx(-12), -V_MAX)

    def test_ending_forces_right(self) -> None:
        # ending ignores left (auto-run) and always adds A_ROLE
        self.assertEqual(walk_accel(left=True, right=False, ending=True), A_ROLE)
        self.assertEqual(walk_accel(left=False, right=False, ending=True), A_ROLE)
        self.assertEqual(walk_accel(left=True, right=False, ending=False), -A_ROLE)

    def test_tile_friction_tables(self) -> None:
        self.assertAlmostEqual(friction_for(6, 1), U_HIGH)
        self.assertAlmostEqual(friction_for(1, 1), U_NORMAL)
        self.assertAlmostEqual(friction_for(6, 3), U_HIGH)
        self.assertAlmostEqual(friction_for(8, 3), friction_for(11, 1))  # both default T3


if __name__ == "__main__":
    unittest.main()
