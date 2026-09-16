"""Four-corner collision matching Role::isHit / hitMap."""

from __future__ import annotations

from dataclasses import dataclass

from .constants import HEIGHT, WIDTH


@dataclass(frozen=True)
class Rect:
    left: int
    top: int
    right: int
    bottom: int


def hero_corners(x: int, y: int, x0: int = 0) -> list[tuple[int, int]]:
    """Inset 32×32 sprite, camera-corrected, as in hitMap."""
    return [
        (-x0 + x + 1, y + 1),
        (-x0 + x + WIDTH - 1, y + 1),
        (-x0 + x + 1, y + HEIGHT - 1),
        (-x0 + x + WIDTH - 1, y + HEIGHT - 1),
    ]


def is_hit(corners: list[tuple[int, int]], box: Rect) -> bool:
    """True if any corner is inside the inclusive AABB."""
    for px, py in corners:
        if box.left <= px <= box.right and box.top <= py <= box.bottom:
            return True
    return False


def tile_rect(x: int, y: int, x_amount: int, y_amount: int, tile_id: int) -> Rect:
    """World-pixel AABB. Ids 8 and 10 use 2× tile size per amount unit."""
    left = x * WIDTH
    top = y * HEIGHT
    if tile_id in (8, 10):
        right = left + x_amount * 2 * WIDTH
        bottom = top + y_amount * 2 * HEIGHT
    else:
        right = left + x_amount * WIDTH
        bottom = top + y_amount * HEIGHT
    return Rect(left, top, right, bottom)


def collides_with_map(
    x: int,
    y: int,
    maps: list[dict],
    *,
    x0: int = 0,
    world: int = 1,
    is_shoot: bool = False,
) -> dict | None:
    """Return the first colliding map record (id 1..10), or None.

    World 3 death-on-touch is reported via the extra key ``lethal`` on a
    shallow copy when the C++ would set Hero.died.
    """
    corners = hero_corners(x, y, x0)
    for rec in maps:
        tile_id = rec["id"]
        if not (1 <= tile_id <= 10):
            continue
        box = tile_rect(rec["x"], rec["y"], rec["xAmount"], rec["yAmount"], tile_id)
        if is_hit(corners, box):
            hit = dict(rec)
            hit["lethal"] = (
                world == 3 and tile_id != 2 and not is_shoot
            )
            return hit
    return None
