"""Four-point inset AABB test from Role::isHit / hitMap."""

from __future__ import annotations

from dataclasses import dataclass

from .constants import C, friction_u


@dataclass(frozen=True)
class Tile:
    x: int
    y: int
    id: int
    xAmount: int = 1
    yAmount: int = 1

    @property
    def solid(self) -> bool:
        return 0 < self.id < 11

    @property
    def friction_t(self) -> float:
        if self.id == 6:
            return C.T1
        if self.id in (1, 2, 3, 4, 5):
            return C.T2
        return C.T3

    @property
    def u(self) -> float:
        return friction_u(self.friction_t)


def tile_aabb(tile: Tile) -> tuple[float, float, float, float]:
    x0 = tile.x * C.WIDTH
    y0 = tile.y * C.HEIGHT
    if tile.id in (8, 10):
        x1 = x0 + tile.xAmount * 2 * C.WIDTH
        y1 = y0 + tile.yAmount * 2 * C.HEIGHT
    else:
        x1 = x0 + tile.xAmount * C.WIDTH
        y1 = y0 + tile.yAmount * C.HEIGHT
    return x0, y0, x1, y1


def inset_corners(world_x: float, y: float, w: int = C.WIDTH, h: int = C.HEIGHT):
    return (
        (world_x + 1, y + 1),
        (world_x + w - 1, y + 1),
        (world_x + 1, y + h - 1),
        (world_x + w - 1, y + h - 1),
    )


def aabb_contains_point(box, px: float, py: float) -> bool:
    x0, y0, x1, y1 = box
    return x0 <= px <= x1 and y0 <= py <= y1


def is_hit(corners, box) -> bool:
    return any(aabb_contains_point(box, px, py) for px, py in corners)


def hit_tile(world_x: float, y: float, tiles: list[Tile], *, world: int = 1, is_shoot: bool = False):
    """First solid tile that contains an inset corner, or None.

    When world==3 a non-cloud solid is lethal unless is_shoot (flower).
    Returns (tile, lethal).
    """
    corners = inset_corners(world_x, y)
    for tile in tiles:
        if not tile.solid:
            continue
        if is_hit(corners, tile_aabb(tile)):
            lethal = world == 3 and tile.id != 2 and not is_shoot
            return tile, lethal
    return None, False


def hit_rect(world_x: float, y: float, box) -> bool:
    return is_hit(inset_corners(world_x, y), box)
