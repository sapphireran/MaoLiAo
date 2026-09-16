from __future__ import annotations

import unittest

from examples.maoliao_lab.aabb import Rect, food_rect, hit_coin, hit_map, is_hit, sprite_corners, tile_rect
from examples.maoliao_lab.constants import HEIGHT, WIDTH


class CornersAndHit(unittest.TestCase):
    def test_corners_are_inset_by_one_pixel(self) -> None:
        c = sprite_corners(64, 64, x0=0)
        self.assertEqual(c[0].x, 65)
        self.assertEqual(c[0].y, 65)
        self.assertEqual(c[3].x, 64 + WIDTH - 1)
        self.assertEqual(c[3].y, 64 + HEIGHT - 1)

    def test_world_space_subtracts_x0(self) -> None:
        c = sprite_corners(192, 64, x0=-100)
        self.assertEqual(c[0].x, 100 + 192 + 1)

    def test_is_hit_needs_only_one_corner(self) -> None:
        corners = sprite_corners(0, 0)
        box = Rect(0, 0, 2, 2)
        self.assertTrue(is_hit(corners, box))
        miss = Rect(100, 100, 110, 110)
        self.assertFalse(is_hit(corners, miss))


class TileRect(unittest.TestCase):
    def test_ordinary_tile(self) -> None:
        r = tile_rect(3, 8, 2, 1, tile_id=1)
        self.assertEqual((r.left, r.top, r.right, r.bottom), (96, 256, 160, 288))

    def test_pipe_mouth_is_double_size(self) -> None:
        r = tile_rect(36, 7, 1, 1, tile_id=10)
        self.assertEqual(r.right - r.left, 64)
        self.assertEqual(r.bottom - r.top, 64)


class HitMapWorld3(unittest.TestCase):
    def setUp(self) -> None:
        self.pipe = [{"x": 10, "y": 4, "id": 10, "xAmount": 1, "yAmount": 1}]
        self.cloud = [{"x": 0, "y": 9, "id": 2, "xAmount": 4, "yAmount": 1}]
        self.grass = [{"x": 0, "y": 9, "id": 11, "xAmount": 1, "yAmount": 1}]

    def test_scenery_is_not_solid(self) -> None:
        self.assertIsNone(hit_map(0, 9 * 32, self.grass, world=1))

    def test_world3_pipe_kills_without_weapon(self) -> None:
        # Stand overlapping the mouth at (10,4) cells = (320, 128) px.
        hit = hit_map(320, 128, self.pipe, world=3, is_shoot=False)
        self.assertIsNotNone(hit)
        self.assertTrue(hit["died"])

    def test_world3_pipe_safe_with_weapon(self) -> None:
        hit = hit_map(320, 128, self.pipe, world=3, is_shoot=True)
        self.assertIsNotNone(hit)
        self.assertFalse(hit["died"])

    def test_world3_cloud_does_not_kill(self) -> None:
        hit = hit_map(0, 9 * 32, self.cloud, world=3, is_shoot=False)
        self.assertIsNotNone(hit)
        self.assertFalse(hit["died"])

    def test_world1_pipe_does_not_set_died(self) -> None:
        hit = hit_map(320, 128, self.pipe, world=1, is_shoot=False)
        self.assertIsNotNone(hit)
        self.assertFalse(hit["died"])


class Pickups(unittest.TestCase):
    def test_coin_cell(self) -> None:
        self.assertEqual(hit_coin(10 * 32, 5 * 32, [(10, 5)]), (10, 5))
        self.assertIsNone(hit_coin(0, 0, [(10, 5)]))

    def test_origin_coin_skipped(self) -> None:
        self.assertIsNone(hit_coin(0, 0, [(0, 0)]))

    def test_food_size(self) -> None:
        box = food_rect(448, 160)
        self.assertAlmostEqual(box.right - box.left, 3 * 32 / 2 + 4)
        self.assertAlmostEqual(box.bottom - box.top, 4 * 32 / 5)


if __name__ == "__main__":
    unittest.main()
