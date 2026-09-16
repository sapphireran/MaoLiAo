#!/usr/bin/env python3
"""Run a replay DSL against the headless hero stepper."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from kit.commands import parse_replay  # noqa: E402
from kit.sim import run_replay  # noqa: E402


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("script", type=Path)
    p.add_argument("--world", type=int, default=1)
    p.add_argument("--seed", type=int, default=2020)
    args = p.parse_args()
    cmds = parse_replay(args.script.read_text(encoding="utf-8"))
    sim = run_replay(args.world, cmds, seed=args.seed)
    print(f"frames     {sim.frame}")
    print(f"screen     ({sim.x:.1f}, {sim.y:.1f})  x0={sim.x0:.1f}")
    print(f"world-x    {sim.wx:.1f}")
    print(f"v          ({sim.vX:.4f}, {sim.vY:.4f})")
    print(f"fly/shoot  {sim.is_fly} / {sim.is_shoot}")
    print(f"ending     {sim.ending}  passed={sim.passed}  died={sim.died}")
    print(f"score      {sim.score}  coins left={len(sim.coins_left)}")
    return 1 if sim.died else 0


if __name__ == "__main__":
    raise SystemExit(main())
