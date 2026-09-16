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


def summarize(world: int, seed: int) -> None:
    authored = authored_map(world, seed)
    loaded = loaded_map(world, seed)
    dropped = dropped_map(world, seed)
    print(f"== world {world} ==")
    print(f"authored tiles {len(authored)}  loaded {len(loaded)}  "
          f"MAP_NUMBER={MAP_NUMBER}")
    if dropped:
        print("dropped:")
        for tile in dropped:
            print(f"  x={tile.x} y={tile.y} id={tile.id} "
                  f"{tile.x_amount}x{tile.y_amount}")
    else:
        print("dropped: none")
    print()
    print(format_ascii(render_ascii(world, seed=seed, use_loaded=True)))
    print()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--world", type=int, choices=(1, 2, 3), default=0)
    parser.add_argument("--seed", type=int, default=1)
    args = parser.parse_args()
    worlds = (args.world,) if args.world else (1, 2, 3)
    for world in worlds:
        summarize(world, args.seed)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
