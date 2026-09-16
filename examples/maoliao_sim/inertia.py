"""Kinematic step copied from Inertia::move and Role jump / friction."""

from __future__ import annotations

import math
from typing import Iterable, List, Tuple

from .constants import G, PIXEL_SCALE, REAL_HEIGHT, T1, T2, T3, TIME, V_MAX


def move(v: float, t: float, a: float) -> Tuple[float, float]:
    """Return ``(displacement, new_velocity)``.

    C++ mutates ``v`` in place:

    ::

        X = v * t + 1.0 / 2.0 * a * t * t
        v = v + a * t
        return X
    """
    displacement = v * t + 0.5 * a * t * t
    return displacement, v + a * t


def jump_launch_vy() -> float:
    """``vY = -sqrt(2 * G * REAL_HEIGHT)`` from Role::action."""
    return -math.sqrt(2.0 * G * REAL_HEIGHT)


def friction_u(tile_id: int, world: int) -> float:
    """Surface coefficient baked in Scene::createMap.

    Worlds 1–2: ids 1/3/4/5 and 2 use T2; id 6 uses T1; else T3.
    World 3: ids 1/3/4/5/6 use T1; id 2 uses T2; else T3.
    """
    if world == 3:
        if tile_id in (1, 3, 4, 5, 6):
            t = T1
        elif tile_id == 2:
            t = T2
        else:
            t = T3
    else:
        if tile_id in (1, 2, 3, 4, 5):
            t = T2
        elif tile_id == 6:
            t = T1
        else:
            t = T3
    return (V_MAX / t) / G


def friction_k(vx: float) -> float:
    """Signed 2× scale from the ``k = |v|/v ± 3`` block in Role::action."""
    if vx == 0:
        return 0.0
    xabs = abs(vx)
    if vx < 0:
        return xabs / vx + 3.0
    return xabs / vx - 3.0


def integrate_jump(
    frames: int | None = None,
    dt: float = TIME,
    launch_vy: float | None = None,
) -> List[Tuple[int, float, float, float]]:
    """Integrate an unobstructed jump.

    Each row is ``(frame, vY_before, delta_pixels, y_pixels_up)``.
    ``y_pixels_up`` is the rise relative to launch (positive = up).
    Stops at apex+descent back to y<=0, or after ``frames`` steps.
    """
    vy = jump_launch_vy() if launch_vy is None else launch_vy
    y_up = 0.0
    rows: List[Tuple[int, float, float, float]] = []
    limit = frames if frames is not None else 10_000
    for frame in range(1, limit + 1):
        before = vy
        meters, vy = move(vy, dt, G)
        # Role: yy -= -move(...) * scale  => rise when meters is negative
        delta_px = -meters * PIXEL_SCALE
        y_up += delta_px
        rows.append((frame, before, delta_px, y_up))
        if frames is None and frame > 2 and y_up <= 0.0 and vy > 0:
            break
    return rows


def peak_jump_pixels(dt: float = TIME) -> Tuple[float, int]:
    """Return ``(max rise in pixels, frame index of the peak)``."""
    peak = 0.0
    peak_frame = 0
    for frame, _before, _dx, y_up in integrate_jump(dt=dt):
        if y_up > peak:
            peak = y_up
            peak_frame = frame
    return peak, peak_frame


def horizontal_slide(
    vx0: float,
    accel: float,
    u: float,
    grounded: bool,
    steps: int,
    dt: float = TIME,
) -> Iterable[Tuple[int, float, float]]:
    """Yield ``(frame, vx, pixels)`` for a straight-line slide.

    Applies the same ``a1 = k * G * u`` rule when ``vx * accel <= 0``.
    """
    vx = vx0
    x = 0.0
    for frame in range(1, steps + 1):
        a1 = 0.0
        if vx != 0 and vx * accel <= 0:
            if grounded:
                a1 = friction_k(vx) * G * u
        tmp = vx
        meters, vx = move(vx, dt, accel + a1)
        if tmp * vx < 0:
            vx = 0.0
        if abs(vx) > V_MAX:
            vx = math.copysign(V_MAX, vx)
        x += meters * PIXEL_SCALE
        yield frame, vx, x
