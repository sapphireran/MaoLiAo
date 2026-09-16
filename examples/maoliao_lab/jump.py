"""Jump arc in real units, then scaled to pixels like Role::action."""

from __future__ import annotations

from dataclasses import dataclass

from .constants import G, PIXELS_PER_REAL, REAL_HEIGHT, TIME, UNREAL_HEIGHT, jump_launch_vy
from .inertia import step


@dataclass
class JumpSample:
    tick: int
    t: float
    vy: float
    y_real: float
    y_px: float
    rising: bool


def arc(max_ticks: int = 400) -> list[JumpSample]:
    """Integrate gravity until the cat is back at y=0 (or max_ticks).

    Role applies move() with +G and then ``yy += displacement * scale``.
    Launch vy is negative (up). Apex is where vy crosses 0.
    """
    vy = jump_launch_vy()
    y_px = 0.0
    y_real = 0.0
    out: list[JumpSample] = []
    for tick in range(1, max_ticks + 1):
        dy_real, vy = step(vy, TIME, G)
        y_real += dy_real
        y_px += dy_real * PIXELS_PER_REAL
        out.append(
            JumpSample(
                tick=tick,
                t=tick * TIME,
                vy=vy,
                y_real=y_real,
                y_px=y_px,
                rising=vy < 0,
            )
        )
        if y_px >= 0 and tick > 2 and vy > 0:
            break
    return out


def apex(samples: list[JumpSample] | None = None) -> JumpSample:
    series = samples if samples is not None else arc()
    return min(series, key=lambda s: s.y_px)


def expected_peak_px() -> float:
    """v²/(2g) = REAL_HEIGHT meters → UNREAL_HEIGHT pixels (sign: up is negative)."""
    return -UNREAL_HEIGHT
