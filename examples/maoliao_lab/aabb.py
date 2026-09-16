"""Four-corner overlap copied from Role::isHit / hitMap / hitCoins."""

from __future__ import annotations

from dataclasses import dataclass

from .constants import HEIGHT, PIPE_MOUTH_IDS, SOLID_ID_MAX, WIDTH, X0


@dataclass(frozen=True)
class Point:
    x: float
    y: float


@dataclass(frozen=True)
class Rect:
    """Inclusive top-left / bottom-right, matching the C++ POINT m[2] pair."""

    left: float
    top: float
    right: float
    bottom: float

    def contains(self, p: Point) -> bool:
        return self.left <= p.x <= self.right and self.top <= p.y <= self.bottom


def sprite_corners(x: float, y: float, x0: float = 0.0) -> tuple[Point, Point, Point, Point]:
    """Role four corners, inset by 1 px, in world space (-x0 + x)."""
    return (
        Point(-x0 + x + 1, y + 1),
        Point(-x0 + x + WIDTH - 1, y + 1),
        Point(-x0 + x + 1, y + HEIGHT - 1),
        Point(-x0 + x + WIDTH - 1, y + HEIGHT - 1),
    )


def is_hit(corners: tuple[Point, ...], box: Rect) -> bool:
    return any(box.contains(p) for p in corners)


def tile_rect(cell_x: int, cell_y: int, x_amount: int, y_amount: int, tile_id: int) -> Rect:
    left = cell_x * WIDTH
    top = cell_y * HEIGHT
    if tile_id in PIPE_MOUTH_IDS:
        right = left + x_amount * 2 * WIDTH
        bottom = top + y_amount * 2 * HEIGHT
    else:
        right = left + x_amount * WIDTH
        bottom = top + y_amount * HEIGHT
    return Rect(left, top, right, bottom)


def hit_map(
    x: float,
    y: float,
    tiles: list[dict],
    *,
    x0: float = 0.0,
    world: int = 1,
    is_shoot: bool = False,
) -> dict | None:
    """First solid tile under the sprite, plus World 3 death flag.

    Returns the tile dict (with an extra ``died`` bool) or None.
    Scenery (id >= 11) is skipped, matching hitMap's ``id < 11`` loop.
    """
    corners = sprite_corners(x, y, x0)
    for tile in tiles:
        tile_id = int(tile["id"])
        if not (0 < tile_id <= SOLID_ID_MAX):
            continue
        box = tile_rect(tile["x"], tile["y"], tile["xAmount"], tile["yAmount"], tile_id)
        if is_hit(corners, box):
            died = world == 3 and tile_id != 2 and not is_shoot
            return {**tile, "died": died}
    return None


def hit_coin(x: float, y: float, coins: list[tuple[int, int]], x0: float = 0.0) -> tuple[int, int] | None:
    corners = sprite_corners(x, y, x0)
    for cx, cy in coins:
        if cx == 0 and cy == 0:
            continue
        box = Rect(cx * WIDTH, cy * HEIGHT, cx * WIDTH + WIDTH, cy * HEIGHT + HEIGHT)
        if is_hit(corners, box):
            return (cx, cy)
    return None


def food_rect(px: float, py: float) -> Rect:
    """Food AABB: 3*WIDTH/2+4 by 4*HEIGHT/5, pixel coordinates."""
    return Rect(px, py, px + 3 * WIDTH / 2 + 4, py + 4 * HEIGHT / 5)


SPAWN_Y_ACTUAL = X0  # Role::Role assigns y = X0, not Y0
