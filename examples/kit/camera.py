"""Scroll rail and ending tripwire."""

from __future__ import annotations

from .constants import C


def world_x(x: float, x0: float) -> float:
    return -x0 + x


def apply_rail(x: float, x0: float, ending: bool) -> tuple[float, float, bool]:
    """Return (x, x0, scrolled). Mirrors the XRIGHT pin in Role::action."""
    if x < C.XLEFT:
        return float(C.XLEFT), x0, False
    if x > C.XRIGHT and not ending:
        x0 = x0 - (x - C.XRIGHT)
        return float(C.XRIGHT), x0, True
    return x, x0, False


def is_ending(distance: float, world: int) -> bool:
    return distance > C.ENDING[world] * C.WIDTH


def sky_step(vx: float) -> float:
    return abs(vx) * C.TIME * C.UNREAL_HEIGHT / C.REAL_HEIGHT / C.K_MAP_BG
