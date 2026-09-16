"""Hard-coded world tables from Scene::createMap / createCoin / createFood
and Role::createEnemy.

The C++ copy loops sometimes run one past the local array and sometimes
stop at MAP_NUMBER (30). Helpers here expose both the authored rows and
the rows that actually fit in map[30].
"""

from __future__ import annotations

import random
from dataclasses import dataclass

from .collision import Tile
from .constants import HEIGHT, MAP_NUMBER, WIDTH
from .inertia import friction_u

# (x, y, id, xAmount, yAmount) as written in scene.cpp
WORLD1_MAP_AUTHORED: tuple[tuple[int, int, int, int, int], ...] = (
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

WORLD2_MAP_AUTHORED: tuple[tuple[int, int, int, int, int], ...] = (
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

WORLD1_COINS: tuple[tuple[int, int], ...] = (
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

WORLD2_COINS: tuple[tuple[int, int], ...] = (
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

# Pixel positions, not tiles.
WORLD1_FOOD: tuple[tuple[float, float], ...] = ((14 * WIDTH, 5 * HEIGHT),)
WORLD2_FOOD: tuple[tuple[float, float], ...] = (
    (39 * WIDTH - WIDTH / 3, 3 * HEIGHT + HEIGHT / 5),
)
WORLD3_FOOD: tuple[tuple[float, float], ...] = ((10.0, 10.0),)

# (tile_x, tile_y, turn) — turn +1 right, -1 left. Stored in pixels at spawn.
WORLD1_ENEMIES: tuple[tuple[int, int, int], ...] = (
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

WORLD2_ENEMIES: tuple[tuple[int, int, int], ...] = (
    (18, 8, 1),
    (24, 5, 1),
    (28, 5, -1),
    (44, 9, 1),
    (97, 6, 1),
    (99, 4, 1),
)

WORLD3_ENEMIES: tuple[tuple[int, int, int], ...] = (
    (24, 4, -1),
    (34, 6, 1),
    (37, 3, -1),
    (43, 5, -1),
    (63, 7, 1),
    (67, 8, -1),
    (86, 5, 1),
)

ENDING_DISTANCE = {1: 94 * WIDTH, 2: 104 * WIDTH, 3: 94 * WIDTH}

TILE_CHARS = {
    1: "#",
    2: "=",
    3: ":",
    4: "%",
    5: "^",
    6: "*",
    7: "|",
    8: "v",
    9: "u",
    10: "n",
    11: ",",
    12: "W",
    13: "~",
    14: "T",
}


def _to_tiles(rows: tuple[tuple[int, int, int, int, int], ...], world: int) -> list[Tile]:
    return [
        Tile(x, y, tid, xa, ya, friction_u(tid, world))
        for x, y, tid, xa, ya in rows
    ]


def world3_pipe_field(seed: int = 1, columns: int = 7) -> list[tuple[int, int, int, int, int]]:
    """Reproduce Scene::createMap world==3 with an explicit RNG seed.

    The C++ uses unseeded rand(); tests pin the field so dumps stay stable.
    """
    rng = random.Random(seed)
    height = [rng.randrange(1, 7) for _ in range(10)]
    xs = [i * 10 + 10 for i in range(10)]
    rows: list[tuple[int, int, int, int, int]] = []
    for i in range(columns):
        h = height[i]
        x = xs[i]
        rows.append((x, h - 2, 8, 1, 1))
        rows.append((x, 0, 7, 1, max(0, h - 2)))
        rows.append((x, 4 + h, 10, 1, 1))
        rows.append((x, h + 6, 7, 1, max(0, 6 - h)))
    rows.append((80, 6, 2, 25, 1))
    rows.append((101, 4, 12, 1, 1))
    return rows


def world3_coins(seed: int = 1) -> list[tuple[int, int]]:
    rng = random.Random(seed)
    height = [rng.randrange(3, 7) for _ in range(10)]
    xs = [5, 15, 25, 35, 45, 55, 65]
    return [(xs[i], height[i]) for i in range(7)]


def authored_map(world: int, seed: int = 1) -> list[Tile]:
    if world == 1:
        return _to_tiles(WORLD1_MAP_AUTHORED, 1)
    if world == 2:
        return _to_tiles(WORLD2_MAP_AUTHORED, 2)
    if world == 3:
        return _to_tiles(tuple(world3_pipe_field(seed)), 3)
    raise ValueError(f"unknown world {world}")


def loaded_map(world: int, seed: int = 1) -> list[Tile]:
    """Tiles that fit in map[MAP_NUMBER]."""
    return authored_map(world, seed)[:MAP_NUMBER]


def dropped_map(world: int, seed: int = 1) -> list[Tile]:
    return authored_map(world, seed)[MAP_NUMBER:]


def coins_for(world: int, seed: int = 1) -> list[tuple[int, int]]:
    if world == 1:
        return list(WORLD1_COINS)
    if world == 2:
        return list(WORLD2_COINS)
    if world == 3:
        return world3_coins(seed)
    raise ValueError(f"unknown world {world}")


def food_for(world: int) -> list[tuple[float, float]]:
    if world == 1:
        return list(WORLD1_FOOD)
    if world == 2:
        return list(WORLD2_FOOD)
    if world == 3:
        return list(WORLD3_FOOD)
    raise ValueError(f"unknown world {world}")


def enemies_for(world: int) -> list[tuple[float, float, int]]:
    table = {1: WORLD1_ENEMIES, 2: WORLD2_ENEMIES, 3: WORLD3_ENEMIES}[world]
    return [(tx * WIDTH, ty * HEIGHT, turn) for tx, ty, turn in table]


def is_ending(world: int, distance: int) -> bool:
    return distance > ENDING_DISTANCE[world]


def tile_span(tile: Tile) -> tuple[int, int]:
    if tile.id in (8, 10):
        return tile.x_amount * 2, tile.y_amount * 2
    return tile.x_amount, tile.y_amount


@dataclass
class WorldDump:
    world: int
    width: int
    height: int
    grid: list[list[str]]
    legend: dict[str, str]


def render_ascii(world: int, seed: int = 1, use_loaded: bool = True) -> WorldDump:
    tiles = loaded_map(world, seed) if use_loaded else authored_map(world, seed)
    coins = coins_for(world, seed)
    foods = food_for(world)
    enemies = enemies_for(world)

    max_x = 0
    max_y = 0
    for tile in tiles:
        w, h = tile_span(tile)
        max_x = max(max_x, tile.x + w)
        max_y = max(max_y, tile.y + h)
    for cx, cy in coins:
        max_x = max(max_x, cx + 1)
        max_y = max(max_y, cy + 1)
    for ex, ey, _ in enemies:
        max_x = max(max_x, int(ex / WIDTH) + 1)
        max_y = max(max_y, int(ey / HEIGHT) + 1)

    grid = [[" " for _ in range(max_x)] for _ in range(max_y)]
    for tile in tiles:
        ch = TILE_CHARS.get(tile.id, "?")
        w, h = tile_span(tile)
        for j in range(w):
            for k in range(h):
                gx, gy = tile.x + j, tile.y + k
                if 0 <= gy < max_y and 0 <= gx < max_x:
                    grid[gy][gx] = ch
    for cx, cy in coins:
        if 0 <= cy < max_y and 0 <= cx < max_x:
            grid[cy][cx] = "o"
    for fx, fy in foods:
        gx, gy = int(fx / WIDTH), int(fy / HEIGHT)
        if 0 <= gy < max_y and 0 <= gx < max_x:
            grid[gy][gx] = "F"
    for ex, ey, turn in enemies:
        gx, gy = int(ex / WIDTH), int(ey / HEIGHT)
        if 0 <= gy < max_y and 0 <= gx < max_x:
            grid[gy][gx] = ">" if turn > 0 else "<"

    legend = {
        "#": "grass top (1)",
        "=": "cloud (2)",
        ":": "dirt (3)",
        "%": "snow underground (4)",
        "^": "snow top (5)",
        "*": "high-friction (6)",
        "|": "pipe shaft (7)",
        "v": "inverted pipe (8)",
        "n": "pipe mouth (10)",
        ",": "grass tuft (11)",
        "W": "victory sign (12)",
        "~": "water (13)",
        "T": "tree (14)",
        "o": "coin",
        "F": "weapon / star",
        ">": "enemy facing right",
        "<": "enemy facing left",
    }
    return WorldDump(world, max_x, max_y, grid, legend)


def format_ascii(dump: WorldDump) -> str:
    lines = [
        f"world {dump.world}  {dump.width} x {dump.height} tiles "
        f"(y grows downward, as in the EasyX framebuffer)",
        "",
    ]
    for y, row in enumerate(dump.grid):
        lines.append(f"{y:02d}|" + "".join(row))
    lines.append("  +" + "-" * dump.width)
    lines.append("")
    lines.append("legend: " + ", ".join(f"{k}={v}" for k, v in dump.legend.items() if any(
        k in row for row in dump.grid
    )))
    return "\n".join(lines)
