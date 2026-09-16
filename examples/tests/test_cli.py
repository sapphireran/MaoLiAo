from __future__ import annotations

import io
import json
import unittest
from contextlib import redirect_stdout

from examples.run_lab import main


class CliSmoke(unittest.TestCase):
    def _run(self, *args: str) -> str:
        buf = io.StringIO()
        with redirect_stdout(buf):
            code = main(list(args))
        self.assertEqual(code, 0)
        return buf.getvalue()

    def test_constants(self) -> None:
        out = self._run("constants")
        self.assertIn("512x384", out)
        self.assertIn("MAP_NUMBER cap", out)

    def test_summary_world2_json(self) -> None:
        out = self._run("summary", "--world", "2")
        payload = json.loads(out)
        self.assertFalse(payload["goal_present"])
        self.assertEqual(payload["stored_tiles"], 30)
        self.assertEqual(len(payload["dropped"]), 6)

    def test_save_decode(self) -> None:
        out = self._run("save", "--decode", "2")
        self.assertEqual(out.strip(), "2")

    def test_jump_table_header(self) -> None:
        out = self._run("jump-table", "--every", "50")
        self.assertIn("peak_px", out)
        self.assertIn("tick", out)

    def test_ascii_map_world1(self) -> None:
        out = self._run("ascii-map", "--world", "1")
        self.assertIn("stored tiles=30", out)
        self.assertIn("G", out)

    def test_tick_runs(self) -> None:
        out = self._run("tick", "--keys", "D", "--frames", "5")
        self.assertIn("camera x0", out)
