import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao.lint import lint_all, lint_world


class LintTests(unittest.TestCase):
    def test_world1_truncation_warning(self):
        codes = [f.code for f in lint_world(1)]
        self.assertIn("truncated_records", codes)
        self.assertNotIn("short_runway", codes)

    def test_world2_truncation_includes_goal(self):
        findings = lint_world(2)
        truncated = next(f for f in findings if f.code == "truncated_records")
        self.assertIn("id=12", truncated.message)

    def test_world3_food_warning(self):
        codes = [f.code for f in lint_world(3, seed=1)]
        self.assertIn("food_near_origin", codes)

    def test_no_errors_on_shipped_tables(self):
        errors = [f for f in lint_all(seed=1) if f.severity == "error"]
        self.assertEqual(errors, [])


if __name__ == "__main__":
    unittest.main()
