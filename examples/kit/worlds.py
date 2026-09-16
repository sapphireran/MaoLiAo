"""Authored maps plus the world-3 pipe recipe."""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path
import json

from .collision import Tile
from .constants import C, CATALOG
from .msvc_rand import MsvcRand

WORLD_DIR = CATALOG / "worlds"


@dataclass
class Enemy:
    x: float
    y: float
    turn: int


@dataclass
class World:
    index: int
    title: str
    tiles: list[Tile]
    coins: list[tuple[int, int]]
    food_pixels: list[tuple[float, float]]
    enemies: list[Enemy]
    ending_tiles: int
    authored_tile_count: int
    max_score: int
    dropped: list[dict] = field(default_factory=list)

    @property
    def runtime_tiles(self) -> list[Tile]:
        return self.tiles[: C.MAP_NUMBER]

    @property
    def ending_px(self) -> int:
        return self.ending_tiles * C.WIDTH


def _tiles_from(records) -> list[Tile]:
    return [Tile(r["x"], r["y"], r["id"], r["xAmount"], r["yAmount"]) for r in records]


def load_world(index: int, *, cap: bool = True, seed: int = 2020) -> World:
    if index == 3:
        return generate_world3(seed=seed, cap=cap)
    raw = json.loads((WORLD_DIR / f"world{index}.json").read_text())
    tiles = _tiles_from(raw["tiles"])
    if cap:
        tiles = tiles[: C.MAP_NUMBER]
    enemies = [
        Enemy(e["x"] * C.WIDTH, e["y"] * C.HEIGHT, e["turn"]) for e in raw["enemies"]
    ]
    food = [tuple(p) for p in raw["food_pixels"]]
    coins = [tuple(c) for c in raw["coins"]]
    return World(
        index=index,
        title=raw["title"],
        tiles=tiles,
        coins=coins,
        food_pixels=food,
        enemies=enemies,
        ending_tiles=raw["ending_tiles"],
        authored_tile_count=raw["authored_tile_count"],
        max_score=raw["max_score"],
        dropped=list(raw.get("dropped") or []),
    )


def generate_world3(seed: int = 2020, *, cap: bool = True) -> World:
    raw = json.loads((WORLD_DIR / "world3.json").read_text())
    rng = MsvcRand(seed)
    coin_h = [rng.random_ab(3, 7) for _ in range(10)]
    pipe_h = [rng.random_ab(1, 7) for _ in range(10)]
    coins = [(5 + 10 * i, coin_h[i]) for i in range(7)]
    tiles: list[Tile] = []
    for i in range(7):
        x = 10 + 10 * i
        h = pipe_h[i]
        tiles.append(Tile(x, h - 2, 8, 1, 1))
        tiles.append(Tile(x, 0, 7, 1, max(0, h - 2)))
        tiles.append(Tile(x, 4 + h, 10, 1, 1))
        tiles.append(Tile(x, h + 6, 7, 1, max(0, 6 - h)))
    tiles.append(Tile(80, 6, 2, 25, 1))
    tiles.append(Tile(101, 4, 12, 1, 1))
    authored = len(tiles)
    if cap:
        tiles = tiles[: C.MAP_NUMBER]
    enemies = [
        Enemy(e["x"] * C.WIDTH, e["y"] * C.HEIGHT, e["turn"]) for e in raw["enemies"]
    ]
    return World(
        index=3,
        title=raw["title"],
        tiles=tiles,
        coins=coins,
        food_pixels=[tuple(raw["food_pixels"][0])],
        enemies=enemies,
        ending_tiles=raw["ending_tiles"],
        authored_tile_count=authored,
        max_score=raw["max_score"],
        dropped=[],
    )


def catalog_seed_heights(seed: int) -> tuple[list[int], list[int]]:
    raw = json.loads((WORLD_DIR / "world3.json").read_text())
    block = raw["seeds"][str(seed)]
    return list(block["coin_heights_10"]), list(block["pipe_heights_10"])


def world_json_path(index: int) -> Path:
    return WORLD_DIR / f"world{index}.json"
