#!/usr/bin/env python3
"""List walkable gaps and whether a V_MAX jump clears them."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from kit.reachability import classify_gaps, jump_envelope, ledges  # noqa: E402
from kit.worlds import load_world  # noqa: E402


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--world", type=int, default=0, help="1/2/3, or 0 for all")
    p.add_argument("--seed", type=int, default=2020)
    args = p.parse_args()
    print(f"flat jump envelope at V_MAX: {jump_envelope():.2f} px")
    worlds = (1, 2, 3) if args.world == 0 else (args.world,)
    for i in worlds:
        world = load_world(i, seed=args.seed)
        print(f"\nworld {i} {world.title}")
        found = ledges(world) or classify_gaps(world)
        if not found:
            print("  (no walkable gaps)")
            continue
        for g in found:
            flag = "OK " if g.jumpable_at_vmax else "WIDE"
            if hasattr(g, "left_y"):
                print(f"  {flag}  ({g.left_tile},{g.left_y}) -> ({g.right_tile},{g.right_y})  "
                      f"{g.width_px:4d} px  {g.note}")
            else:
                print(f"  {flag}  y={g.y:2d}  tiles {g.left_tile:3d} .. {g.right_tile:3d}  "
                      f"{g.width_px:4d} px  {g.note}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
