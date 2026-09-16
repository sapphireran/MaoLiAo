#!/usr/bin/env python3
"""Write ASCII and SVG plans for worlds 1–3 into examples/out/."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from kit.ascii import render_ascii  # noqa: E402
from kit.raster import render_jump_png, render_png  # noqa: E402
from kit.svg import render_jump_svg, render_svg  # noqa: E402
from kit.worlds import load_world  # noqa: E402


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("-o", "--out", type=Path, default=ROOT / "out")
    p.add_argument("--seed", type=int, default=2020)
    p.add_argument("--no-cap", action="store_true", help="draw authored tiles past MAP_NUMBER")
    args = p.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    for i in (1, 2, 3):
        world = load_world(i, cap=not args.no_cap, seed=args.seed)
        ascii_path = args.out / f"world{i}.txt"
        svg_path = args.out / f"world{i}.svg"
        ascii_path.write_text(render_ascii(world), encoding="utf-8")
        svg_path.write_text(render_svg(world, cap=not args.no_cap), encoding="utf-8")
        png_path = args.out / f"world{i}.png"
        render_png(world, png_path, cap=not args.no_cap)
        print(f"  {png_path}")
        extra = f" dropped={len(world.dropped)}" if world.dropped else ""
        print(f"world {i}: {len(world.runtime_tiles)} tiles, "
              f"{len(world.coins)} coins, {len(world.enemies)} enemies{extra}")
        print(f"  {ascii_path}")
        print(f"  {svg_path}")
    authored = load_world(2, cap=False)
    render_png(authored, args.out / "world2_authored.png", cap=False)
    (args.out / "world2_authored.svg").write_text(render_svg(authored, cap=False), encoding="utf-8")
    print(f"world 2 authored: {args.out / 'world2_authored.png'}")
    jump_path = args.out / "jump_arc.svg"
    jump_path.write_text(render_jump_svg(), encoding="utf-8")
    jump_png = args.out / "jump_arc.png"
    render_jump_png(jump_png)
    print(f"jump: {jump_path}")
    print(f"jump: {jump_png}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
