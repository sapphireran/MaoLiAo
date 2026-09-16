from __future__ import annotations

import unittest

from examples.maoliao_lab.ascii_map import render
from examples.maoliao_lab.worlds import world1, world2, world3


class AsciiDump(unittest.TestCase):
    def test_world1_shows_goal_and_grass(self) -> None:
        w = world1()
        text = render(w["tiles"], w)
        self.assertIn("G", text)
        self.assertIn("=", text)
        self.assertTrue(text.splitlines()[0].startswith("   "))

    def test_world2_goal_missing_after_cap(self) -> None:
        w = world2()
        text = render(w["tiles"], w)
        self.assertNotIn("G", text)

    def test_world3_has_pipe_glyphs(self) -> None:
        w = world3()
        text = render(w["tiles"], w, scenery=False)
        self.assertIn("v", text)
        self.assertIn("^", text)
        self.assertIn("~", text)  # cloud runway

    def test_solids_only_hides_goal(self) -> None:
        w = world1()
        text = render(w["tiles"], w, scenery=False)
        self.assertNotIn("G", text)
