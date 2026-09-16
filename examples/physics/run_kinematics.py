#!/usr/bin/env python3
"""Compare grass vs ice slides and print friction coefficients."""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from maoliao_sim.constants import A_ROLE, V_MAX
from maoliao_sim.inertia import friction_u, horizontal_slide


def _table(title: str, vx0: float, accel: float, u: float, grounded: bool, steps: int = 40) -> None:
    print(title)
    print("frame      vx        x_px")
    for frame, vx, x in horizontal_slide(vx0, accel, u, grounded, steps):
        if frame == 1 or frame % 5 == 0 or vx == 0 or frame == steps:
            print(f"{frame:5d}  {vx:8.4f}  {x:9.2f}")
    print()


def main() -> int:
    print("Friction u = (V_MAX / T) / G")
    for world in (1, 3):
        print(f"  world {world}:")
        for tile_id in (1, 2, 6, 7, 10):
            print(f"    id {tile_id:2d}  u={friction_u(tile_id, world):.6f}")
    print()

    grass = friction_u(1, 1)
    ice = friction_u(6, 1)
    _table("Release from V_MAX on grass (id 1, world 1), no input", V_MAX, 0.0, grass, True)
    _table("Release from V_MAX on ice (id 6, world 1), no input", V_MAX, 0.0, ice, True)
    _table("Airborne release from V_MAX (u ignored)", V_MAX, 0.0, grass, False, steps=20)
    _table("Hold right from rest on grass", 0.0, A_ROLE, grass, True, steps=30)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
