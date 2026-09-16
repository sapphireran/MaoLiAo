#!/usr/bin/env python3
"""Dump hardcoded MaoLiAo level tables from the C++ sources as JSON."""

from __future__ import annotations

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from maoliao_fixtures import (  # noqa: E402
    extract_ending_tiles,
    extract_enemies,
    extract_map_tiles,
    extract_points,
    parse_define_h,
)

def main() -> int:
    macros = parse_define_h()
    payload = {
        "macros": macros,
        "ending_distance_tiles": extract_ending_tiles(),
        "worlds": {},
    }
    for world in (1, 2):
        payload["worlds"][str(world)] = {
            "tiles": extract_map_tiles(world),
            "coins": extract_points("void Scene::createCoin", world, macros),
            "food": extract_points("void Scene::createFood", world, macros),
            "enemies": extract_enemies(world),
        }
    payload["worlds"]["3"] = {
        "food": extract_points("void Scene::createFood", 3, macros),
        "enemies": extract_enemies(3),
        "notes": "Pipes and coins are rand()-generated; see fixtures/levels/world-3.json",
    }
    json.dump(payload, sys.stdout, indent=2, sort_keys=True)
    sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
