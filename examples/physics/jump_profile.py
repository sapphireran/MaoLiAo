#!/usr/bin/env python3
"""Print the unobstructed jump arc from Role::action."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from maoliao_sim.constants import G, PIXEL_SCALE, REAL_HEIGHT, TIME, UNREAL_HEIGHT
from maoliao_sim.inertia import integrate_jump, jump_launch_vy, peak_jump_pixels


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--every", type=int, default=8, help="print every Nth frame after the first 8")
    args = parser.parse_args()

    launch = jump_launch_vy()
    peak, peak_frame = peak_jump_pixels()
    rows = integrate_jump()

    print("MaoLiAo jump profile")
    print(f"  vY0            = {launch:.6f}  (-sqrt(2*G*h) = -sqrt({2*G*REAL_HEIGHT}))")
    print(f"  TIME           = {TIME}")
    print(f"  scale          = {PIXEL_SCALE:.6f}  ({UNREAL_HEIGHT}/{REAL_HEIGHT})")
    print(f"  peak           = {peak:.3f} px at frame {peak_frame}")
    print(f"  theoretical h  = {UNREAL_HEIGHT} px (authored UNREAL_HEIGHT)")
    print(f"  airtime frames = {len(rows)}")
    print()
    print("frame  vY_before     dpx       y_up")
    for frame, before, dpx, y_up in rows:
        apex = before <= 0 < (before + G * TIME)
        if frame <= 8 or apex or frame == peak_frame or frame % args.every == 0 or frame == len(rows):
            print(f"{frame:5d}  {before:10.6f}  {dpx:8.3f}  {y_up:8.3f}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
