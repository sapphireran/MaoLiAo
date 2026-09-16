import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao.constants import MAP_NUMBER
from maoliao.maps import (
    apply_map_cap,
    colliding_extent_tiles,
    generate_world3_maps,
    load_worlds,
    world_payload,
)
from maoliao.preview import render


class World1Tests(unittest.TestCase):
    def test_authored_count(self):
        authored = load_worlds()["worlds"]["1"]["maps"]
        self.assertEqual(len(authored), 32)
        loaded = apply_map_cap(authored)
        self.assertEqual(len(loaded), MAP_NUMBER)
        dropped_ids = [r["id"] for r in authored[30:]]
        self.assertEqual(dropped_ids, [13, 13])

    def test_ending_covered(self):
        payload = world_payload(1)
        self.assertEqual(payload["ending_tiles"], 94)
        self.assertGreaterEqual(colliding_extent_tiles(payload["maps_loaded"]), 94)

    def test_coin_and_enemy_counts(self):
        payload = world_payload(1)
        self.assertEqual(len(payload["coins"]), 20)
        self.assertEqual(len(payload["enemies"]), 10)
        self.assertEqual(payload["food_pixels"], [[448, 160]])


class World2Tests(unittest.TestCase):
    def test_truncates_goal_and_trees(self):
        payload = world_payload(2)
        self.assertEqual(len(payload["maps_authored"]), 36)
        self.assertEqual(len(payload["maps_loaded"]), 30)
        truncated_ids = [r["id"] for r in payload["maps_truncated"]]
        self.assertEqual(truncated_ids, [11, 12, 14, 14, 14, 14])

    def test_cloud_runway_present(self):
        loaded = world_payload(2)["maps_loaded"]
        self.assertTrue(any(r["x"] == 102 and r["id"] == 2 for r in loaded))


class World3Tests(unittest.TestCase):
    def test_generator_size(self):
        records = generate_world3_maps(seed=1)
        self.assertEqual(len(records), 7 * 4 + 2)
        self.assertEqual(records[-2]["id"], 2)
        self.assertEqual(records[-1]["id"], 12)

    def test_seed_is_deterministic(self):
        a = generate_world3_maps(seed=3)
        b = generate_world3_maps(seed=3)
        c = generate_world3_maps(seed=4)
        self.assertEqual(a, b)
        self.assertNotEqual(a, c)

    def test_payload_loads_all_thirty(self):
        payload = world_payload(3, seed=1)
        self.assertEqual(len(payload["maps_loaded"]), 30)
        self.assertEqual(payload["maps_truncated"], [])


class PreviewTests(unittest.TestCase):
    def test_world1_contains_spawn_and_legend(self):
        text = render(1)
        self.assertIn("world 1", text)
        self.assertIn("@", text)
        self.assertIn("legend:", text)
        self.assertGreater(len(text.splitlines()), 12)
