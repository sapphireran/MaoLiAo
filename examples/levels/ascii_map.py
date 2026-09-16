#!/usr/bin/env python3
"""Render an authored world as an ASCII tile map."""

from __future__ import annotations

import argparse
import random
import sys
from pathlib import Path
from typing import Dict, List, Sequence, Tuple

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from maoliao_sim.constants import HEIGHT, MAP_NUMBER, WIDTH, X0, Y0
from maoliao_sim.worlds import (
    coins_for,
    enemies_for,
    food_for,
    generate_world3_coins,
    generate_world3_tiles,
    tiles_for,
    WORLD_SPECS,
)

LEGEND = {
    "#": "grass / generic solid (id 1)",
    "=": "surface or underground fill (id 3/4/5)",
    "%": "high-friction strip (id 6)",
    "~": "cloud (id 2) or water scenery (id 13)",
    "|": "pipe shaft (id 7)",
    "v": "downward pipe (id 8)",
    "^": "upward pipe (id 10)",
    '"': "background grass (id 11)",
    "G": "goal sign (id 12)",
    "T": "tree (id 14)",
    "o": "coin",
    "E": "enemy",
    "F": "weapon mushroom (food)",
    "@": "hero spawn (screen, not a tile)",
}

ID_GLYPH = {
    1: "#",
    2: "~",
    3: "=",
    4: ":",
    5: "=",
    6: "%",
    7: "|",
    8: "v",
    10: "^",
    11: '"',
    12: "G",
    13: "~",
    14: "T",
}


def _stamp(grid: List[List[str]], x: int, y: int, ch: str, w: int = 1, h: int = 1) -> None:
    rows = len(grid)
    cols = len(grid[0]) if rows else 0
    for dy in range(h):
        for dx in range(w):
            gx, gy = x + dx, y + dy
            if 0 <= gy < rows and 0 <= gx < cols:
                grid[gy][gx] = ch


def render(
    world: int,
    pipe_heights: Sequence[int] | None = None,
    coin_heights: Sequence[int] | None = None,
    width_tiles: int | None = None,
    height_tiles: int = 16,
    apply_map_cap: bool = True,
) -> Tuple[str, Dict[str, int]]:
    tiles = tiles_for(world, pipe_heights)
    coins = coins_for(world, coin_heights)
    enemies = enemies_for(world)
    food = food_for(world)

    if apply_map_cap and world in (1, 2):
        loaded = tiles[:MAP_NUMBER]
        dropped = tiles[MAP_NUMBER:]
    else:
        loaded = tiles
        dropped = ()

    max_x = 0
    for x, _y, _id, xa, _ya in tiles:
        max_x = max(max_x, x + max(xa, 2))
    for x, _y in coins:
        max_x = max(max_x, x + 1)
    cols = width_tiles or max(max_x + 2, 16)
    grid = [["."] * cols for _ in range(height_tiles)]

    for x, y, tid, xa, ya in loaded:
        glyph = ID_GLYPH.get(tid, "?")
        cell_w = 2 if tid in (8, 10) else 1
        cell_h = 2 if tid in (8, 10) else 1
        _stamp(grid, x, y, glyph, xa * cell_w, ya * cell_h)

    for x, y in coins:
        _stamp(grid, x, y, "o")

    for x, y, _turn in enemies:
        _stamp(grid, x, y, "E")

    for fx, fy in food:
        _stamp(grid, fx // WIDTH, fy // HEIGHT, "F")

    spawn_x, spawn_y = X0 // WIDTH, Y0 // HEIGHT
    if 0 <= spawn_y < height_tiles and 0 <= spawn_x < cols:
        if grid[spawn_y][spawn_x] == ".":
            grid[spawn_y][spawn_x] = "@"

    # Screen-down Y: print y=0 at the top, matching EasyX.
    lines = ["".join(row) for row in grid]
    counts = {
        "tiles_authored": len(tiles),
        "tiles_loaded": len(loaded),
        "tiles_dropped": len(dropped),
        "coins": len(coins),
        "enemies": len(enemies),
        "food": len(food),
        "cols": cols,
        "rows": height_tiles,
    }
    return "\n".join(lines), counts


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--world", type=int, default=1, choices=(1, 2, 3))
    parser.add_argument("--seed", type=int, default=2020, help="world 3 RNG seed (Python random)")
    parser.add_argument("--legend", action="store_true")
    parser.add_argument("--no-cap", action="store_true", help="draw tiles the C++ cap would drop")
    args = parser.parse_args()

    pipe_h = coin_h = None
    if args.world == 3:
        rng = random.Random(args.seed)
        # Display seed only; generation uses Python random, not CRT rand().
        pipe_h = [rng.randint(1, 6) for _ in range(10)]
        coin_h = [rng.randint(3, 6) for _ in range(10)]
        # randint is inclusive; match random(1,7) / random(3,7) spans.
        _ = generate_world3_tiles(pipe_h)
        _ = generate_world3_coins(coin_h)

    text, counts = render(
        args.world,
        pipe_heights=pipe_h,
        coin_heights=coin_h,
        apply_map_cap=not args.no_cap,
    )
    spec = WORLD_SPECS[args.world]
    print(f"world {args.world} ({spec.name})  ending={spec.ending_tiles} tiles  goal={spec.goal}")
    if args.world == 3:
        print(f"seed={args.seed}  pipe_h={list(pipe_h)}  coin_h={list(coin_h)[:7]}")
    print(
        "tiles {tiles_loaded}/{tiles_authored} loaded (dropped {tiles_dropped})  "
        "coins={coins} enemies={enemies} food={food}  grid={cols}x{rows}".format(**counts)
    )
    print()
    print(text)
    if args.legend:
        print()
        print("legend")
        for glyph, meaning in LEGEND.items():
            print(f"  {glyph}  {meaning}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
