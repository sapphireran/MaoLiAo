#!/usr/bin/env python3
"""Replay MaoLiAo's jump integrator in pure Python.

Matches Inertia::move and the vY = -sqrt(2 G REAL_HEIGHT) takeoff in role.cpp.
Used as a second implementation next to examples/src/jump_trajectory.cpp.
"""

from __future__ import annotations

import math
import sys

G = 30.0
TIME = 0.01
REAL_HEIGHT = 3.5
UNREAL_HEIGHT = 3 * 32 + 5  # 101


def integrate(v: float, t: float, a: float) -> tuple[float, float]:
    x = v * t + 0.5 * a * t * t
    return x, v + a * t


def jump_speed() -> float:
    return -math.sqrt(2.0 * G * REAL_HEIGHT)


def metres_to_pixels(m: float) -> float:
    return m * UNREAL_HEIGHT / REAL_HEIGHT


def simulate(max_ticks: int = 200) -> list[tuple[int, float, float, float]]:
    v = jump_speed()
    metres = 0.0
    out = [(0, v, 0.0, 0.0)]
    for i in range(1, max_ticks + 1):
        dx, v = integrate(v, TIME, G)
        metres += dx
        out.append((i, v, metres, metres_to_pixels(metres)))
        if v > 0.0 and metres >= 0.0:
            break
    return out


def main() -> int:
    samples = simulate()
    apex = min(samples, key=lambda s: s[2])
    print(f"takeoff={jump_speed():.12f}")
    print(f"ticks={len(samples) - 1} apex_tick={apex[0]} metres={apex[2]:.6f} px={apex[3]:.4f}")
    if abs(apex[2] + REAL_HEIGHT) > 0.02:
        print("FAIL: apex metres", file=sys.stderr)
        return 1
    if abs(apex[3] + UNREAL_HEIGHT) > 0.6:
        print("FAIL: apex pixels", file=sys.stderr)
        return 1
    print("simulate_jump.py: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
