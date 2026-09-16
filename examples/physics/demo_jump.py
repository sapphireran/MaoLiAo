#!/usr/bin/env python3
"""Print the open-air jump table that docs/physics.md describes."""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao_lib.inertia import integrate_vertical, jump_apex_ticks, jump_speed  # noqa: E402


def main() -> int:
    print("tick  y_px      vY        rise_px")
    print("----  --------  --------  -------")
    v = jump_speed()
    y = 0.0
    ticks, peak, v_apex = jump_apex_ticks()
    for n in range(1, ticks + 6):
        y, v = integrate_vertical(y, v)
        print(f"{n:4d}  {y:8.3f}  {v:8.3f}  {-y:7.3f}")
        if n == ticks:
            print(f"# apex crossing  peak_rise={peak:.3f}  v={v_apex:.3f}")
    print()
    print(f"launch vY           {jump_speed():.9f}")
    print(f"ticks to vY >= 0    {ticks}")
    print(f"peak rise (pixels)  {peak:.6f}  (designed 101)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
