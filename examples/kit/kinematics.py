"""Inertia::move, jump arc, and a single horizontal coast step."""

from __future__ import annotations

from dataclasses import dataclass
import math

from .constants import C, launch_vy, pixel_scale


def inertia_move(v: float, t: float, a: float) -> tuple[float, float]:
    """Return (displacement, new_velocity). Matches Inertia::move."""
    x = v * t + 0.5 * a * t * t
    return x, v + a * t


def jump_launch_vy() -> float:
    return launch_vy()


def metres_to_pixels(dx: float) -> float:
    return dx * pixel_scale()


@dataclass(frozen=True)
class JumpSample:
    frame: int
    t: float
    vY: float
    rise_px: float


def jump_profile(max_frames: int = 200) -> list[JumpSample]:
    """Unobstructed jump. y decreases on screen; rise_px is how far up."""
    v = jump_launch_vy()
    y = 0.0
    out: list[JumpSample] = []
    for frame in range(1, max_frames + 1):
        dx, v = inertia_move(v, C.TIME, C.G)
        y += metres_to_pixels(dx)
        out.append(JumpSample(frame, frame * C.TIME, v, -y))
        if y > 0 and frame > 2:
            break
    return out


def apex(samples: list[JumpSample] | None = None) -> JumpSample:
    samples = samples or jump_profile()
    return max(samples, key=lambda s: s.rise_px)


def friction_accel(vx: float, u: float) -> float:
    """a1 = k * G * u with k = ±2. Idle vx==0 is the NaN path in C++; here 0."""
    if vx == 0.0:
        return 0.0
    k = -2.0 if vx > 0.0 else 2.0
    return k * C.G * u


def coast_step(vx: float, u: float, grounded: bool) -> tuple[float, float]:
    """One TIME step with no walk input. Returns (dx_px, new_vx)."""
    a = friction_accel(vx, u) if grounded else 0.0
    if vx * a <= 0 and vx != 0:
        pass
    else:
        a = 0.0
    dx, nv = inertia_move(vx, C.TIME, a)
    if vx * nv < 0:
        nv = 0.0
    if abs(nv) > C.V_MAX:
        nv = math.copysign(C.V_MAX, nv)
    return metres_to_pixels(dx), nv


def walk_step(vx: float, direction: int, u: float, grounded: bool) -> tuple[float, float]:
    """direction is -1, 0, or +1. Ending rail uses +1."""
    a = C.A_ROLE * direction
    a1 = 0.0
    if vx != 0 and vx * a <= 0:
        if grounded:
            a1 = friction_accel(vx, u)
    dx, nv = inertia_move(vx, C.TIME, a + a1)
    if vx * nv < 0:
        nv = 0.0
    if abs(nv) > C.V_MAX:
        nv = math.copysign(C.V_MAX, nv)
    return metres_to_pixels(dx), nv
