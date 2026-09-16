"""Kinematic step copied from MaoLiAo/inertia.cpp and Role::action."""

from __future__ import annotations

import math

from .constants import (
    A_ROLE,
    G,
    PIXELS_PER_METER,
    REAL_HEIGHT,
    TIME,
    UNREAL_HEIGHT,
    V_MAX,
)


def step(v: float, t: float, a: float) -> tuple[float, float]:
    """One Inertia::move call.

    Returns (displacement_meters, new_v). The C++ function writes the new
    speed back through a double&.
    """
    displacement = v * t + 0.5 * a * t * t
    return displacement, v + a * t


def to_pixels(meters: float) -> float:
    return meters * UNREAL_HEIGHT / REAL_HEIGHT


def jump_speed() -> float:
    """vY launch: -sqrt(2 * G * REAL_HEIGHT). Up is negative."""
    return -math.sqrt(2.0 * G * REAL_HEIGHT)


def friction_k(v_x: float) -> float:
    """Sign helper from Role::action. |k| is 2; sign opposes v_x."""
    if v_x == 0:
        raise ValueError("friction_k is only defined for vX != 0")
    xabs = abs(v_x)
    if v_x < 0:
        return xabs / v_x + 3.0
    return xabs / v_x - 3.0


def friction_u(tile_id: int, world: int) -> float:
    """u = (V_MAX / T) / G with the same ID switch as Scene::createMap."""
    if world == 3:
        if tile_id in (1, 3, 4, 5, 6):
            t = 0.5  # T1
        elif tile_id == 2:
            t = 1.2  # T2
        else:
            t = 1.5  # T3
    else:
        if tile_id in (1, 2, 3, 4, 5):
            t = 1.2
        elif tile_id == 6:
            t = 0.5
        else:
            t = 1.5
    return (V_MAX / t) / G


def friction_accel(v_x: float, run_accel: float, u: float | None) -> float:
    """a1 in Role::action. Zero in air or when powered in the same direction."""
    if v_x == 0:
        return 0.0
    if u is None:
        return 0.0
    if v_x * run_accel <= 0:
        return friction_k(v_x) * G * u
    return 0.0


def integrate_vertical(yy: float, v_y: float, dt: float = TIME) -> tuple[float, float]:
    """yy += move(vY, dt, G) * scale. Matches the jump integrator."""
    meters, new_v = step(v_y, dt, G)
    return yy + to_pixels(meters), new_v


def integrate_horizontal(
    xx: float,
    v_x: float,
    run_accel: float,
    u: float | None,
    dt: float = TIME,
) -> tuple[float, float]:
    """xx += move(vX, dt, a + a1) * scale, then zero v if it flipped sign."""
    a1 = friction_accel(v_x, run_accel, u)
    old = v_x
    meters, new_v = step(v_x, dt, run_accel + a1)
    if old * new_v < 0:
        new_v = 0.0
    if abs(new_v) > V_MAX:
        new_v = math.copysign(V_MAX, new_v)
    return xx + to_pixels(meters), new_v


def powered_run_ticks_to_vmax() -> int:
    """Ticks of held-right on the ground with a1 == 0 until V_MAX."""
    v = 0.0
    n = 0
    while v < V_MAX - 1e-12:
        _, v = step(v, TIME, A_ROLE)
        n += 1
        if n > 10_000:
            raise RuntimeError("V_MAX was not reached")
    return n


def jump_apex_ticks() -> tuple[int, float, float]:
    """Open-air jump from y=0. Returns (ticks_to_apex, peak_rise_px, v_at_apex)."""
    v = jump_speed()
    y = 0.0
    peak_rise = 0.0
    ticks = 0
    while v < 0:
        y, v = integrate_vertical(y, v)
        rise = -y
        if rise > peak_rise:
            peak_rise = rise
        ticks += 1
        if ticks > 10_000:
            raise RuntimeError("apex was not reached")
    return ticks, peak_rise, v
