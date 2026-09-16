"""World 3 pipe columns. Matches Scene::createMap's random(1,7) / i*10+10.

The C++ CRT `rand()` is not reproduced; callers pass a sequence of heights
(or use the deterministic lab seed) so tests stay stable.
"""

from __future__ import annotations

from dataclasses import dataclass

from .constants import MAP_NUMBER


def crt_random(value: int, a: int, b: int) -> int:
    """define.h: `#define random(a,b) (rand()%(b-a)+a)`."""
    return value % (b - a) + a


@dataclass(frozen=True)
class PipeColumn:
    x: int
    height: int  # 1..6 from random(1, 7)

    def tiles(self) -> list[dict]:
        h = self.height
        x = self.x
        return [
            {"x": x, "y": h - 2, "id": 8, "xAmount": 1, "yAmount": 1},
            {"x": x, "y": 0, "id": 7, "xAmount": 1, "yAmount": max(0, h - 2)},
            {"x": x, "y": 4 + h, "id": 10, "xAmount": 1, "yAmount": 1},
            {"x": x, "y": h + 6, "id": 7, "xAmount": 1, "yAmount": max(0, 6 - h)},
        ]


def column_x(index: int) -> int:
    return index * 10 + 10


def pipes_from_heights(heights: list[int], count: int = 7) -> list[dict]:
    """World 3 ships 7 columns; 8–10 are commented out in scene.cpp."""
    tiles: list[dict] = []
    for i, height in enumerate(heights[:count]):
        tiles.extend(PipeColumn(x=column_x(i), height=height).tiles())
    tiles.append({"x": 80, "y": 6, "id": 2, "xAmount": 25, "yAmount": 1})
    tiles.append({"x": 101, "y": 4, "id": 12, "xAmount": 1, "yAmount": 1})
    if len(tiles) > MAP_NUMBER:
        raise ValueError(f"{len(tiles)} tiles exceed MAP_NUMBER={MAP_NUMBER}")
    return tiles


def coins_from_heights(heights: list[int]) -> list[tuple[int, int]]:
    """createCoin world 3: seven cells at x=5,15,...,65."""
    return [(5 + 10 * i, h) for i, h in enumerate(heights[:7])]


# Default lab roll: six distinct gap sizes, then a repeat. Not the CRT sequence.
LAB_PIPE_HEIGHTS = [1, 2, 3, 4, 5, 6, 3]
LAB_COIN_HEIGHTS = [3, 4, 5, 6, 3, 4, 5]
