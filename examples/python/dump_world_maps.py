#!/usr/bin/env python3
"""Print the authored MaoLiAo tile tables as ASCII and CSV-ish rows.

The 30-slot cap in Scene::createMap is applied the same way as
maoliao::loaded_map: keep records while id is in 1..14 and count < 30.
"""

from __future__ import annotations

from typing import Iterable

WIDTH = 32
MAP_NUMBER = 30
NAMES = {
    1: "grass",
    2: "cloud",
    3: "dirt",
    4: "snow-u",
    5: "snow",
    6: "pipe",
    7: "shaft",
    8: "mouth-d",
    10: "mouth-u",
    11: "tuft",
    12: "sign",
    13: "water",
    14: "tree",
}

# (x, y, id, x_amount, y_amount) — copied from scene.cpp
WORLD1 = [
    (0, 9, 1, 15, 1), (18, 8, 1, 9, 1), (27, 9, 5, 37, 1),
    (67, 9, 6, 4, 1), (74, 9, 5, 1, 1), (78, 9, 1, 28, 1),
    (0, 10, 3, 15, 2), (18, 9, 3, 9, 3), (27, 10, 4, 37, 2),
    (67, 10, 3, 4, 2), (74, 10, 4, 1, 2), (78, 10, 3, 28, 2),
    (10, 6, 2, 4, 1), (40, 4, 2, 3, 1), (62, 6, 2, 1, 1),
    (65, 4, 2, 5, 1), (36, 7, 10, 1, 1), (45, 7, 10, 1, 1),
    (80, 7, 10, 1, 1), (90, 7, 10, 1, 1), (19, 6, 11, 1, 1),
    (23, 6, 11, 1, 1), (32, 7, 11, 1, 1), (49, 7, 11, 1, 1),
    (53, 7, 11, 1, 1), (57, 7, 11, 1, 1), (93, 7, 11, 1, 1),
    (101, 7, 12, 1, 1), (15, 10, 13, 1, 1), (64, 10, 13, 1, 1),
    (71, 10, 13, 1, 1), (75, 10, 13, 1, 1),
]

WORLD2 = [
    (18, 9, 2, 4, 1), (23, 6, 2, 6, 1), (25, 3, 2, 3, 1),
    (31, 10, 2, 2, 1), (39, 7, 2, 1, 1), (39, 4, 2, 1, 1),
    (96, 7, 2, 2, 1), (99, 5, 2, 2, 1), (102, 3, 2, 12, 1),
    (0, 10, 5, 15, 1), (38, 10, 5, 10, 1), (50, 10, 5, 1, 1),
    (53, 10, 5, 1, 1), (56, 10, 5, 1, 1), (61, 10, 5, 10, 1),
    (75, 10, 5, 1, 1), (82, 10, 5, 11, 1), (0, 11, 4, 15, 2),
    (38, 11, 4, 10, 2), (50, 11, 4, 1, 2), (53, 11, 4, 1, 2),
    (56, 11, 4, 1, 2), (61, 11, 4, 10, 2), (75, 11, 4, 1, 2),
    (82, 11, 4, 11, 2), (67, 9, 2, 1, 1), (68, 8, 2, 1, 2),
    (69, 7, 2, 1, 3), (70, 6, 2, 1, 4), (75, 6, 2, 1, 4),
    (42, 8, 11, 1, 1), (111, 1, 12, 1, 1), (4, 8, 14, 1, 1),
    (9, 8, 14, 1, 1), (83, 8, 14, 1, 1), (89, 8, 14, 1, 1),
]


def loaded(rows: list[tuple[int, int, int, int, int]]) -> list[tuple[int, int, int, int, int]]:
    live: list[tuple[int, int, int, int, int]] = []
    for rec in rows:
        if len(live) >= MAP_NUMBER:
            break
        if rec[2] <= 0 or rec[2] >= 15:
            break
        live.append(rec)
    return live


def paint(rows: Iterable[tuple[int, int, int, int, int]], width_tiles: int = 112, height_tiles: int = 13) -> str:
    grid = [["." for _ in range(width_tiles)] for _ in range(height_tiles)]
    glyphs = {
        1: "=",
        2: "~",
        3: "#",
        4: "#",
        5: "=",
        6: "=",
        7: "|",
        8: "v",
        10: "^",
        11: ",",
        12: "F",
        13: "w",
        14: "T",
    }
    for x, y, tid, xa, ya in rows:
        cell_w = 2 if tid in (8, 10) else 1
        cell_h = 2 if tid in (8, 10) else 1
        ch = glyphs.get(tid, "?")
        for dy in range(ya * cell_h):
            for dx in range(xa * cell_w):
                xx, yy = x + dx, y + dy
                if 0 <= yy < height_tiles and 0 <= xx < width_tiles:
                    grid[yy][xx] = ch
    return "\n".join("".join(row) for row in grid)


def report(name: str, rows: list[tuple[int, int, int, int, int]]) -> None:
    live = loaded(rows)
    print(f"## {name} authored={len(rows)} loaded={len(live)} dropped={len(rows) - len(live)}")
    print(paint(live))
    print()


def main() -> int:
    report("world 1", WORLD1)
    report("world 2", WORLD2)
    if len(WORLD1) != 32 or len(loaded(WORLD1)) != 30:
        raise SystemExit("world 1 capacity mismatch")
    if len(WORLD2) != 36 or len(loaded(WORLD2)) != 30:
        raise SystemExit("world 2 capacity mismatch")
    print("dump_world_maps.py: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
