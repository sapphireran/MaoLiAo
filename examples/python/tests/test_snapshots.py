import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao.preview import render
from maoliao.lint import lint_all

OUTPUT = Path(__file__).resolve().parents[2] / "output"


class SnapshotTests(unittest.TestCase):
    def test_world1_snapshot(self):
        self.assertEqual(render(1) + "\n", (OUTPUT / "world1.txt").read_text(encoding="utf-8"))

    def test_world2_snapshot(self):
        self.assertEqual(render(2) + "\n", (OUTPUT / "world2.txt").read_text(encoding="utf-8"))

    def test_world3_snapshot(self):
        self.assertEqual(
            render(3, seed=1) + "\n",
            (OUTPUT / "world3_seed1.txt").read_text(encoding="utf-8"),
        )

    def test_lint_snapshot(self):
        lines = [
            f"world {f.world:d}  {f.severity:7s}  {f.code}: {f.message}"
            for f in lint_all(seed=1)
        ]
        self.assertEqual("\n".join(lines) + "\n", (OUTPUT / "lint.txt").read_text(encoding="utf-8"))
