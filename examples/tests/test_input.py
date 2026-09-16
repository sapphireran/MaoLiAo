from __future__ import annotations

import unittest

from examples.maoliao_lab.commands import pack, sticky_sample, unpack
from examples.maoliao_lab.constants import CMD_ESC, CMD_LEFT, CMD_RIGHT, CMD_SHOOT, CMD_UP, VIR_HOME


class Bitmask(unittest.TestCase):
    def test_pack_wasd_style(self) -> None:
        self.assertEqual(pack("A"), CMD_LEFT)
        self.assertEqual(pack("D"), CMD_RIGHT)
        self.assertEqual(pack("W"), CMD_UP)
        self.assertEqual(pack("K"), CMD_UP)
        self.assertEqual(pack("J"), CMD_SHOOT)
        self.assertEqual(pack("ESC"), CMD_ESC)
        self.assertEqual(pack("AWJ"), CMD_LEFT | CMD_UP | CMD_SHOOT)

    def test_unpack_roundtrip(self) -> None:
        bits = pack(["A", "D"])
        names = unpack(bits)
        self.assertEqual(names, ["left", "right"])

    def test_virtual_bits_survive_or(self) -> None:
        self.assertIn("vir_home", unpack(VIR_HOME | CMD_ESC))

    def test_sticky_cache_without_kbhit(self) -> None:
        self.assertEqual(sticky_sample(CMD_RIGHT, False, 0), CMD_RIGHT)

    def test_sticky_cache_replaced_on_hit(self) -> None:
        self.assertEqual(sticky_sample(CMD_RIGHT, True, CMD_LEFT), CMD_LEFT)


if __name__ == "__main__":
    unittest.main()
