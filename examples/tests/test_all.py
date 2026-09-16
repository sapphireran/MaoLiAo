#!/usr/bin/env python3
"""Regression checks for the headless MaoLiAo ports.

Run from anywhere:

    python3 examples/tests/test_all.py
"""

from __future__ import annotations

import math
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPO = ROOT.parent
sys.path.insert(0, str(ROOT))

from maoliao_sim.collision import actor_corners, hit_map, is_hit, lethal_world3, rect_from_tile
from maoliao_sim.commands import Command, combine, describe, is_jump, parse_command_names
from maoliao_sim.constants import (
    CMD_LEFT,
    CMD_RIGHT,
    CMD_SHOOT,
    CMD_UP,
    ENDING,
    G,
    HEIGHT,
    LIFE,
    MAP_NUMBER,
    PIXEL_SCALE,
    REAL_HEIGHT,
    T1,
    T2,
    TIME,
    UNREAL_HEIGHT,
    V_MAX,
    VIR_HOME,
    WIDTH,
    XRIGHT,
    XSIZE,
    YSIZE,
)
from maoliao_sim.inertia import (
    friction_k,
    friction_u,
    integrate_jump,
    jump_launch_vy,
    move,
    peak_jump_pixels,
)
from maoliao_sim.save import read_record, valid_world, write_record
from maoliao_sim.worlds import (
    WORLD1_TILES,
    WORLD2_TILES,
    WORLD_SPECS,
    coins_for,
    enemies_for,
    food_for,
    generate_world3_tiles,
    loaded_tile_count,
    tiles_for,
)


class Failed(AssertionError):
    pass


def check(cond: bool, message: str) -> None:
    if not cond:
        raise Failed(message)


def test_constants() -> None:
    check(XSIZE == 512 and YSIZE == 384, "screen size")
    check(WIDTH == 32 and HEIGHT == 32, "tile size")
    check(XRIGHT == 192, "hero right clamp")
    check(UNREAL_HEIGHT == 101, "pixel jump height")
    check(abs(PIXEL_SCALE - 101 / 3.5) < 1e-12, "pixel scale")
    check(LIFE == 5, "lives")
    check(ENDING[1] == 94 * 32 and ENDING[2] == 104 * 32 and ENDING[3] == 94 * 32, "ending px")
    check(MAP_NUMBER == 30, "map cap")


def test_inertia_step() -> None:
    disp, new_v = move(8.0, 0.01, -20.0)
    expected_disp = 8.0 * 0.01 + 0.5 * (-20.0) * 0.01 * 0.01
    expected_v = 8.0 + (-20.0) * 0.01
    check(abs(disp - expected_disp) < 1e-12, f"move disp {disp}")
    check(abs(new_v - expected_v) < 1e-12, f"move v {new_v}")
    # Rest + gravity for one frame.
    d0, v0 = move(0.0, TIME, G)
    check(abs(d0 - 0.5 * G * TIME * TIME) < 1e-12, "rest gravity disp")
    check(abs(v0 - G * TIME) < 1e-12, "rest gravity v")


def test_jump() -> None:
    launch = jump_launch_vy()
    check(abs(launch - (-math.sqrt(2 * G * REAL_HEIGHT))) < 1e-12, "launch formula")
    peak, peak_frame = peak_jump_pixels()
    # Discrete integration undershoots the continuous 101 px slightly.
    check(95.0 < peak < 101.0, f"peak {peak} px out of expected band")
    check(40 <= peak_frame <= 55, f"peak frame {peak_frame}")
    rows = integrate_jump()
    check(rows[0][1] == launch, "first frame uses launch vy")
    check(rows[-1][3] <= 0.0, "arc returns to or below launch y")
    # Apex: vy crosses from <=0 to >0.
    crossings = [row for row in rows if row[1] <= 0 < row[1] + G * TIME]
    check(len(crossings) == 1, "single apex")


def test_friction() -> None:
    check(abs(friction_u(1, 1) - (V_MAX / T2) / G) < 1e-12, "grass u")
    check(abs(friction_u(6, 1) - (V_MAX / T1) / G) < 1e-12, "ice u")
    check(abs(friction_u(7, 1) - (V_MAX / 1.5) / G) < 1e-12, "default u")
    check(abs(friction_u(6, 3) - (V_MAX / T1) / G) < 1e-12, "world3 solid u")
    check(abs(friction_u(2, 3) - (V_MAX / T2) / G) < 1e-12, "world3 cloud u")
    check(friction_k(4.0) == -2.0, "k right")
    check(friction_k(-4.0) == 2.0, "k left")
    check(friction_k(0.0) == 0.0, "k rest")


def test_commands() -> None:
    bits = parse_command_names(["a", "w", "j"])
    check(bits == (CMD_LEFT | CMD_UP | CMD_SHOOT), f"combo bits {bits}")
    check(describe(bits) == ["CMD_LEFT", "CMD_UP", "CMD_SHOOT"], "describe combo")
    check(combine(Command.RIGHT, Command.UP) == (CMD_RIGHT | CMD_UP), "combine")
    check(is_jump(CMD_UP, is_fly=False, ending=False, world=1), "ground jump w1")
    check(not is_jump(CMD_UP, is_fly=True, ending=False, world=1), "no midair w1")
    check(is_jump(CMD_UP, is_fly=True, ending=False, world=3), "flap w3")
    check(not is_jump(CMD_UP, is_fly=False, ending=True, world=1), "no jump during ending")
    check(parse_command_names(["home"]) == VIR_HOME, "virtual home")


def test_collision() -> None:
    grass = (0, 9, 1, 15, 1)
    gx, gy, gid, gxa, gya = grass
    box = rect_from_tile(gx, gy, gxa, gya, gid)
    check(box.left == 0 and box.top == 9 * 32, "grass origin")
    check(box.right == 15 * 32 and box.bottom == 10 * 32, "grass extent")
    pipe = rect_from_tile(36, 7, 1, 1, 10)
    check(pipe.right - pipe.left == 64 and pipe.bottom - pipe.top == 64, "pipe 2x2")

    # Role probes hitMap(x, y+1). A hero whose sprite top is y=256 (tile row 8)
    # has an inset bottom vertex at 288, which is the top edge of the y=9 floor.
    standing = hit_map(64, 256 + 1, [grass], x0=0, world=1)
    check(standing == grass, f"stand on grass got {standing}")
    above = hit_map(64, 200, [grass], x0=0, world=1)
    check(above is None, "airborne miss")

    corners = actor_corners(0, 288)
    check(is_hit(corners, box), "inset corners still on the tile")

    cloud = (10, 6, 2, 4, 1)
    pipe_tile = (36, 7, 10, 1, 1)
    check(lethal_world3(pipe_tile, is_shoot=False), "world3 pipe kills")
    check(not lethal_world3(cloud, is_shoot=False), "world3 cloud safe")
    check(not lethal_world3(pipe_tile, is_shoot=True), "weapon blocks death")

    scenery = [(19, 6, 11, 1, 1)]
    check(hit_map(19 * 32, 6 * 32, scenery) is None, "scenery is not solid")


def test_worlds() -> None:
    check(len(WORLD1_TILES) == 32, f"world1 authored {len(WORLD1_TILES)}")
    check(loaded_tile_count(1) == 30, "world1 cap drops 2 water tiles")
    check(tiles_for(1)[-1] == (75, 10, 13, 1, 1), "last authored water")
    check(len(coins_for(1)) == 20, "world1 coins")
    check(len(enemies_for(1)) == 10, "world1 enemies")
    check(food_for(1) == ((448, 160),), "world1 mushroom pixels")

    check(len(WORLD2_TILES) == 36, f"world2 authored {len(WORLD2_TILES)}")
    check(loaded_tile_count(2) == 30, "world2 also hits the 30-tile cap")
    check(len(coins_for(2)) == 11, "world2 coins")
    check(len(enemies_for(2)) == 6, "world2 enemies")
    check(WORLD_SPECS[2].goal == (111, 1), "world2 goal")

    pipes = generate_world3_tiles([3, 4, 2, 5, 1, 6, 3, 9, 9, 9])
    check(len(pipes) == 7 * 4 + 2, "7 columns + cloud + goal")
    check(pipes[-2] == (80, 6, 2, 25, 1), "world3 cloud runway")
    check(pipes[-1] == (101, 4, 12, 1, 1), "world3 goal")
    check(food_for(3) == ((10, 10),), "world3 leftover food")
    check(len(enemies_for(3)) == 7, "world3 enemies")


def test_save_roundtrip() -> None:
    repo_save = REPO / "MaoLiAo" / "gameRecord.dat"
    if repo_save.exists() and repo_save.stat().st_size > 0:
        world = read_record(repo_save)
        check(valid_world(world), f"repo save {world}")
    with tempfile.TemporaryDirectory() as tmp:
        path = Path(tmp) / "gameRecord.dat"
        write_record(path, 2)
        check(path.read_text(encoding="ascii") == "2", "write payload")
        check(read_record(path) == 2, "read back")
        try:
            write_record(path, 4)
        except ValueError:
            pass
        else:
            raise Failed("world 4 should be rejected")
        path.write_text("0", encoding="ascii")
        try:
            read_record(path)
        except ValueError:
            pass
        else:
            raise Failed("world 0 should be rejected")


def test_ascii_and_export_scripts() -> None:
    ascii_py = ROOT / "levels" / "ascii_map.py"
    export_py = ROOT / "levels" / "export_worlds.py"
    out = subprocess.check_output([sys.executable, str(ascii_py), "--world", "1"], text=True)
    check("world 1" in out and "tiles 30/32" in out, f"ascii world1 banner:\n{out[:200]}")
    check("G" in out and "E" in out and "o" in out, "ascii glyphs")
    js = subprocess.check_output([sys.executable, str(export_py), "--world", "1"], text=True)
    check('"ending_tiles": 94' in js and '"id": 1' in js, "export json")
    with tempfile.TemporaryDirectory() as tmp:
        svg_path = Path(tmp) / "w1.svg"
        subprocess.check_call(
            [sys.executable, str(ROOT / "levels" / "svg_map.py"), "--world", "1", "-o", str(svg_path)]
        )
        svg = svg_path.read_text(encoding="utf-8")
        check("<svg" in svg and "world 1" in svg, "svg map")


def test_command_and_save_scripts() -> None:
    cmd = subprocess.check_output(
        [sys.executable, str(ROOT / "input" / "command_demo.py"), "a", "w"],
        text=True,
    )
    check("CMD_LEFT" in cmd and "CMD_UP" in cmd, "command demo")
    with tempfile.TemporaryDirectory() as tmp:
        path = Path(tmp) / "gameRecord.dat"
        subprocess.check_call(
            [
                sys.executable,
                str(ROOT / "save" / "record_demo.py"),
                "--write",
                str(path),
                "--world",
                "3",
            ]
        )
        shown = subprocess.check_output(
            [sys.executable, str(ROOT / "save" / "record_demo.py"), "--read", str(path)],
            text=True,
        )
        check("world=3" in shown, shown)


def test_cpp_inertia_demo() -> None:
    src = ROOT / "physics" / "inertia_demo.cpp"
    with tempfile.TemporaryDirectory() as tmp:
        binary = Path(tmp) / "inertia_demo"
        subprocess.check_call(["g++", "-O2", "-std=c++17", "-o", str(binary), str(src)])
        out = subprocess.check_output([str(binary)], text=True)
    check("launch vY = -14.491377" in out, f"cpp launch:\n{out}")
    check("peak rise" in out, "cpp peak line")
    # Match Python peak to a few tenths of a pixel.
    py_peak, _ = peak_jump_pixels()
    peak_line = [line for line in out.splitlines() if line.startswith("peak rise")][0]
    cpp_peak = float(peak_line.split()[2])
    check(abs(cpp_peak - py_peak) < 1e-4, f"cpp {cpp_peak} vs py {py_peak}")


def main() -> int:
    tests = [
        test_constants,
        test_inertia_step,
        test_jump,
        test_friction,
        test_commands,
        test_collision,
        test_worlds,
        test_save_roundtrip,
        test_ascii_and_export_scripts,
        test_command_and_save_scripts,
        test_cpp_inertia_demo,
    ]
    failed = 0
    for fn in tests:
        try:
            fn()
        except Failed as exc:
            failed += 1
            print(f"FAIL  {fn.__name__}: {exc}")
        except Exception as exc:  # noqa: BLE001
            failed += 1
            print(f"FAIL  {fn.__name__}: {type(exc).__name__}: {exc}")
        else:
            print(f"ok    {fn.__name__}")
    print()
    print(f"{len(tests) - failed}/{len(tests)} passed")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
