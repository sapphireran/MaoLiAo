import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao.collision import Rect, collides_with_map, hero_corners, is_hit, tile_rect
from maoliao.constants import HEIGHT, WIDTH
from maoliao.maps import world_payload


class HitTests(unittest.TestCase):
    def test_corner_inside(self):
        box = Rect(0, 0, 32, 32)
        self.assertTrue(is_hit([(1, 1)], box))
        self.assertFalse(is_hit([(33, 1)], box))

    def test_inclusive_edge(self):
        box = Rect(0, 0, 32, 32)
        self.assertTrue(is_hit([(32, 16)], box))

    def test_hero_inset(self):
        corners = hero_corners(0, 0, x0=0)
        self.assertEqual(corners[0], (1, 1))
        self.assertEqual(corners[3], (WIDTH - 1, HEIGHT - 1))


class TileRectTests(unittest.TestCase):
    def test_grass_strip(self):
        box = tile_rect(0, 9, 15, 1, 1)
        self.assertEqual(box.left, 0)
        self.assertEqual(box.top, 9 * 32)
        self.assertEqual(box.right, 15 * 32)
        self.assertEqual(box.bottom, 10 * 32)

    def test_pipe_mouth_double(self):
        box = tile_rect(36, 7, 1, 1, 10)
        self.assertEqual(box.right - box.left, 64)
        self.assertEqual(box.bottom - box.top, 64)


class World1SpawnFallTests(unittest.TestCase):
    def test_spawn_is_above_floor(self):
        maps = world_payload(1)["maps_loaded"]
        self.assertIsNone(collides_with_map(64, 64, maps, world=1))
        hit = collides_with_map(64, 9 * 32 - 8, maps, world=1)
        self.assertIsNotNone(hit)
        self.assertEqual(hit["id"], 1)

    def test_decor_does_not_collide(self):
        maps = world_payload(1)["maps_loaded"]
        hit = collides_with_map(101 * 32, 7 * 32, maps, world=1)
        self.assertTrue(hit is None or hit["id"] != 12)


class World3LethalTests(unittest.TestCase):
    def test_cloud_is_safe(self):
        maps = world_payload(3, seed=1)["maps_loaded"]
        rec = next(m for m in maps if m["id"] == 2)
        hit = collides_with_map(
            rec["x"] * WIDTH + 4,
            rec["y"] * HEIGHT,
            maps,
            world=3,
            is_shoot=False,
        )
        self.assertIsNotNone(hit)
        self.assertEqual(hit["id"], 2)
        self.assertFalse(hit["lethal"])

    def test_pipe_mouth_is_lethal_until_mushroom(self):
        maps = world_payload(3, seed=1)["maps_loaded"]
        rec = next(m for m in maps if m["id"] == 10)
        x = rec["x"] * WIDTH + 8
        y = rec["y"] * HEIGHT + 8
        hit = collides_with_map(x, y, maps, world=3, is_shoot=False)
        self.assertIsNotNone(hit)
        self.assertTrue(hit["lethal"])
        armed = collides_with_map(x, y, maps, world=3, is_shoot=True)
        self.assertIsNotNone(armed)
        self.assertFalse(armed["lethal"])
