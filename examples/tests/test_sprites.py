from __future__ import annotations

import unittest

from examples.maoliao_lab.constants import HEIGHT, TIME, WIDTH
from examples.maoliao_lab.sprites import (
    HOME_STRIPS,
    MCI_ALIASES,
    bomb_src,
    coin_src,
    hero_blit,
    hero_walk_iframe,
    map_src,
    scenery_src,
    sky_y,
    wrap_iframe,
)


class WalkCycle(unittest.TestCase):
    def test_iframe_pattern(self) -> None:
        self.assertEqual(hero_walk_iframe(0), 1)  # 0/10 % 4 == 0
        self.assertEqual(hero_walk_iframe(20), 2)  # 2 % 4 == 2
        self.assertEqual(hero_walk_iframe(40), 1)

    def test_facing_offsets(self) -> None:
        right = hero_blit(1, 1)
        left = hero_blit(-1, 1)
        dead = hero_blit(1, 1, died=True)
        self.assertEqual(right.color_x, 0)
        self.assertEqual(left.color_x, 96)
        self.assertEqual(dead.color_x, 64)
        self.assertEqual(dead.mask_y, 32)


class Sheets(unittest.TestCase):
    def test_coin_rows(self) -> None:
        b = coin_src(1)
        self.assertEqual(b.color_y, 8 * HEIGHT)
        self.assertEqual(b.mask_y, 9 * HEIGHT)

    def test_pipe_blit_sizes(self) -> None:
        self.assertEqual(map_src(1)[0:2], (32, 32))
        self.assertEqual(map_src(7)[0:2], (64, 32))
        self.assertEqual(map_src(10)[0:2], (64, 64))

    def test_scenery_frames(self) -> None:
        w, h, x, color_y, mask_y = scenery_src(12, 1)
        self.assertEqual((w, h), (96, 64))
        self.assertEqual(color_y, 1 * 4 * HEIGHT)
        self.assertEqual(mask_y, color_y + 2 * HEIGHT)
        self.assertEqual(x, 0)

    def test_bomb_is_centered(self) -> None:
        blit, offset = bomb_src(1)
        self.assertEqual((blit.w, blit.h), (64, 64))
        self.assertEqual(offset, (-16, -16))

    def test_iframe_wrap(self) -> None:
        value = 4.9
        value = wrap_iframe(value, period=5, speed=TIME * 7)
        # 4.9 + 0.07 = 4.97, int 4, no wrap yet
        self.assertGreater(value, 4)
        value = wrap_iframe(4.99, period=5, speed=0.02)
        self.assertEqual(value, 1.0)

    def test_sky_strip_per_world(self) -> None:
        self.assertEqual(sky_y(1), 0)
        self.assertEqual(sky_y(2), -384)
        self.assertEqual(sky_y(3), -768)

    def test_catalogs_are_complete(self) -> None:
        self.assertEqual(len(MCI_ALIASES), 12)
        self.assertIn("music_jump", MCI_ALIASES)
        self.assertEqual(HOME_STRIPS["all_clear"], -3 * 384)
        self.assertIn("通关.mp3", MCI_ALIASES.values())


if __name__ == "__main__":
    unittest.main()
