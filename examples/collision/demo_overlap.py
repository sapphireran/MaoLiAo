#!/usr/bin/env python3
"""Show why isHit is not a full AABB test, using a 10x10 box on the cat."""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao_lib.collision import boxes_overlap_full, hero_corners, is_hit  # noqa: E402


def main() -> int:
    corners = hero_corners(0, 0, 0)
    hero = ((0, 0), (32, 32))
    center = ((11, 11), (21, 21))
    edge = ((0, 0), (2, 2))
    print("hero corners (inset 1 px):")
    for c in corners:
        print(f"  {c}")
    print()
    print(f"center 10x10  full_aabb={boxes_overlap_full(hero, center)}  "
          f"is_hit={is_hit(corners, center)}")
    print(f"corner 2x2    full_aabb={boxes_overlap_full(hero, edge)}  "
          f"is_hit={is_hit(corners, edge)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
