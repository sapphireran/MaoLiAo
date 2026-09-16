from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from examples.maoliao_lab.savefile import SaveError, decode, encode, read_file, write_file


class GameRecord(unittest.TestCase):
    def test_encode_ascii_digit(self) -> None:
        self.assertEqual(encode(1), b"1")
        self.assertEqual(encode(3), b"3")

    def test_rejects_out_of_range(self) -> None:
        with self.assertRaises(SaveError):
            encode(0)
        with self.assertRaises(SaveError):
            encode(4)
        with self.assertRaises(SaveError):
            decode(b"0")
        with self.assertRaises(SaveError):
            decode(b"99")

    def test_checked_in_save_is_world_three(self) -> None:
        path = Path("MaoLiAo/gameRecord.dat")
        self.assertTrue(path.exists())
        self.assertEqual(decode(path.read_bytes()), 3)

    def test_roundtrip_file(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "gameRecord.dat"
            write_file(path, 2)
            self.assertEqual(read_file(path), 2)
            self.assertEqual(path.read_bytes(), b"2")


if __name__ == "__main__":
    unittest.main()
