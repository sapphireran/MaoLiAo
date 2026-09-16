"""CLI: python3 -m examples.run_lab <command>"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

from examples.maoliao_lab.ascii_map import render
from examples.maoliao_lab.camera import Camera, world_x
from examples.maoliao_lab.commands import pack, unpack
from examples.maoliao_lab.constants import (
    ENDING_DISTANCE,
    G,
    LIFE,
    MAP_NUMBER,
    PIXELS_PER_REAL,
    TIME,
    U_DEFAULT,
    U_HIGH,
    U_NORMAL,
    V_MAX,
    WIDTH,
    XSIZE,
    YSIZE,
    jump_launch_vy,
)
from examples.maoliao_lab.friction import skim_to_stop
from examples.maoliao_lab.jump import apex, arc, expected_peak_px
from examples.maoliao_lab.loop import TickState, tick
from examples.maoliao_lab.savefile import decode, encode
from examples.maoliao_lab.sprites import MCI_ALIASES, hero_blit, hero_walk_iframe
from examples.maoliao_lab.worlds import load_world


def cmd_constants(_: argparse.Namespace) -> int:
    rows = [
        ("window", f"{XSIZE}x{YSIZE}"),
        ("tile", f"{WIDTH}x{WIDTH}"),
        ("dt", TIME),
        ("G", G),
        ("V_MAX", V_MAX),
        ("px per real unit", round(PIXELS_PER_REAL, 6)),
        ("jump vY0", round(jump_launch_vy(), 6)),
        ("u high/normal/default", f"{U_HIGH:.6f} / {U_NORMAL:.6f} / {U_DEFAULT:.6f}"),
        ("lives", LIFE),
        ("MAP_NUMBER cap", MAP_NUMBER),
        ("ending px w1/w2/w3", ENDING_DISTANCE),
    ]
    for name, value in rows:
        print(f"{name:24} {value}")
    return 0


def cmd_jump_table(ns: argparse.Namespace) -> int:
    samples = arc()
    peak = apex(samples)
    print(f"# ticks={len(samples)}  peak_px={peak.y_px:.4f}  expected={expected_peak_px():.4f}")
    print(f"{'tick':>6} {'t':>7} {'vy':>10} {'y_real':>10} {'y_px':>10} rising")
    step = max(1, ns.every)
    for s in samples[::step]:
        flag = "up" if s.rising else "down"
        print(f"{s.tick:6d} {s.t:7.3f} {s.vy:10.4f} {s.y_real:10.4f} {s.y_px:10.4f} {flag}")
    return 0


def cmd_ascii_map(ns: argparse.Namespace) -> int:
    world = load_world(ns.world)
    print(f"# world {ns.world}  stored tiles={len(world['tiles'])}  authored={world['authored_tile_count']}")
    if world.get("dropped"):
        print(f"# dropped by MAP_NUMBER cap: {len(world['dropped'])} records")
        for t in world["dropped"]:
            print(f"#   id={t['id']} x={t['x']} y={t['y']}")
    print(render(world["tiles"], world, scenery=not ns.solids_only))
    return 0


def cmd_tick(ns: argparse.Namespace) -> int:
    state = TickState(world=ns.world)
    bits = pack(ns.keys)
    print(f"# keys={ns.keys!r} bits={bits} ({','.join(unpack(bits)) or 'none'}) world={ns.world}")
    print(f"{'f':>5} {'t':>6} {'sx':>8} {'sy':>8} {'wx':>10} {'vx':>8} {'vy':>8} flags")
    for _ in range(ns.frames):
        log = tick(state, bits)
        flags = []
        if log.fly:
            flags.append("fly")
        if log.ending:
            flags.append("end")
        if log.passed:
            flags.append("pass")
        if log.died:
            flags.append("dead")
        print(
            f"{log.frame:5d} {log.t:6.2f} {log.x:8.2f} {log.y:8.2f} "
            f"{log.world_x:10.2f} {log.vx:8.3f} {log.vy:8.3f} {' '.join(flags) or '-'}"
        )
        if log.died or log.passed:
            break
    print(f"# camera x0={state.camera.x0:.2f} xBg={state.camera.x_bg:.4f} score={state.score}")
    return 0


def cmd_save(ns: argparse.Namespace) -> int:
    if ns.decode is not None:
        print(decode(ns.decode.encode("utf-8")))
        return 0
    if ns.encode is not None:
        sys.stdout.buffer.write(encode(ns.encode))
        return 0
    sample = Path("MaoLiAo/gameRecord.dat")
    if sample.exists():
        print(f"{sample}: {decode(sample.read_bytes())}")
    else:
        print("no MaoLiAo/gameRecord.dat")
    return 0


def cmd_friction(_: argparse.Namespace) -> int:
    print(f"{'surface':<12} {'u':>10} {'ticks to stop from V_MAX':>28}")
    for name, u in ("high", U_HIGH), ("normal", U_NORMAL), ("default", U_DEFAULT):
        print(f"{name:<12} {u:10.6f} {skim_to_stop(V_MAX, u):28d}")
    return 0


def cmd_sprites(_: argparse.Namespace) -> int:
    for pos in range(0, 80, 10):
        iframe = hero_walk_iframe(pos)
        blit = hero_blit(1, iframe)
        print(f"rolePos={pos:3d} iframe={iframe} color=({blit.color_x},{blit.color_y})")
    print("# MCI aliases")
    for alias, filename in MCI_ALIASES.items():
        print(f"{alias:18} {filename}")
    return 0


def cmd_summary(ns: argparse.Namespace) -> int:
    world = load_world(ns.world)
    payload = {
        "world": ns.world,
        "stored_tiles": len(world["tiles"]),
        "authored_tiles": world["authored_tile_count"],
        "dropped": world.get("dropped", []),
        "coins": len(world.get("coins", [])),
        "enemies": len(world.get("enemies", [])),
        "food_pixels": world.get("food_pixels"),
        "solids": sum(1 for t in world["tiles"] if t["solid"]),
        "goal_present": any(t.get("is_goal") for t in world["tiles"]),
    }
    print(json.dumps(payload, indent=2, ensure_ascii=False))
    return 0


def cmd_camera(ns: argparse.Namespace) -> int:
    cam = Camera()
    x = float(ns.start)
    for i, vx in enumerate([ns.vx] * ns.frames, start=1):
        prev = cam.x0
        x = cam.pin_hero_screen_x(x + vx * TIME * PIXELS_PER_REAL, vx, ending=False)
        cam.step_parallax(vx, x, prev)
        print(
            f"{i:4d} screen={x:7.2f} x0={cam.x0:9.2f} world={world_x(x, cam.x0):9.2f} "
            f"xBg={cam.x_bg:8.4f}"
        )
    return 0


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(prog="python3 -m examples.run_lab", description=__doc__)
    sub = p.add_subparsers(dest="cmd", required=True)

    sub.add_parser("constants", help="print define.h numbers").set_defaults(func=cmd_constants)

    j = sub.add_parser("jump-table", help="print the gravity arc")
    j.add_argument("--every", type=int, default=5)
    j.set_defaults(func=cmd_jump_table)

    a = sub.add_parser("ascii-map", help="dump a world as ASCII")
    a.add_argument("--world", type=int, default=1, choices=(1, 2, 3))
    a.add_argument("--solids-only", action="store_true")
    a.set_defaults(func=cmd_ascii_map)

    t = sub.add_parser("tick", help="run the toy loop")
    t.add_argument("--keys", default="D", help="e.g. D, AW, DJ")
    t.add_argument("--frames", type=int, default=60)
    t.add_argument("--world", type=int, default=1, choices=(1, 2, 3))
    t.set_defaults(func=cmd_tick)

    s = sub.add_parser("save", help="encode/decode gameRecord.dat")
    s.add_argument("--encode", type=int)
    s.add_argument("--decode")
    s.set_defaults(func=cmd_save)

    sub.add_parser("friction", help="skid distances per surface").set_defaults(func=cmd_friction)
    sub.add_parser("sprites", help="walk-cycle iframe table").set_defaults(func=cmd_sprites)

    u = sub.add_parser("summary", help="JSON summary of a world")
    u.add_argument("--world", type=int, default=1, choices=(1, 2, 3))
    u.set_defaults(func=cmd_summary)

    c = sub.add_parser("camera", help="pin to XRIGHT and crawl x0/xBg")
    c.add_argument("--start", type=float, default=64)
    c.add_argument("--vx", type=float, default=8.0)
    c.add_argument("--frames", type=int, default=40)
    c.set_defaults(func=cmd_camera)
    return p


def main(argv: list[str] | None = None) -> int:
    ns = build_parser().parse_args(argv)
    return ns.func(ns)


if __name__ == "__main__":
    raise SystemExit(main())
