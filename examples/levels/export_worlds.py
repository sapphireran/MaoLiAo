#!/usr/bin/env python3
"""Dump authored worlds as JSON for diffs and external tools."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from maoliao_sim.inertia import friction_u
from maoliao_sim.worlds import (
    WORLD_SPECS,
    coins_for,
    enemies_for,
    food_for,
    loaded_tile_count,
    tiles_for,
)


def world_payload(world: int, pipe_heights=None, coin_heights=None) -> dict:
    spec = WORLD_SPECS[world]
    tiles = [
        {
            "x": x,
            "y": y,
            "id": tid,
            "xAmount": xa,
            "yAmount": ya,
            "u": friction_u(tid, world),
        }
        for x, y, tid, xa, ya in tiles_for(world, pipe_heights)
    ]
    return {
        "world": world,
        "name": spec.name,
        "ending_tiles": spec.ending_tiles,
        "goal": {"x": spec.goal[0], "y": spec.goal[1]},
        "procedural": spec.procedural,
        "tiles_authored": len(tiles),
        "tiles_loaded": loaded_tile_count(world, pipe_heights),
        "tiles": tiles,
        "coins": [{"x": x, "y": y} for x, y in coins_for(world, coin_heights)],
        "enemies": [{"x": x, "y": y, "turn": t} for x, y, t in enemies_for(world)],
        "food_px": [{"x": x, "y": y} for x, y in food_for(world)],
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--world", type=int, action="append", choices=(1, 2, 3))
    parser.add_argument("-o", "--output", type=Path)
    args = parser.parse_args()
    worlds = args.world or [1, 2, 3]
    payload = [world_payload(w) for w in worlds]
    text = json.dumps(payload, indent=2)
    if args.output:
        args.output.write_text(text + "\n", encoding="utf-8")
    else:
        print(text)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
