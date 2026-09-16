#!/usr/bin/env python3
"""Show command bitmasks the way Control::GetCommand builds them."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from maoliao_sim.commands import Command, describe, is_jump, parse_command_names


EXAMPLES = (
    ("A", ["a"]),
    ("D", ["d"]),
    ("W jump", ["w"]),
    ("K jump", ["k"]),
    ("J shoot", ["j"]),
    ("Esc pause", ["esc"]),
    ("A+W run-jump", ["a", "w"]),
    ("D+J run-shoot", ["d", "j"]),
    ("pause restart", ["restart"]),
    ("pause home", ["home"]),
)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("names", nargs="*", help="command names or keys (a, d, w, j, ...)")
    parser.add_argument("--world", type=int, default=1)
    parser.add_argument("--airborne", action="store_true")
    parser.add_argument("--ending", action="store_true")
    args = parser.parse_args()

    rows = [(label, names) for label, names in EXAMPLES]
    if args.names:
        rows = [("cli", args.names)]

    print(f"{'label':<16} {'bits':>6}  flags                         jump?")
    for label, names in rows:
        bits = parse_command_names(names)
        jump = is_jump(bits, args.airborne, args.ending, args.world)
        print(f"{label:<16} {bits:6d}  {','.join(describe(bits)) or '-':<28} {jump}")

    print()
    print("numeric values from define.h:")
    for command in Command:
        print(f"  {command.name:<8} {int(command)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
