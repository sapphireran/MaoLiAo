"""Inertia::move from MaoLiAo/inertia.cpp.

    X = v * t + 1/2 * a * t^2
    v = v + a * t
    return X

Velocity is updated in place (passed by reference in C++).
"""

from __future__ import annotations

from dataclasses import dataclass


def step(v: float, t: float, a: float) -> tuple[float, float]:
    """Return (displacement, new_velocity) for one constant-accel tick."""
    displacement = v * t + 0.5 * a * t * t
    new_v = v + a * t
    return displacement, new_v


@dataclass
class Body:
    """Tiny stand-in for Hero xx/yy + vX/vY in real units."""

    x: float = 0.0
    y: float = 0.0
    vx: float = 0.0
    vy: float = 0.0

    def integrate(self, t: float, ax: float, ay: float) -> None:
        dx, self.vx = step(self.vx, t, ax)
        dy, self.vy = step(self.vy, t, ay)
        self.x += dx
        self.y += dy
