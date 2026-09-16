"""Horizontal friction accel a1 from Role::action."""

from __future__ import annotations

from .constants import A_ROLE, G, TIME, V_MAX
from .inertia import step
from .tiles import friction_for


def k_from_vx(vx: float) -> float:
    """The sign fudge around lines 519–527 of role.cpp."""
    if vx == 0:
        return 0.0
    xabs = abs(vx)
    if vx < 0:
        return xabs / vx + 3  # -1 + 3 = 2
    return xabs / vx - 3  # 1 - 3 = -2


def friction_accel(vx: float, walk_accel: float, u: float, airborne: bool) -> float:
    """a1 is applied only when vX * a <= 0, vX != 0, and a tile is underfoot."""
    if airborne or vx == 0 or vx * walk_accel > 0:
        return 0.0
    return k_from_vx(vx) * G * u


def walk_accel(left: bool, right: bool, ending: bool) -> float:
    a = 0.0
    if left and not ending:
        a -= A_ROLE
    if right or ending:
        a += A_ROLE
    return a


def cap_vx(vx: float) -> float:
    xabs = abs(vx)
    if xabs > V_MAX:
        return vx / xabs * V_MAX
    return vx


def skim_to_stop(vx: float, u: float, max_ticks: int = 500) -> int:
    """Ticks of friction-only motion until vx hits 0 or flips (then clamped)."""
    ticks = 0
    while vx != 0 and ticks < max_ticks:
        a1 = friction_accel(vx, walk_accel=0.0, u=u, airborne=False)
        _, new_vx = step(vx, TIME, a1)
        if vx * new_vx < 0:
            return ticks + 1
        vx = cap_vx(new_vx)
        ticks += 1
    return ticks


def u_for_tile(tile_id: int, world: int) -> float:
    return friction_for(tile_id, world)
