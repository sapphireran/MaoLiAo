"""World tables mirrored from scene.cpp / role.cpp, plus the world-3 generator."""

from __future__ import annotations

import json
import random
from pathlib import Path
from typing import Any

from .constants import DATA_DIR, HEIGHT, MAP_NUMBER, WIDTH, friction_u

WORLDS_PATH = DATA_DIR / "worlds.json"
TILES_PATH = DATA_DIR / "tiles.json"


def load_worlds() -> dict[str, Any]:
    with WORLDS_PATH.open(encoding="utf-8") as fh:
        return json.load(fh)


def load_tiles() -> dict[str, Any]:
    with TILES_PATH.open(encoding="utf-8") as fh:
        return json.load(fh)


def c_random(rng: random.Random, a: int, b: int) -> int:
    """define.h: `#define random(a,b) (rand()%(b-a)+a)` → [a, b)."""
    if b <= a:
        raise ValueError(f"random({a}, {b}) is empty")
    return rng.randrange(a, b)


def friction_bucket(tile_id: int, world: int) -> str:
    if world == 3:
        if tile_id in (1, 3, 4, 5, 6):
            return "T1"
        if tile_id == 2:
            return "T2"
        return "T3"
    if tile_id in (1, 3, 4, 5):
        return "T2"
    if tile_id == 6:
        return "T1"
    if tile_id == 2:
        return "T2"
    return "T3"


def with_friction(records: list[dict], world: int) -> list[dict]:
    out = []
    for rec in records:
        item = dict(rec)
        bucket = friction_bucket(item["id"], world)
        item["friction"] = bucket
        item["u"] = friction_u(bucket)
        out.append(item)
    return out


def apply_map_cap(records: list[dict], cap: int = MAP_NUMBER) -> list[dict]:
    """Scene copies while i < MAP_NUMBER and 0 < id < 15."""
    loaded: list[dict] = []
    for rec in records:
        if not (0 < rec["id"] < 15):
            break
        if len(loaded) >= cap:
            break
        loaded.append(rec)
    return loaded


def generate_world3_maps(seed: int | None = 1) -> list[dict]:
    """Emit the 7 pipe columns + runway + goal from scene.cpp world == 3."""
    rng = random.Random(seed)
    heights = [c_random(rng, 1, 7) for _ in range(10)]
    xs = [i * 10 + 10 for i in range(10)]
    records: list[dict] = []
    for i in range(7):
        h = heights[i]
        x = xs[i]
        records.extend(
            [
                {"x": x, "y": h - 2, "id": 8, "xAmount": 1, "yAmount": 1},
                {"x": x, "y": 0, "id": 7, "xAmount": 1, "yAmount": max(0, h - 2)},
                {"x": x, "y": 4 + h, "id": 10, "xAmount": 1, "yAmount": 1},
                {"x": x, "y": h + 6, "id": 7, "xAmount": 1, "yAmount": max(0, 6 - h)},
            ]
        )
    records.append({"x": 80, "y": 6, "id": 2, "xAmount": 25, "yAmount": 1})
    records.append({"x": 101, "y": 4, "id": 12, "xAmount": 1, "yAmount": 1})
    return records


def generate_world3_coins(seed: int | None = 1) -> list[list[int]]:
    rng = random.Random(seed)
    # Independent of map heights in the C++: createCoin uses its own loop.
    heights = [c_random(rng, 3, 7) for _ in range(10)]
    xs = [5, 15, 25, 35, 45, 55, 65]
    return [[xs[i], heights[i]] for i in range(7)]


def world_payload(world: int, seed: int | None = 1) -> dict[str, Any]:
    data = load_worlds()["worlds"][str(world)]
    if world == 3:
        maps = generate_world3_maps(seed)
        coins = generate_world3_coins(seed)
    else:
        maps = list(data["maps"])
        coins = [list(pair) for pair in data["coins"]]
    loaded = with_friction(apply_map_cap(maps), world)
    truncated = maps[len(loaded) :]
    return {
        "world": world,
        "name": data["name"],
        "ending_tiles": data["ending_tiles"],
        "maps_authored": maps,
        "maps_loaded": loaded,
        "maps_truncated": truncated,
        "coins": coins,
        "food_pixels": [list(p) for p in data["food_pixels"]],
        "enemies": list(data["enemies"]),
        "seed": seed if world == 3 else None,
    }


def colliding_extent_tiles(records: list[dict]) -> int:
    """Right-most colliding pixel column, in tiles (ceil)."""
    max_px = 0
    for rec in records:
        if not (1 <= rec["id"] <= 10):
            continue
        factor = 2 if rec["id"] in (8, 10) else 1
        right = (rec["x"] + rec["xAmount"] * factor) * WIDTH
        max_px = max(max_px, right)
    return (max_px + WIDTH - 1) // WIDTH if max_px else 0


def food_tile(pixels: list[int]) -> tuple[int, int]:
    return pixels[0] // WIDTH, pixels[1] // HEIGHT
