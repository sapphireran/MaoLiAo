"""Camera pin + sky parallax from Role::action / Scene::action."""

from __future__ import annotations

from dataclasses import dataclass

from .constants import (
    ENDING_DISTANCE,
    K_MAP_BG,
    PIXELS_PER_REAL,
    TIME,
    XLEFT,
    XRIGHT,
    XSIZE,
)


@dataclass
class Camera:
    x0: float = 0.0  # world origin, ≤ 0 while scrolling right
    x_bg: float = 0.0

    def pin_hero_screen_x(self, x: float, vx: float, ending: bool) -> float:
        """Keep the cat in [XLEFT, XRIGHT] and eat overflow into x0."""
        if x < XLEFT:
            return XLEFT
        if x > XRIGHT and not ending:
            self.x0 -= x - XRIGHT
            return float(XRIGHT)
        return x

    def step_parallax(self, vx: float, screen_x: float, previous_x0: float) -> None:
        """Sky only moves when glued to the right edge, walking right, x0 changed."""
        if screen_x == XRIGHT and vx > 0 and previous_x0 != self.x0:
            bg_step = abs(vx) * TIME * PIXELS_PER_REAL / K_MAP_BG
            self.x_bg -= bg_step

    def wrap_sky(self, sky_width: float) -> None:
        if self.x_bg <= -sky_width:
            self.x_bg = 0.0


def world_x(screen_x: float, x0: float) -> float:
    return -x0 + screen_x


def is_ending(distance: float, world: int) -> bool:
    return distance > ENDING_DISTANCE[world]


def is_passed(screen_x: float) -> bool:
    return screen_x > XSIZE
