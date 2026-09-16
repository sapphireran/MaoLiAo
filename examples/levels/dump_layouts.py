#!/usr/bin/env python3
"""Print ASCII maps for worlds 1–3 and report MAP_NUMBER truncation."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao_lib.constants import MAP_NUMBER  # noqa: E402
from maoliao_lib.worlds import (  # noqa: E402
    authored_map,
    dropped_map,
    format_ascii,
    loaded_map,
    render_ascii,
)


FIXTURE_DIR = Path(__file__).resolve().parent / "fixtures"


def report(world: int, seed: int) -> str:
    authored = authored_map(world, seed)
    loaded = loaded_map(world, seed)
    dropped = dropped_map(world, seed)
    lines = [
        f"== world {world} ==",
        f"authored tiles {len(authored)}  loaded {len(loaded)}  "
        f"MAP_NUMBER={MAP_NUMBER}",
    ]
    if dropped:
        lines.append("dropped:")
        for tile in dropped:
            lines.append(
                f"  x={tile.x} y={tile.y} id={tile.id} "
                f"{tile.x_amount}x{tile.y_amount}"
            )
    else:
        lines.append("dropped: none")
    lines.append("")
    lines.append(format_ascii(render_ascii(world, seed=seed, use_loaded=True)))
    lines.append("")
    return "\n".join(lines) + "\n"


def summarize(world: int, seed: int) -> None:
    sys.stdout.write(report(world, seed))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--world", type=int, choices=(1, 2, 3), default=0)
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument(
        "--check-fixtures",
        action="store_true",
        help="compare the dump to examples/levels/fixtures/worldN.txt",
    )
    args = parser.parse_args()
    worlds = (args.world,) if args.world else (1, 2, 3)
    if args.check_fixtures:
        failed = 0
        for world in worlds:
            got = report(world, args.seed)
            path = FIXTURE_DIR / f"world{world}.txt"
            expected = path.read_text(encoding="utf-8")
            if got != expected:
                print(f"fixture mismatch {path}", file=sys.stderr)
                failed += 1
            else:
                print(f"ok  fixture world {world}")
        return 1 if failed else 0
    for world in worlds:
        summarize(world, args.seed)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
