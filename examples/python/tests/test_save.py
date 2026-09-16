import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao.savefile import SaveError, read_world, write_world


class SaveTests(unittest.TestCase):
    def test_roundtrip(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "gameRecord.dat"
            write_world(path, 2)
            self.assertEqual(path.read_text(encoding="ascii"), "2")
            self.assertEqual(read_world(path), 2)

    def test_repo_save_is_world_3(self):
        repo = Path(__file__).resolve().parents[3]
        dat = repo / "MaoLiAo" / "gameRecord.dat"
        self.assertTrue(dat.is_file())
        self.assertEqual(read_world(dat), 3)

    def test_rejects_out_of_range(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "gameRecord.dat"
            path.write_text("4", encoding="ascii")
            with self.assertRaises(SaveError):
                read_world(path)
            path.write_text("0", encoding="ascii")
            with self.assertRaises(SaveError):
                read_world(path)

    def test_missing_file(self):
        with self.assertRaises(SaveError):
            read_world("/tmp/maoliao-does-not-exist-save.dat")

    def test_write_rejects_bad_world(self):
        with self.assertRaises(SaveError):
            write_world("/tmp/x.dat", 9)


if __name__ == "__main__":
    unittest.main()
