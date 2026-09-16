"""Constants copied from define.h / role.h / scene.h."""

from __future__ import annotations

from pathlib import Path
import json
import math

CATALOG = Path(__file__).resolve().parents[1] / "catalog"
CONST_PATH = CATALOG / "constants.json"


def _load() -> dict:
    return json.loads(CONST_PATH.read_text())


_RAW = _load()


class C:
    XSIZE = 512
    YSIZE = 384
    WIDTH = 32
    HEIGHT = 32
    X0 = 2 * WIDTH
    Y0 = 3 * HEIGHT
    TIME = 0.01
    STEP = 10
    ENEMY_STEP = 1
    K_MAP_BG = 5
    XLEFT = 0
    XRIGHT = WIDTH * 6
    REAL_HEIGHT = 3.5
    UNREAL_HEIGHT = 3 * HEIGHT + 5
    G = 30.0
    V_MAX = 8.0
    A_ROLE = 20.0
    T1 = 0.5
    T2 = 1.2
    T3 = 1.5
    LIFE = 5
    MAP_NUMBER = 30
    COINS_NUMBER = 70
    FOOD_NUMBER = 5
    ENEMY_TOTE = 30
    BULLET_NUMBER = 30
    LEHGTH_INTERVAL_BULLET = 4
    TIME_INTERVAL_BULLET = 0.2
    MAX_DISTANCE = 480
    SCORE_COIN = 10
    SCORE_STOMP = 5
    SCORE_BULLET = 5
    ENDING = {1: 94, 2: 104, 3: 94}


def pixel_scale() -> float:
    return C.UNREAL_HEIGHT / C.REAL_HEIGHT


def friction_u(t: float) -> float:
    """Tile.u = (V_MAX / T) / G as stored in Scene::createMap."""
    return (C.V_MAX / t) / C.G


def launch_vy() -> float:
    return -math.sqrt(2.0 * C.G * C.REAL_HEIGHT)


def assert_matches_catalog() -> None:
    raw = _load()
    screen = raw["screen"]
    motion = raw["motion"]
    assert screen["XSIZE"] == C.XSIZE
    assert screen["YSIZE"] == C.YSIZE
    assert screen["WIDTH"] == C.WIDTH
    assert screen["XRIGHT"] == C.XRIGHT
    assert screen["LIFE"] == C.LIFE
    assert motion["UNREAL_HEIGHT"] == C.UNREAL_HEIGHT
    assert motion["G"] == C.G
    assert abs(motion["pixel_scale"] - pixel_scale()) < 1e-12
    assert raw["ending_tiles"]["1"] == C.ENDING[1]
    assert raw["ending_tiles"]["2"] == C.ENDING[2]
    assert raw["pools"]["MAP_NUMBER"] == C.MAP_NUMBER
