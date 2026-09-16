#!/usr/bin/env python3
"""Render a fixture world as ASCII. Optionally refresh the golden file."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from maoliao_fixtures import FIXTURES, level_payload, render_ascii  # noqa: E402


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--world", type=int, choices=(1, 2, 3), required=True)
    parser.add_argument(
        "--write-expected",
        action="store_true",
        help="overwrite fixtures/expected/world-N.ascii.txt",
    )
    args = parser.parse_args()
    data = level_payload(args.world)
    text = render_ascii(data["tiles"], data["coins"], data["enemies"], data["food"])
    if args.write_expected:
        dest = FIXTURES / "expected" / f"world-{args.world}.ascii.txt"
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_text(text, encoding="utf-8")
        print(f"wrote {dest}", file=sys.stderr)
    sys.stdout.write(text)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
