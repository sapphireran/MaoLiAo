"""AABB helpers matching Role::isHit / hitMap / hitCoins / hitEnemy."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Iterable, Optional, Sequence, Tuple

from .constants import HEIGHT, WIDTH


Point = Tuple[int, int]


@dataclass(frozen=True)
class Rect:
    left: int
    top: int
    right: int
    bottom: int

    def as_corners(self) -> Tuple[Point, Point]:
        return (self.left, self.top), (self.right, self.bottom)


def actor_corners(x: int, y: int, x0: int = 0) -> Tuple[Point, Point, Point, Point]:
    """Four inset vertices. Role subtracts ``x0`` from X (camera space)."""
    return (
        (-x0 + x + 1, y + 1),
        (-x0 + x + WIDTH - 1, y + 1),
        (-x0 + x + 1, y + HEIGHT - 1),
        (-x0 + x + WIDTH - 1, y + HEIGHT - 1),
    )


def rect_from_tile(x_tile: int, y_tile: int, x_amount: int, y_amount: int, tile_id: int) -> Rect:
    """World-space AABB used by hitMap.

    Ids 8 and 10 (pipe mouths) occupy 2×2 tiles of pixels per authored cell.
    """
    left = x_tile * WIDTH
    top = y_tile * HEIGHT
    if tile_id in (8, 10):
        right = left + x_amount * 2 * WIDTH
        bottom = top + y_amount * 2 * HEIGHT
    else:
        right = left + x_amount * WIDTH
        bottom = top + y_amount * HEIGHT
    return Rect(left, top, right, bottom)


def is_hit(actor: Sequence[Point], box: Rect) -> bool:
    """True if any actor vertex lies inside ``box``, inclusive.

    Direct translation of Role::isHit (four points vs two-corner rect).
    """
    for px, py in actor:
        if box.left <= px <= box.right and box.top <= py <= box.bottom:
            return True
    return False


def hit_map(
    x: int,
    y: int,
    tiles: Iterable[Tuple[int, int, int, int, int]],
    x0: int = 0,
    world: int = 1,
    is_shoot: bool = False,
) -> Optional[Tuple[int, int, int, int, int]]:
    """Return the first colliding tile ``(x, y, id, xAmount, yAmount)``.

    Only ``1 <= id < 11`` is solid, matching ``hitMap``.
    On world 3 a hit on a non-cloud tile is still returned; the caller
    decides death from ``id != 2 and not is_shoot`` (same as the C++ flag).
    """
    corners = actor_corners(x, y, x0)
    for tile in tiles:
        tx, ty, tid, xa, ya = tile
        if not (1 <= tid < 11):
            continue
        if is_hit(corners, rect_from_tile(tx, ty, xa, ya, tid)):
            if world == 3 and tid != 2 and not is_shoot:
                return tile
            return tile
    return None


def lethal_world3(tile: Optional[Tuple[int, int, int, int, int]], is_shoot: bool) -> bool:
    if tile is None:
        return False
    return tile[2] != 2 and not is_shoot
