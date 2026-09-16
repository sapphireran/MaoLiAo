"""Load authored worlds and apply the MAP_NUMBER=30 cap from scene.cpp."""

from __future__ import annotations

import json
from pathlib import Path

from .constants import HEIGHT, MAP_NUMBER, WIDTH
from .tiles import annotate
from .world3 import LAB_COIN_HEIGHTS, LAB_PIPE_HEIGHTS, coins_from_heights, pipes_from_heights

DATA_DIR = Path(__file__).resolve().parent.parent / "data"


def _load_json(name: str):
    return json.loads((DATA_DIR / name).read_text(encoding="utf-8"))


def cap_map(tiles: list[dict]) -> list[dict]:
    """createMap copies while i < MAP_NUMBER. Extra authored rows vanish."""
    return list(tiles[:MAP_NUMBER])


def world1() -> dict:
    tiles = [annotate(t, 1) for t in cap_map(_load_json("world1_map.json"))]
    actors = _load_json("world1_actors.json")
    return {"world": 1, "tiles": tiles, "authored_tile_count": 32, **actors}


def world2() -> dict:
    authored = _load_json("world2_map.json")
    tiles = [annotate(t, 2) for t in cap_map(authored)]
    actors = _load_json("world2_actors.json")
    dropped = authored[MAP_NUMBER:]
    return {
        "world": 2,
        "tiles": tiles,
        "authored_tile_count": len(authored),
        "dropped": dropped,
        **actors,
    }


def world3(pipe_heights: list[int] | None = None, coin_heights: list[int] | None = None) -> dict:
    pipes = pipe_heights or LAB_PIPE_HEIGHTS
    coins_h = coin_heights or LAB_COIN_HEIGHTS
    tiles = [annotate(t, 3) for t in pipes_from_heights(pipes)]
    return {
        "world": 3,
        "tiles": tiles,
        "authored_tile_count": len(tiles),
        "coins": coins_from_heights(coins_h),
        "food_pixels": [[10, 10]],
        "enemies": [
            {"x": 24, "y": 4, "turn": -1},
            {"x": 34, "y": 6, "turn": 1},
            {"x": 37, "y": 3, "turn": -1},
            {"x": 43, "y": 5, "turn": -1},
            {"x": 63, "y": 7, "turn": 1},
            {"x": 67, "y": 8, "turn": -1},
            {"x": 86, "y": 5, "turn": 1},
        ],
        "pipe_heights": pipes,
    }


def load_world(n: int) -> dict:
    if n == 1:
        return world1()
    if n == 2:
        return world2()
    if n == 3:
        return world3()
    raise ValueError(f"no world {n}")


def food_pixels_world1() -> tuple[int, int]:
    return (14 * WIDTH, 5 * HEIGHT)


def food_pixels_world2() -> tuple[int, int]:
    """Integer division matches the C++ initializer list."""
    return (39 * WIDTH - WIDTH // 3, 3 * HEIGHT + HEIGHT // 5)
