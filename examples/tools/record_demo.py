#!/usr/bin/env python3
"""Read / write gameRecord.dat the way Control::pauseClick does."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from kit.savefile import read_record, write_record  # noqa: E402


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    sub = p.add_subparsers(dest="cmd", required=True)
    r = sub.add_parser("read")
    r.add_argument("path")
    w = sub.add_parser("write")
    w.add_argument("path")
    w.add_argument("world", type=int)
    args = p.parse_args()
    if args.cmd == "read":
        value, status = read_record(args.path)
        print(f"{args.path}: {status}" + (f" world={value}" if value is not None else ""))
        return 0 if status == "ok" else 1
    write_record(args.path, args.world)
    print(f"wrote {args.world} -> {args.path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
