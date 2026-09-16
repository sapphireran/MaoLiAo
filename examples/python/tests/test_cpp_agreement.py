import math
import subprocess
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao.physics import jump_initial_vy, simulate_jump

CPP_DEMO = Path(__file__).resolve().parents[2] / "cpp" / "inertia_demo"


class CppAgreementTests(unittest.TestCase):
    def test_jump_table_matches_inertia_demo(self):
        if not CPP_DEMO.is_file():
            self.skipTest("examples/cpp/inertia_demo is not built")
        out = subprocess.check_output([str(CPP_DEMO)], text=True)
        table = []
        for ln in out.splitlines():
            parts = ln.split()
            if len(parts) == 5:
                try:
                    table.append(
                        (
                            int(parts[0]),
                            float(parts[1]),
                            float(parts[2]),
                            float(parts[3]),
                            float(parts[4]),
                        )
                    )
                except ValueError:
                    continue
        self.assertGreaterEqual(len(table), 9)
        samples = simulate_jump(ticks=8)
        for expected, row in zip(samples, table):
            self.assertEqual(expected.tick, row[0])
            self.assertAlmostEqual(expected.t, row[1], places=8)
            self.assertAlmostEqual(expected.vy, row[2], places=8)
            self.assertAlmostEqual(expected.y_metres, row[3], places=8)
            self.assertAlmostEqual(expected.y_pixels, row[4], places=8)
        self.assertAlmostEqual(jump_initial_vy(), -math.sqrt(210.0))
