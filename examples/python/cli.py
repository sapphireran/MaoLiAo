#!/usr/bin/env python3
"""Headless MaoLiAo tools: preview, lint, physics, saves, command flags."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from maoliao.commands import decode, from_keys  # noqa: E402
from maoliao.constants import (  # noqa: E402
    A_ROLE,
    G,
    PIXELS_PER_METRE,
    TIME,
    V_MAX,
    friction_u,
)
from maoliao.lint import findings_as_dicts, lint_all, lint_world  # noqa: E402
from maoliao.maps import colliding_extent_tiles, world_payload  # noqa: E402
from maoliao.physics import (  # noqa: E402
    apex_pixels,
    apex_time_seconds,
    hold_right_until_vmax,
    jump_initial_vy,
    run_step,
    simulate_jump,
)
from maoliao.preview import render  # noqa: E402
from maoliao.savefile import SaveError, read_world, write_world  # noqa: E402


def cmd_preview(args: argparse.Namespace) -> int:
    print(render(args.world, args.seed))
    return 0


def cmd_lint(args: argparse.Namespace) -> int:
    if args.world:
        findings = lint_world(args.world, args.seed)
    else:
        findings = lint_all(args.seed)
    if args.json:
        json.dump(findings_as_dicts(findings), sys.stdout, indent=2)
        sys.stdout.write("\n")
    else:
        if not findings:
            print("no findings")
        for item in findings:
            print(f"world {item.world:d}  {item.severity:7s}  {item.code}: {item.message}")
    return 1 if any(f.severity == "error" for f in findings) else 0


def cmd_physics(args: argparse.Namespace) -> int:
    samples = simulate_jump(ticks=args.ticks)
    print(f"vY0            {jump_initial_vy():.10f}")
    print(f"G              {G}")
    print(f"TIME           {TIME}")
    print(f"pixels/metre   {PIXELS_PER_METRE:.10f}")
    print(f"apex time s    {apex_time_seconds():.10f}")
    print(f"apex pixels    {apex_pixels():.10f}  (negative = upward in metres, EasyX y goes down)")
    print(f"ticks to V_MAX holding D on T2: {hold_right_until_vmax('T2')}")
    print(f"u T1/T2/T3     {friction_u('T1'):.6f}  {friction_u('T2'):.6f}  {friction_u('T3'):.6f}")
    print()
    print("tick  t_s        vY            y_metres       y_pixels")
    for s in samples:
        print(
            f"{s.tick:4d}  {s.t:8.4f}  {s.vy:12.6f}  {s.y_metres:12.6f}  {s.y_pixels:12.4f}"
        )
    if args.csv:
        path = Path(args.csv)
        path.parent.mkdir(parents=True, exist_ok=True)
        with path.open("w", encoding="utf-8") as fh:
            fh.write("tick,t,vy,y_metres,y_pixels,airborne\n")
            for s in samples:
                fh.write(
                    f"{s.tick},{s.t},{s.vy},{s.y_metres},{s.y_pixels},{int(s.airborne)}\n"
                )
        print(f"\nwrote {path}", file=sys.stderr)
    return 0


def cmd_run(args: argparse.Namespace) -> int:
    vx = args.vx
    print("tick  vx            dx_pixels")
    for tick in range(args.ticks):
        dx, vx = run_step(vx, A_ROLE if args.hold == "right" else -A_ROLE if args.hold == "left" else 0.0, True, args.friction)
        print(f"{tick + 1:4d}  {vx:12.6f}  {dx:12.4f}")
        if abs(vx) >= V_MAX - 1e-12 and args.hold != "none":
            print(f"# clamped at V_MAX={V_MAX}")
            break
    return 0


def cmd_save_read(args: argparse.Namespace) -> int:
    try:
        world = read_world(args.path)
    except SaveError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    print(world)
    return 0


def cmd_save_write(args: argparse.Namespace) -> int:
    try:
        write_world(args.path, args.world)
    except SaveError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    print(f"wrote world {args.world} -> {args.path}")
    return 0


def cmd_decode(args: argparse.Namespace) -> int:
    if args.keys:
        mask = from_keys(args.keys)
        print(f"mask {mask} (0x{mask:x}) from keys {args.keys!r}")
    else:
        mask = args.mask
    names = decode(mask)
    print(f"{mask} = {' | '.join(names) if names else '(none)'}")
    return 0


def cmd_extent(args: argparse.Namespace) -> int:
    payload = world_payload(args.world, args.seed)
    extent = colliding_extent_tiles(payload["maps_loaded"])
    print(
        json.dumps(
            {
                "world": args.world,
                "ending_tiles": payload["ending_tiles"],
                "ending_pixels": payload["ending_tiles"] * 32,
                "colliding_extent_tiles": extent,
                "loaded_maps": len(payload["maps_loaded"]),
                "authored_maps": len(payload["maps_authored"]),
                "truncated": len(payload["maps_truncated"]),
            },
            indent=2,
        )
    )
    return 0


def cmd_dump(args: argparse.Namespace) -> int:
    payload = world_payload(args.world, args.seed)
    json.dump(payload, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description=__doc__)
    sub = p.add_subparsers(dest="cmd", required=True)

    prev = sub.add_parser("preview", help="ASCII map for a world")
    prev.add_argument("--world", type=int, default=1, choices=(1, 2, 3))
    prev.add_argument("--seed", type=int, default=1, help="world 3 RNG seed (Python)")
    prev.set_defaults(func=cmd_preview)

    lint = sub.add_parser("lint", help="capacity and runway checks")
    lint.add_argument("--world", type=int, choices=(1, 2, 3))
    lint.add_argument("--seed", type=int, default=1)
    lint.add_argument("--json", action="store_true")
    lint.set_defaults(func=cmd_lint)

    phys = sub.add_parser("physics-jump", help="print the ballistic jump table")
    phys.add_argument("--ticks", type=int, default=40)
    phys.add_argument("--csv")
    phys.set_defaults(func=cmd_physics)

    run = sub.add_parser("physics-run", help="horizontal integrate while holding a key")
    run.add_argument("--hold", choices=("right", "left", "none"), default="right")
    run.add_argument("--friction", default="T2", choices=("T1", "T2", "T3"))
    run.add_argument("--vx", type=float, default=0.0)
    run.add_argument("--ticks", type=int, default=80)
    run.set_defaults(func=cmd_run)

    sr = sub.add_parser("save-read", help="read gameRecord.dat")
    sr.add_argument("path")
    sr.set_defaults(func=cmd_save_read)

    sw = sub.add_parser("save-write", help="write a valid one-integer save")
    sw.add_argument("path")
    sw.add_argument("--world", type=int, required=True, choices=(1, 2, 3))
    sw.set_defaults(func=cmd_save_write)

    dec = sub.add_parser("decode", help="decode a CMD_* bitmask")
    dec.add_argument("--mask", type=int, default=None)
    dec.add_argument("--keys", help="e.g. A+W+J")
    dec.set_defaults(func=cmd_decode)

    ext = sub.add_parser("extent", help="ending trigger vs colliding map width")
    ext.add_argument("--world", type=int, default=1, choices=(1, 2, 3))
    ext.add_argument("--seed", type=int, default=1)
    ext.set_defaults(func=cmd_extent)

    dump = sub.add_parser("dump", help="JSON dump of a resolved world")
    dump.add_argument("--world", type=int, default=1, choices=(1, 2, 3))
    dump.add_argument("--seed", type=int, default=1)
    dump.set_defaults(func=cmd_dump)

    return p


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if args.cmd == "decode" and args.mask is None and not args.keys:
        build_parser().parse_args(["decode", "--help"])
        return 2
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
