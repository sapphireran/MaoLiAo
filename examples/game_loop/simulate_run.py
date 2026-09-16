#!/usr/bin/env python3
"""Drive a keyboard script through world 1 and print a compact log."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao_lib.commands import pack, unpack  # noqa: E402
from maoliao_lib.hero import GameState, tick  # noqa: E402


def parse_script(text: str) -> list[int]:
    """Lines of `frames names` such as `40 right` or `8 right up`."""
    keys: list[int] = []
    for raw in text.splitlines():
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        frames = int(parts[0])
        names = parts[1:]
        mask = pack(*names) if names else 0
        keys.extend([mask] * frames)
    return keys


DEFAULT_SCRIPT = """
80            # fall onto the grass
12 right up   # hop toward the first cloud
40 right
8 right up
50 right
"""


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--world", type=int, default=1, choices=(1, 2, 3))
    parser.add_argument("--script", type=Path, default=None)
    parser.add_argument("--every", type=int, default=10)
    args = parser.parse_args()
    text = args.script.read_text(encoding="utf-8") if args.script else DEFAULT_SCRIPT
    sequence = parse_script(text)

    state = GameState.fresh(args.world)
    print(f"tick  key          x      y     x0     vx     score  fly died")
    for i, key in enumerate(sequence, start=1):
        tick(state, key)
        if i == 1 or i % args.every == 0 or i == len(sequence) or state.hero.died:
            names = ",".join(unpack(key)) or "-"
            h = state.hero
            print(
                f"{i:4d}  {names:12s} {h.x:6.1f} {h.y:6.1f} {h.x0:6.1f} "
                f"{h.vx:6.2f} {h.score:6d}  {int(h.is_fly)} {int(h.died)}"
            )
        if state.hero.died or state.hero.passed:
            break
    h = state.hero
    print()
    print(f"world={state.world} score={h.score} died={h.died} "
          f"passed={h.passed} ending={h.ending} shoot={h.is_shoot} "
          f"world_x={h.world_x:.1f}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
