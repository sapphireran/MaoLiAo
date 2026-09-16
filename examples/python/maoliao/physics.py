"""Kinematics matching Inertia::move and Role::action's jump / run."""

from __future__ import annotations

import math
from dataclasses import dataclass

from .constants import (
    A_ROLE,
    G,
    PIXELS_PER_METRE,
    REAL_HEIGHT,
    TIME,
    V_MAX,
    friction_u,
)


def move(v: float, t: float, a: float) -> tuple[float, float]:
    """Return (displacement, new_velocity) for constant acceleration.

    Matches inertia.cpp:
        X = v*t + 1/2*a*t*t
        v = v + a*t
    """
    displacement = v * t + 0.5 * a * t * t
    return displacement, v + a * t


def jump_initial_vy() -> float:
    """vY = -sqrt(2 * G * REAL_HEIGHT)."""
    return -math.sqrt(2.0 * G * REAL_HEIGHT)


@dataclass(frozen=True)
class JumpSample:
    tick: int
    t: float
    vy: float
    y_metres: float
    y_pixels: float
    airborne: bool


def simulate_jump(
    ticks: int = 80,
    dt: float = TIME,
    ceiling: float | None = None,
) -> list[JumpSample]:
    """Ballistic jump with no floor, in the same units as Role::action.

    y_pixels starts at 0 and increases downward (EasyX y-down) because
    the game does yy += move(vY, TIME, +G) * scale.
    """
    vy = jump_initial_vy()
    y_m = 0.0
    y_px = 0.0
    out: list[JumpSample] = [
        JumpSample(0, 0.0, vy, y_m, y_px, True),
    ]
    for tick in range(1, ticks + 1):
        disp, vy = move(vy, dt, G)
        y_m += disp
        y_px += disp * PIXELS_PER_METRE
        if ceiling is not None and y_px > ceiling:
            y_px = ceiling
            y_m = y_px / PIXELS_PER_METRE
            vy = 0.0
            out.append(JumpSample(tick, tick * dt, vy, y_m, y_px, False))
            break
        out.append(JumpSample(tick, tick * dt, vy, y_m, y_px, True))
    return out


def apex_time_seconds() -> float:
    """Time to vy = 0: t = |v0| / G."""
    return abs(jump_initial_vy()) / G


def apex_pixels() -> float:
    """Peak height in pixels (upward, so a negative EasyX delta)."""
    v0 = jump_initial_vy()
    t = apex_time_seconds()
    disp, _ = move(v0, t, G)
    return disp * PIXELS_PER_METRE


def run_step(
    vx: float,
    a_control: float,
    on_ground: bool,
    friction_bucket: str | None,
    dt: float = TIME,
) -> tuple[float, float]:
    """One horizontal tick. Returns (pixel displacement, new vx).

    Friction acceleration a1 is applied only when v*a <= 0 and v != 0,
    matching role.cpp. k is +3 (moving left) or -3 (moving right).
    """
    a1 = 0.0
    if vx != 0.0 and vx * a_control <= 0:
        if on_ground and friction_bucket is not None:
            k = 3.0 if vx < 0 else -3.0
            a1 = k * G * friction_u(friction_bucket)
    prev = vx
    disp, vx = move(vx, dt, a_control + a1)
    if prev * vx < 0:
        vx = 0.0
    if abs(vx) > V_MAX:
        vx = math.copysign(V_MAX, vx)
    return disp * PIXELS_PER_METRE, vx


def hold_right_until_vmax(friction_bucket: str = "T2", dt: float = TIME) -> int:
    """Ticks of holding right on the ground to reach V_MAX (or 0 if already)."""
    vx = 0.0
    for tick in range(1, 10_000):
        _, vx = run_step(vx, A_ROLE, True, friction_bucket, dt)
        if abs(vx - V_MAX) < 1e-9 or vx >= V_MAX:
            return tick
    raise RuntimeError("did not reach V_MAX")
