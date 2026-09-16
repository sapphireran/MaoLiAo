"""Authored level tables extracted from scene.cpp and role.cpp.

Each tile is ``(x, y, id, xAmount, yAmount)``.
Each coin is ``(x, y)`` in tiles.
Each enemy is ``(x, y, turn)`` in tiles.
Food is stored in pixels, same as createFood.

World 3 pipes and coins are generated with the C ``random(a,b)`` macro
``rand()%(b-a)+a``. Pass a Python RNG that implements the same span.
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Callable, List, Sequence, Tuple

from .constants import HEIGHT, WIDTH

Tile = Tuple[int, int, int, int, int]
Coin = Tuple[int, int]
Enemy = Tuple[int, int, int]
Food = Tuple[int, int]


def _c_random(rng: Callable[[], int], a: int, b: int) -> int:
    """define.h: ``#define random(a,b) (rand()%(b-a)+a)``."""
    return rng() % (b - a) + a


@dataclass(frozen=True)
class WorldSpec:
    number: int
    name: str
    ending_tiles: int
    tiles: Tuple[Tile, ...]
    coins: Tuple[Coin, ...]
    enemies: Tuple[Enemy, ...]
    food_px: Tuple[Food, ...]
    goal: Tuple[int, int]
    procedural: bool = False


WORLD1_TILES: Tuple[Tile, ...] = (
    (0, 9, 1, 15, 1),
    (18, 8, 1, 9, 1),
    (27, 9, 5, 37, 1),
    (67, 9, 6, 4, 1),
    (74, 9, 5, 1, 1),
    (78, 9, 1, 28, 1),
    (0, 10, 3, 15, 2),
    (18, 9, 3, 9, 3),
    (27, 10, 4, 37, 2),
    (67, 10, 3, 4, 2),
    (74, 10, 4, 1, 2),
    (78, 10, 3, 28, 2),
    (10, 6, 2, 4, 1),
    (40, 4, 2, 3, 1),
    (62, 6, 2, 1, 1),
    (65, 4, 2, 5, 1),
    (36, 7, 10, 1, 1),
    (45, 7, 10, 1, 1),
    (80, 7, 10, 1, 1),
    (90, 7, 10, 1, 1),
    (19, 6, 11, 1, 1),
    (23, 6, 11, 1, 1),
    (32, 7, 11, 1, 1),
    (49, 7, 11, 1, 1),
    (53, 7, 11, 1, 1),
    (57, 7, 11, 1, 1),
    (93, 7, 11, 1, 1),
    (101, 7, 12, 1, 1),
    (15, 10, 13, 1, 1),
    (64, 10, 13, 1, 1),
    (71, 10, 13, 1, 1),
    (75, 10, 13, 1, 1),
)

WORLD1_COINS: Tuple[Coin, ...] = (
    (10, 5),
    (11, 5),
    (12, 5),
    (13, 5),
    (66, 3),
    (67, 3),
    (68, 3),
    (69, 3),
    (74, 5),
    (74, 6),
    (74, 7),
    (74, 8),
    (82, 7),
    (83, 8),
    (84, 7),
    (85, 8),
    (86, 7),
    (87, 8),
    (88, 7),
    (89, 8),
)

WORLD1_ENEMIES: Tuple[Enemy, ...] = (
    (3, 8, 1),
    (18, 7, -1),
    (25, 7, 1),
    (28, 8, -1),
    (33, 8, -1),
    (39, 8, 1),
    (68, 3, 1),
    (66, 8, -1),
    (81, 6, 1),
    (92, 6, 1),
)

WORLD2_TILES: Tuple[Tile, ...] = (
    (18, 9, 2, 4, 1),
    (23, 6, 2, 6, 1),
    (25, 3, 2, 3, 1),
    (31, 10, 2, 2, 1),
    (39, 7, 2, 1, 1),
    (39, 4, 2, 1, 1),
    (96, 7, 2, 2, 1),
    (99, 5, 2, 2, 1),
    (102, 3, 2, 12, 1),
    (0, 10, 5, 15, 1),
    (38, 10, 5, 10, 1),
    (50, 10, 5, 1, 1),
    (53, 10, 5, 1, 1),
    (56, 10, 5, 1, 1),
    (61, 10, 5, 10, 1),
    (75, 10, 5, 1, 1),
    (82, 10, 5, 11, 1),
    (0, 11, 4, 15, 2),
    (38, 11, 4, 10, 2),
    (50, 11, 4, 1, 2),
    (53, 11, 4, 1, 2),
    (56, 11, 4, 1, 2),
    (61, 11, 4, 10, 2),
    (75, 11, 4, 1, 2),
    (82, 11, 4, 11, 2),
    (67, 9, 2, 1, 1),
    (68, 8, 2, 1, 2),
    (69, 7, 2, 1, 3),
    (70, 6, 2, 1, 4),
    (75, 6, 2, 1, 4),
    (42, 8, 11, 1, 1),
    (111, 1, 12, 1, 1),
    (4, 8, 14, 1, 1),
    (9, 8, 14, 1, 1),
    (83, 8, 14, 1, 1),
    (89, 8, 14, 1, 1),
)

WORLD2_COINS: Tuple[Coin, ...] = (
    (25, 2),
    (26, 2),
    (27, 2),
    (32, 4),
    (32, 5),
    (32, 6),
    (32, 7),
    (32, 8),
    (50, 6),
    (53, 6),
    (56, 6),
)

WORLD2_ENEMIES: Tuple[Enemy, ...] = (
    (18, 8, 1),
    (24, 5, 1),
    (28, 5, -1),
    (44, 9, 1),
    (97, 6, 1),
    (99, 4, 1),
)

WORLD3_ENEMIES: Tuple[Enemy, ...] = (
    (24, 4, -1),
    (34, 6, 1),
    (37, 3, -1),
    (43, 5, -1),
    (63, 7, 1),
    (67, 8, -1),
    (86, 5, 1),
)


def generate_world3_tiles(heights: Sequence[int]) -> Tuple[Tile, ...]:
    """Build the seven live pipe columns plus the cloud runway and goal.

    ``heights[i]`` is ``random(1, 7)`` for i in 0..9. Only i=0..6 are used
    (columns 7–9 are commented out in scene.cpp).
    """
    tiles: List[Tile] = []
    for i in range(7):
        h = heights[i]
        x = i * 10 + 10
        tiles.append((x, h - 2, 8, 1, 1))
        tiles.append((x, 0, 7, 1, max(0, h - 2)))
        tiles.append((x, 4 + h, 10, 1, 1))
        tiles.append((x, h + 6, 7, 1, max(0, 6 - h)))
    tiles.append((80, 6, 2, 25, 1))
    tiles.append((101, 4, 12, 1, 1))
    return tuple(tiles)


def generate_world3_coins(heights: Sequence[int]) -> Tuple[Coin, ...]:
    """Seven coins at x = 5,15,...,65. ``heights[i]`` is ``random(3, 7)``."""
    return tuple((5 + 10 * i, heights[i]) for i in range(7))


def world3_from_rng(rand_u32: Callable[[], int]) -> Tuple[Tuple[Tile, ...], Tuple[Coin, ...]]:
    pipe_h = [_c_random(rand_u32, 1, 7) for _ in range(10)]
    coin_h = [_c_random(rand_u32, 3, 7) for _ in range(10)]
    return generate_world3_tiles(pipe_h), generate_world3_coins(coin_h)


WORLD_SPECS = {
    1: WorldSpec(
        number=1,
        name="grassland",
        ending_tiles=94,
        tiles=WORLD1_TILES,
        coins=WORLD1_COINS,
        enemies=WORLD1_ENEMIES,
        food_px=((14 * WIDTH, 5 * HEIGHT),),
        goal=(101, 7),
    ),
    2: WorldSpec(
        number=2,
        name="sky islands",
        ending_tiles=104,
        tiles=WORLD2_TILES,
        coins=WORLD2_COINS,
        enemies=WORLD2_ENEMIES,
        food_px=((39 * WIDTH - WIDTH // 3, 3 * HEIGHT + HEIGHT // 5),),
        goal=(111, 1),
    ),
    3: WorldSpec(
        number=3,
        name="pipe gauntlet",
        ending_tiles=94,
        tiles=(),
        coins=(),
        enemies=WORLD3_ENEMIES,
        food_px=((10, 10),),
        goal=(101, 4),
        procedural=True,
    ),
}


def tiles_for(world: int, heights: Sequence[int] | None = None) -> Tuple[Tile, ...]:
    spec = WORLD_SPECS[world]
    if not spec.procedural:
        return spec.tiles
    if heights is None:
        heights = (3, 4, 2, 5, 1, 6, 3, 4, 2, 5)
    return generate_world3_tiles(heights)


def coins_for(world: int, heights: Sequence[int] | None = None) -> Tuple[Coin, ...]:
    spec = WORLD_SPECS[world]
    if not spec.procedural:
        return spec.coins
    if heights is None:
        heights = (3, 4, 5, 6, 3, 4, 5)
    return generate_world3_coins(heights)


def enemies_for(world: int) -> Tuple[Enemy, ...]:
    return WORLD_SPECS[world].enemies


def food_for(world: int) -> Tuple[Food, ...]:
    return WORLD_SPECS[world].food_px


def loaded_tile_count(world: int, heights: Sequence[int] | None = None) -> int:
    """How many tiles the C++ copy loop actually keeps.

    Worlds 1–2 stop at ``MAP_NUMBER`` (30). World 3 copies ``sizeof+1``
    records (overrun); we report the authored count instead.
    """
    from .constants import MAP_NUMBER

    tiles = tiles_for(world, heights)
    if world in (1, 2):
        return min(len(tiles), MAP_NUMBER)
    return len(tiles)
