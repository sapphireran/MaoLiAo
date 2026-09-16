from __future__ import annotations

import unittest

from examples.maoliao_lab.commands import pack
from examples.maoliao_lab.constants import LIFE, SCORE_COIN, TIME, V_MAX, X0, XRIGHT, YSIZE
from examples.maoliao_lab.loop import TickState, collect_coin, tick


class ToyLoop(unittest.TestCase):
    def test_starts_with_five_lives_and_x0_spawn(self) -> None:
        s = TickState()
        self.assertEqual(s.life, LIFE)
        self.assertEqual(s.screen_x, X0)
        self.assertEqual(s.screen_y, X0)
        self.assertTrue(s.is_fly)

    def test_holding_d_reaches_right_pin(self) -> None:
        s = TickState()
        bits = pack("D")
        saw_pin = False
        for _ in range(400):
            log = tick(s, bits)
            if log.x == XRIGHT and s.camera.x0 < 0:
                saw_pin = True
                break
        self.assertTrue(saw_pin)
        self.assertLessEqual(abs(s.vx), V_MAX + 1e-9)

    def test_jump_leaves_ground(self) -> None:
        s = TickState()
        s.is_fly = False
        s.screen_y = s.ground_y - 32
        s.vy = 0
        log = tick(s, pack("W"))
        self.assertTrue(log.fly)
        self.assertLess(log.y, s.ground_y - 32)

    def test_world3_can_flap_while_airborne(self) -> None:
        s = TickState(world=3, is_fly=True, vy=5.0)
        tick(s, pack("W"))
        self.assertLess(s.vy, 0)

    def test_world1_cannot_double_jump(self) -> None:
        s = TickState(world=1, is_fly=True, vy=5.0)
        tick(s, pack("W"))
        self.assertGreater(s.vy, 0)

    def test_fall_off_bottom_sets_died(self) -> None:
        s = TickState()
        s.ground_y = 10_000
        s.is_fly = True
        s.screen_y = YSIZE - 1
        s.vy = 20
        for _ in range(50):
            tick(s, 0)
            if s.died:
                break
        self.assertTrue(s.died)

    def test_dt_accumulates(self) -> None:
        s = TickState()
        tick(s, 0)
        tick(s, 0)
        self.assertAlmostEqual(s.t, 2 * TIME)
        self.assertEqual(s.frames, 2)

    def test_coin_score(self) -> None:
        s = TickState()
        collect_coin(s)
        collect_coin(s)
        self.assertEqual(s.score, 2 * SCORE_COIN)


if __name__ == "__main__":
    unittest.main()
