#!/usr/bin/env python3
"""Stdlib tests for maoliao_levels.py. Run from the repo root:

    python3 examples/toolkit/test_maoliao_levels.py
"""

from __future__ import annotations

import math
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))

import maoliao_levels as ml  # noqa: E402

LEVELS = ROOT / "examples" / "levels"


class ConstantsTests(unittest.TestCase):
    def test_macros_match_define_h(self):
        self.assertEqual(ml.XSIZE, 512)
        self.assertEqual(ml.YSIZE, 384)
        self.assertEqual(ml.WIDTH, 32)
        self.assertEqual(ml.HEIGHT, 32)
        self.assertEqual(ml.X0, 64)
        self.assertEqual(ml.Y0, 96)
        self.assertEqual(ml.UNREAL_HEIGHT, 101)
        self.assertEqual(ml.XRIGHT, 192)
        self.assertEqual(ml.MAP_NUMBER, 30)

    def test_jump_closed_form(self):
        v0 = ml.jump_takeoff_speed()
        self.assertAlmostEqual(v0, -math.sqrt(210.0), places=9)
        self.assertAlmostEqual(ml.jump_apex_pixels(), 101.0)
        self.assertAlmostEqual(ml.jump_apex_tiles(), 101.0 / 32.0, places=9)

    def test_friction_tables(self):
        self.assertAlmostEqual(ml.friction_u(1, "overworld"), (8.0 / 1.2) / 30.0)
        self.assertAlmostEqual(ml.friction_u(6, "overworld"), (8.0 / 0.5) / 30.0)
        self.assertAlmostEqual(ml.friction_u(10, "overworld"), (8.0 / 1.5) / 30.0)
        self.assertAlmostEqual(ml.friction_u(6, "pipes"), (8.0 / 0.5) / 30.0)
        self.assertAlmostEqual(ml.friction_u(2, "pipes"), (8.0 / 1.2) / 30.0)
        # left-facing stop is twice the right-facing magnitude (k = +4 vs -2)
        right = ml.friction_accel(1, "overworld", +1)
        left = ml.friction_accel(1, "overworld", -1)
        self.assertAlmostEqual(abs(left) / abs(right), 2.0)


class CollisionTests(unittest.TestCase):
    def test_pipe_mouth_is_2x2(self):
        self.assertEqual(ml.cell_size(10), (2, 2))
        self.assertEqual(ml.cell_size(8), (2, 2))
        self.assertEqual(ml.cell_size(1), (1, 1))
        self.assertEqual(ml.cell_size(7), (1, 1))

    def test_expand_pipe(self):
        cells = ml.expand_cells({"x": 36, "y": 7, "id": 10, "x_amount": 1, "y_amount": 1})
        self.assertEqual(set(cells), {(36, 7), (37, 7), (36, 8), (37, 8)})

    def test_hit_inset_quad_matches_role(self):
        quad = ml.inset_quad(x=64, y=64, x0=0)
        self.assertEqual(quad[0], (65, 65))
        self.assertEqual(quad[3], (95, 95))
        floor = (0, 96, 480, 128)
        self.assertFalse(ml.is_hit(quad, floor))
        overlapping = (60, 60, 70, 70)
        self.assertTrue(ml.is_hit(quad, overlapping))


class World3GeneratorTests(unittest.TestCase):
    def test_frozen_heights_match_json(self):
        path = LEVELS / "world-3-flappy.json"
        level = ml.load_level(path)
        heights = level["procedural"]["frozen_heights"]
        built = [
            (t["x"], t["y"], t["id"], t["x_amount"], t["y_amount"])
            for t in ml.generate_world3(heights)
        ]
        authored = [
            (t["x"], t["y"], t["id"], t["x_amount"], t["y_amount"]) for t in level["tiles"]
        ]
        self.assertEqual(authored, built)
        self.assertEqual(len(built), 30)

    def test_height_formula_sample(self):
        col = ml.build_pipe_column(10, 3)
        self.assertEqual(col[0]["y"], 1)
        self.assertEqual(col[1]["y_amount"], 1)
        self.assertEqual(col[2]["y"], 7)
        self.assertEqual(col[3]["y"], 9)
        self.assertEqual(col[3]["y_amount"], 3)


class FileTests(unittest.TestCase):
    def _all_levels(self):
        return [ml.load_level(p) for p in sorted(LEVELS.glob("*.json"))]

    def test_five_level_files(self):
        names = sorted(p.name for p in LEVELS.glob("*.json"))
        self.assertEqual(
            names,
            [
                "example-tutorial-parkour.json",
                "example-world-4-canyon.json",
                "world-1-grassland.json",
                "world-2-snow.json",
                "world-3-flappy.json",
            ],
        )

    def test_world1_census(self):
        level = ml.load_level(LEVELS / "world-1-grassland.json")
        s = ml.stats(level)
        self.assertEqual(s["tiles"], 32)
        self.assertEqual(s["tiles_dropped"], 2)
        self.assertEqual(s["coins"], 20)
        self.assertEqual(s["enemies"], 10)
        self.assertEqual(s["ending_pixels"], 94 * 32)
        self.assertAlmostEqual(level["food"][0]["x"], 14 * 32)
        self.assertAlmostEqual(level["food"][0]["y"], 5 * 32)

    def test_world2_census(self):
        level = ml.load_level(LEVELS / "world-2-snow.json")
        s = ml.stats(level)
        self.assertEqual(s["tiles"], 36)
        self.assertEqual(s["tiles_dropped"], 6)
        self.assertEqual(s["coins"], 11)
        self.assertEqual(s["ending_tiles"], 104)
        self.assertEqual(level["food"][0]["x"], 39 * 32 - 32 // 3)
        self.assertEqual(level["food"][0]["y"], 3 * 32 + 32 // 5)

    def test_examples_fit_caps(self):
        for name in ("example-world-4-canyon.json", "example-tutorial-parkour.json"):
            level = ml.load_level(LEVELS / name)
            self.assertLessEqual(len(level["tiles"]), ml.MAP_NUMBER)
            errors = [i for i in ml.validate(level) if i["severity"] == "error"]
            self.assertEqual(errors, [], errors)

    def test_shipped_worlds_only_warn_on_cap(self):
        for name in ("world-1-grassland.json", "world-2-snow.json"):
            level = ml.load_level(LEVELS / name)
            issues = ml.validate(level)
            caps = [i for i in issues if i["code"] == "map-cap"]
            self.assertEqual(len(caps), 1, caps)
            self.assertEqual(caps[0]["severity"], "warning")
            errors = [i for i in issues if i["severity"] == "error"]
            self.assertEqual(errors, [], errors)

    def test_validate_cli_ok_on_examples(self):
        rc = ml.main(
            [
                "validate",
                str(LEVELS / "example-world-4-canyon.json"),
                str(LEVELS / "example-tutorial-parkour.json"),
                str(LEVELS / "world-3-flappy.json"),
            ]
        )
        self.assertEqual(rc, 0)

    def test_export_contains_four_blocks(self):
        level = ml.load_level(LEVELS / "example-world-4-canyon.json")
        text = ml.export_cpp(level)
        self.assertIn("Map m[] = {", text)
        self.assertIn("Scene::createCoin", text)
        self.assertIn("Scene::createFood", text)
        self.assertIn("Role::createEnemy", text)
        self.assertIn("i < MAP_NUMBER", text)
        self.assertNotIn("i <= sizeof", text)

    def test_render_mentions_legend_and_ending(self):
        level = ml.load_level(LEVELS / "world-1-grassland.json")
        clipped = ml.render_ascii(level, max_width=40)
        self.assertIn("Grassland", clipped)
        self.assertIn("legend:", clipped)
        self.assertIn("ending@94", clipped)
        full = ml.render_ascii(level)
        self.assertIn(">", full)

    def test_food_pixels_tiles_and_pixels(self):
        self.assertEqual(ml.food_pixels({"x": 2, "y": 3, "unit": "tiles"}), (64.0, 96.0))
        self.assertEqual(ml.food_pixels({"x": 10, "y": 10, "unit": "pixels"}), (10.0, 10.0))
        with self.assertRaises(ml.LevelError):
            ml.food_pixels({"x": 0, "y": 0})


if __name__ == "__main__":
    unittest.main(verbosity=2)
