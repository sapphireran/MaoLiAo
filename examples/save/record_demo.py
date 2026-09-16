#!/usr/bin/env python3
"""Read or write the one-integer gameRecord.dat format."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from maoliao_sim.save import read_record, valid_world, write_record


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--read", type=Path, metavar="PATH")
    group.add_argument("--write", type=Path, metavar="PATH")
    parser.add_argument("--world", type=int, help="required with --write")
    args = parser.parse_args()

    if args.read:
        world = read_record(args.read)
        print(f"{args.read}: world={world} valid={valid_world(world)}")
        return 0

    if args.world is None:
        parser.error("--write requires --world")
    write_record(args.write, args.world)
    print(f"wrote {args.write}: world={args.world}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
