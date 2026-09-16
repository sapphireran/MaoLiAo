"""Tile id catalog and friction assignment from Scene::createMap."""

from __future__ import annotations

from .constants import CLOUD_ID, GOAL_ID, U_DEFAULT, U_HIGH, U_NORMAL

TILES = {
    1: {"name": "grass-top", "solid": True, "sheet": "map.bmp", "draw": (32, 32)},
    2: {"name": "cloud-or-snow-platform", "solid": True, "sheet": "map.bmp", "draw": (32, 32)},
    3: {"name": "dirt-under-grass", "solid": True, "sheet": "map.bmp", "draw": (32, 32)},
    4: {"name": "snow-underground", "solid": True, "sheet": "map.bmp", "draw": (32, 32)},
    5: {"name": "snow-top", "solid": True, "sheet": "map.bmp", "draw": (32, 32)},
    6: {"name": "pipe-body-or-ice-top", "solid": True, "sheet": "map.bmp", "draw": (32, 32)},
    7: {"name": "pipe-shaft", "solid": True, "sheet": "map.bmp", "draw": (64, 32)},
    8: {"name": "pipe-mouth-down", "solid": True, "sheet": "map.bmp", "draw": (64, 64)},
    9: {"name": "pipe-mouth-unused", "solid": True, "sheet": "map.bmp", "draw": (32, 32)},
    10: {"name": "pipe-mouth-up", "solid": True, "sheet": "map.bmp", "draw": (64, 64)},
    11: {"name": "bg-grass", "solid": False, "sheet": "scenery.bmp", "draw": (96, 64)},
    12: {"name": "goal-sign", "solid": False, "sheet": "scenery.bmp", "draw": (96, 64)},
    13: {"name": "water", "solid": False, "sheet": "scenery.bmp", "draw": (96, 64)},
    14: {"name": "tree", "solid": False, "sheet": "scenery.bmp", "draw": (96, 64)},
}


def friction_for(tile_id: int, world: int) -> float:
    """Match the switch in Scene::createMap for each world."""
    if world == 3:
        if tile_id in (1, 3, 4, 5, 6):
            return U_HIGH
        if tile_id == CLOUD_ID:
            return U_NORMAL
        return U_DEFAULT
    if tile_id in (1, 3, 4, 5, CLOUD_ID):
        return U_NORMAL
    if tile_id == 6:
        return U_HIGH
    return U_DEFAULT


def annotate(tile: dict, world: int) -> dict:
    info = TILES.get(int(tile["id"]), {"name": "unknown", "solid": False})
    return {
        **tile,
        "name": info["name"],
        "solid": info.get("solid", False),
        "u": friction_for(int(tile["id"]), world),
        "is_goal": int(tile["id"]) == GOAL_ID,
    }
