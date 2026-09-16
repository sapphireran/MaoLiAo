#!/usr/bin/env python3
"""Write a simple SVG of an authored world (no BMP assets required)."""

from __future__ import annotations

import argparse
import random
import sys
from pathlib import Path
from typing import Sequence
from xml.sax.saxutils import escape

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from maoliao_sim.constants import HEIGHT, MAP_NUMBER, WIDTH, X0, Y0
from maoliao_sim.worlds import coins_for, enemies_for, food_for, tiles_for, WORLD_SPECS

TILE_FILL = {
    1: "#3d8c40",
    2: "#d9e8f5",
    3: "#8b5a2b",
    4: "#cfd8dc",
    5: "#eceff1",
    6: "#26a69a",
    7: "#2e7d32",
    8: "#1b5e20",
    10: "#1b5e20",
    11: "#81c784",
    12: "#f9a825",
    13: "#4fc3f7",
    14: "#6d4c41",
}


def svg_for(
    world: int,
    pipe_heights: Sequence[int] | None = None,
    coin_heights: Sequence[int] | None = None,
    apply_map_cap: bool = True,
    tile_px: int = 10,
) -> str:
    tiles = tiles_for(world, pipe_heights)
    if apply_map_cap and world in (1, 2):
        tiles = tiles[:MAP_NUMBER]
    coins = coins_for(world, coin_heights)
    enemies = enemies_for(world)
    food = food_for(world)
    spec = WORLD_SPECS[world]

    max_x = max((x + (2 if tid in (8, 10) else 1) * xa) for x, _y, tid, xa, _ya in tiles)
    max_x = max(max_x, spec.goal[0] + 3, 20)
    rows = 16
    w = max_x * tile_px
    h = rows * tile_px

    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {w} {h}" '
        f'width="{w}" height="{h}" font-family="monospace">',
        f'<rect width="{w}" height="{h}" fill="#87ceeb"/>',
        f'<text x="8" y="14" font-size="12" fill="#102027">'
        f"{escape(f'world {world} — {spec.name}')}</text>",
    ]
    for x, y, tid, xa, ya in tiles:
        cell_w = 2 if tid in (8, 10) else 1
        cell_h = 2 if tid in (8, 10) else 1
        fill = TILE_FILL.get(tid, "#9e9e9e")
        parts.append(
            f'<rect x="{x * tile_px}" y="{y * tile_px}" '
            f'width="{xa * cell_w * tile_px}" height="{ya * cell_h * tile_px}" '
            f'fill="{fill}" stroke="#37474f" stroke-width="0.4"/>'
        )
    for x, y in coins:
        cx = (x + 0.5) * tile_px
        cy = (y + 0.5) * tile_px
        parts.append(f'<circle cx="{cx}" cy="{cy}" r="{tile_px * 0.35}" fill="#ffd54f" stroke="#f9a825"/>')
    for x, y, _turn in enemies:
        parts.append(
            f'<rect x="{x * tile_px + 2}" y="{y * tile_px + 2}" '
            f'width="{tile_px - 4}" height="{tile_px - 4}" fill="#e53935" rx="2"/>'
        )
    for fx, fy in food:
        parts.append(
            f'<rect x="{(fx // WIDTH) * tile_px + 1}" y="{(fy // HEIGHT) * tile_px + 1}" '
            f'width="{tile_px - 2}" height="{tile_px - 2}" fill="#8e24aa" rx="3"/>'
        )
    parts.append(
        f'<rect x="{(X0 // WIDTH) * tile_px + 2}" y="{(Y0 // HEIGHT) * tile_px + 2}" '
        f'width="{tile_px - 4}" height="{tile_px - 4}" fill="#1565c0" rx="2"/>'
    )
    parts.append("</svg>")
    return "\n".join(parts) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--world", type=int, default=1, choices=(1, 2, 3))
    parser.add_argument("--seed", type=int, default=2020)
    parser.add_argument("-o", "--output", type=Path, required=True)
    parser.add_argument("--no-cap", action="store_true")
    args = parser.parse_args()

    pipe_h = coin_h = None
    if args.world == 3:
        rng = random.Random(args.seed)
        pipe_h = [rng.randint(1, 6) for _ in range(10)]
        coin_h = [rng.randint(3, 6) for _ in range(10)]

    args.output.write_text(
        svg_for(args.world, pipe_h, coin_h, apply_map_cap=not args.no_cap),
        encoding="utf-8",
    )
    print(f"wrote {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
