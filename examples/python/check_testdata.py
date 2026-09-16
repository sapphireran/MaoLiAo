#!/usr/bin/env python3
"""Confirm the checked-in CSVs match the tables copied from scene.cpp."""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from dump_world_maps import WORLD1

ROOT = Path(__file__).resolve().parent.parent
MAP_CSV = ROOT / "testdata" / "world1_map.csv"
COIN_CSV = ROOT / "testdata" / "world1_coins.csv"

WORLD1_COINS = [
    (10, 5), (11, 5), (12, 5), (13, 5), (66, 3), (67, 3), (68, 3), (69, 3),
    (74, 5), (74, 6), (74, 7), (74, 8), (82, 7), (83, 8), (84, 7), (85, 8),
    (86, 7), (87, 8), (88, 7), (89, 8),
]


def load_rows(path: Path, cols: int) -> list[tuple]:
    rows: list[tuple] = []
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        parts = line.split(",")
        nums = tuple(int(p) for p in parts[:cols])
        extra = tuple(parts[cols:])
        rows.append(nums + extra)
    return rows


def main() -> int:
    maps = load_rows(MAP_CSV, 5)
    if len(maps) != len(WORLD1):
        raise SystemExit(f"map csv has {len(maps)} rows, expected {len(WORLD1)}")
    for i, rec in enumerate(WORLD1):
        got = maps[i][:5]
        if got != rec:
            raise SystemExit(f"map row {i}: {got} != {rec}")
        kept = maps[i][6]
        expect = "yes" if i < 30 else "no"
        if kept != expect:
            raise SystemExit(f"map row {i} kept flag {kept} != {expect}")

    coins = load_rows(COIN_CSV, 2)
    got_coins = [c[:2] for c in coins]
    if got_coins != WORLD1_COINS:
        raise SystemExit(f"coin csv mismatch: {got_coins}")

    print(f"checked {MAP_CSV.name} ({len(maps)} tiles) and {COIN_CSV.name} ({len(coins)} coins)")
    print("check_testdata.py: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
