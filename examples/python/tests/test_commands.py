import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao.commands import decode, encode, from_keys
from maoliao.constants import CMD_LEFT, CMD_SHOOT, CMD_UP, VIR_HOME, VIR_RESTART


class CommandTests(unittest.TestCase):
    def test_chord_awj(self):
        mask = from_keys("A+W+J")
        self.assertEqual(mask, CMD_LEFT | CMD_UP | CMD_SHOOT)
        self.assertEqual(mask, 21)
        self.assertEqual(decode(21), ["CMD_LEFT", "CMD_UP", "CMD_SHOOT"])

    def test_k_is_jump(self):
        self.assertEqual(from_keys("K"), CMD_UP)
        self.assertEqual(from_keys("W"), from_keys("K"))

    def test_esc(self):
        self.assertIn("CMD_ESC", decode(from_keys("Esc")))

    def test_virtual_keys(self):
        self.assertEqual(encode("VIR_RESTART"), VIR_RESTART)
        self.assertEqual(encode("VIR_HOME"), VIR_HOME)
        self.assertEqual(decode(VIR_RESTART | VIR_HOME), ["VIR_RESTART", "VIR_HOME"])

    def test_unknown_key(self):
        with self.assertRaises(KeyError):
            from_keys("Q")

    def test_empty(self):
        self.assertEqual(from_keys(""), 0)
        self.assertEqual(decode(0), [])


if __name__ == "__main__":
    unittest.main()
