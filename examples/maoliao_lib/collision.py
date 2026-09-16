"""Corner-inset overlap tests from Role::isHit / hitMap / hitCoins / hitEnemy."""

from __future__ import annotations

from dataclasses import dataclass

from .constants import (
    HEIGHT,
    PIPE_DOUBLE_IDS,
    SAFE_TILE_WORLD3,
    WIDTH,
    is_solid_id,
)


Point = tuple[float, float]
Box = tuple[Point, Point]


def hero_corners(x: float, y: float, xmap: float = 0.0) -> list[Point]:
    """Four inset corners in world space. xmap is Hero::x0."""
    return [
        (-xmap + x + 1, y + 1),
        (-xmap + x + WIDTH - 1, y + 1),
        (-xmap + x + 1, y + HEIGHT - 1),
        (-xmap + x + WIDTH - 1, y + HEIGHT - 1),
    ]


def is_hit(corners: list[Point], box: Box) -> bool:
    """True if any corner sits inside the inclusive AABB (p2[0] .. p2[1])."""
    (x0, y0), (x1, y1) = box
    for x, y in corners:
        if x0 <= x <= x1 and y0 <= y <= y1:
            return True
    return False


def boxes_overlap_full(a: Box, b: Box) -> bool:
    """True AABB vs AABB. The game does not use this; tests contrast it with is_hit."""
    (ax0, ay0), (ax1, ay1) = a
    (bx0, by0), (bx1, by1) = b
    return ax0 <= bx1 and ax1 >= bx0 and ay0 <= by1 and ay1 >= by0


@dataclass(frozen=True)
class Tile:
    x: int
    y: int
    id: int
    x_amount: int
    y_amount: int
    u: float = 0.0

    def aabb(self) -> Box:
        x0 = self.x * WIDTH
        y0 = self.y * HEIGHT
        if self.id in PIPE_DOUBLE_IDS:
            x1 = x0 + self.x_amount * 2 * WIDTH
            y1 = y0 + self.y_amount * 2 * HEIGHT
        else:
            x1 = x0 + self.x_amount * WIDTH
            y1 = y0 + self.y_amount * HEIGHT
        return ((x0, y0), (x1, y1))


def coin_box(tile_x: int, tile_y: int) -> Box:
    x0 = tile_x * WIDTH
    y0 = tile_y * HEIGHT
    return ((x0, y0), (x0 + WIDTH, y0 + HEIGHT))


def food_box(px: float, py: float) -> Box:
    return ((px, py), (px + 3 * WIDTH / 2 + 4, py + 4 * HEIGHT / 5))


def enemy_box(px: float, py: float) -> Box:
    return ((px, py), (px + WIDTH, py + HEIGHT))


@dataclass
class MapHit:
    tile: Tile
    died: bool


def hit_map(
    x: float,
    y: float,
    tiles: list[Tile],
    xmap: float = 0.0,
    world: int = 1,
    is_shoot: bool = False,
    cap: int | None = None,
) -> MapHit | None:
    """First solid tile whose AABB contains a hero corner.

    When world == 3 the C++ function may also set Hero::died. That side
    effect is returned here instead of being written onto a hero.
    `cap` is MAP_NUMBER when reproducing the 30-slot array.
    """
    corners = hero_corners(x, y, xmap)
    sequence = tiles if cap is None else tiles[:cap]
    for tile in sequence:
        if not is_solid_id(tile.id):
            continue
        if is_hit(corners, tile.aabb()):
            died = world == 3 and tile.id != SAFE_TILE_WORLD3 and not is_shoot
            return MapHit(tile, died)
    return None


def hit_coins(
    x: float,
    y: float,
    coins: list[tuple[int, int]],
    xmap: float = 0.0,
) -> int | None:
    """Index of the first live coin (not 0,0) that overlaps, else None."""
    corners = hero_corners(x, y, xmap)
    for i, (cx, cy) in enumerate(coins):
        if cx == 0 and cy == 0:
            continue
        if is_hit(corners, coin_box(cx, cy)):
            return i
    return None


def hit_food(
    x: float,
    y: float,
    foods: list[tuple[float, float]],
    xmap: float = 0.0,
) -> int | None:
    corners = hero_corners(x, y, xmap)
    for i, (fx, fy) in enumerate(foods):
        if fx == 0 and fy == 0:
            continue
        if is_hit(corners, food_box(fx, fy)):
            return i
    return None


def hit_enemy(
    x: float,
    y: float,
    enemies: list[tuple[float, float, int]],
    xmap: float = 0.0,
) -> int | None:
    """enemies entries are (world_x, world_y, turn). turn == 0 is unused."""
    corners = hero_corners(x, y, xmap)
    for i, (ex, ey, turn) in enumerate(enemies):
        if turn == 0:
            continue
        if is_hit(corners, enemy_box(ex, ey)):
            return i
    return None
