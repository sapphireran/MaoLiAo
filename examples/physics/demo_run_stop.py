#!/usr/bin/env python3
"""Hold right to V_MAX, then coast against T2 friction until stop."""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao_lib.constants import A_ROLE, TIME, V_MAX  # noqa: E402
from maoliao_lib.inertia import friction_u, integrate_horizontal  # noqa: E402


def main() -> int:
    u = friction_u(1, 1)  # grass / T2
    xx = 0.0
    vx = 0.0
    print("phase     tick   vx        xx_px")
    print("--------  -----  --------  --------")
    n = 0
    while vx < V_MAX - 1e-12:
        n += 1
        xx, vx = integrate_horizontal(xx, vx, A_ROLE, u)
        if n <= 5 or n % 10 == 0 or vx >= V_MAX - 1e-12:
            print(f"powered   {n:5d}  {vx:8.4f}  {xx:8.2f}")
    powered_ticks = n
    powered_x = xx

    coast = 0
    while abs(vx) > 1e-6:
        coast += 1
        xx, vx = integrate_horizontal(xx, vx, 0.0, u)
        if coast <= 5 or coast % 10 == 0 or abs(vx) <= 1e-6:
            print(f"coast     {coast:5d}  {vx:8.4f}  {xx:8.2f}")
        if coast > 10_000:
            raise RuntimeError("did not stop")

    print()
    print(f"T2 u                 {u:.6f}")
    print(f"powered ticks        {powered_ticks}  (expect 40)")
    print(f"pixels while powered {powered_x:.2f}")
    print(f"coast ticks          {coast}")
    print(f"pixels while coast   {xx - powered_x:.2f}")
    print(f"dt                   {TIME}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
