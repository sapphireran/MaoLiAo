#!/usr/bin/env python3
"""Print the unobstructed jump arc from Role::action."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from kit.kinematics import apex, jump_launch_vy, jump_profile  # noqa: E402
from kit.constants import C, pixel_scale  # noqa: E402


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--csv", action="store_true")
    args = p.parse_args()
    samples = jump_profile()
    top = apex(samples)
    if args.csv:
        print("frame,t,vY,rise_px")
        for s in samples:
            print(f"{s.frame},{s.t:.4f},{s.vY:.6f},{s.rise_px:.4f}")
    else:
        print(f"launch vY     {jump_launch_vy():.9f}")
        print(f"pixel scale   {pixel_scale():.9f}  (101/3.5)")
        print(f"G, TIME       {C.G}, {C.TIME}")
        print(f"apex          {top.rise_px:.4f} px at frame {top.frame} (t={top.t:.4f}s)")
        print(f"hang frames   {len(samples)}")
        print(f"authored UNREAL_HEIGHT {C.UNREAL_HEIGHT}")
        print()
        print(f"{'frame':>6} {'t':>7} {'vY':>12} {'rise_px':>10}")
        for s in samples:
            mark = "  <-- apex" if s.frame == top.frame else ""
            print(f"{s.frame:6d} {s.t:7.3f} {s.vY:12.6f} {s.rise_px:10.3f}{mark}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
