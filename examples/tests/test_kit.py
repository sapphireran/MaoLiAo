#!/usr/bin/env python3
"""Checks that the catalog and kit still match the V2.0 sources."""

from __future__ import annotations

import json
import math
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPO = ROOT.parent
sys.path.insert(0, str(ROOT))

from kit.camera import apply_rail, is_ending, sky_step, world_x  # noqa: E402
from kit.collision import Tile, hit_tile, tile_aabb  # noqa: E402
from kit.commands import CMD_LEFT, CMD_RIGHT, CMD_UP, Command, parse_replay  # noqa: E402
from kit.constants import C, assert_matches_catalog, friction_u, pixel_scale  # noqa: E402
from kit.kinematics import apex, inertia_move, jump_launch_vy, jump_profile, walk_step  # noqa: E402
from kit.msvc_rand import MsvcRand  # noqa: E402
from kit.reachability import classify_gaps, jump_envelope, ledges  # noqa: E402
from kit.savefile import read_record, write_record  # noqa: E402
from kit.sim import HeroSim, run_replay, step_hero  # noqa: E402
from kit.worlds import catalog_seed_heights, generate_world3, load_world  # noqa: E402

FAILED = 0


def check(ok: bool, msg: str) -> None:
    global FAILED
    if not ok:
        print(f"FAIL  {msg}")
        FAILED += 1
    else:
        print(f"ok    {msg}")


def source(name: str) -> str:
    return (REPO / "MaoLiAo" / name).read_text(encoding="utf-8", errors="replace")


def test_constants_vs_headers() -> None:
    assert_matches_catalog()
    define = source("define.h")
    role = source("role.h")
    scene = source("scene.h")
    check("#define XSIZE 512" in define, "define.h XSIZE")
    check("#define YSIZE 384" in define, "define.h YSIZE")
    check("#define UNREAL_HEIGHT (3*HEIGHT+5)" in define, "define.h UNREAL_HEIGHT")
    check("#define V_MAX 8.0" in define, "define.h V_MAX")
    check("#define LIFE 5;" in define, "LIFE trailing semicolon")
    check("#define MYDIFINE" in define, "include-guard typo MYDIFINE")
    check("const int MAP_NUMBER = 30;" in scene, "MAP_NUMBER")
    check("const int MAX_DISTANCE = 480;" in role, "MAX_DISTANCE")
    check(C.X0 == 64 and C.Y0 == 96, "X0/Y0")
    check(abs(pixel_scale() - 101 / 3.5) < 1e-12, "pixel scale 101/3.5")


def test_jump() -> None:
    samples = jump_profile()
    top = apex(samples)
    check(abs(jump_launch_vy() + math.sqrt(210)) < 1e-9, "launch -sqrt(210)")
    check(top.frame == 48, f"apex frame 48 (got {top.frame})")
    check(abs(top.rise_px - 101) < 0.01, f"apex ~101 (got {top.rise_px:.4f})")
    check(96 <= len(samples) <= 98, f"hang frames {len(samples)}")
    dx, nv = inertia_move(8.0, 0.01, 0.0)
    check(abs(dx - 0.08) < 1e-12 and abs(nv - 8.0) < 1e-12, "inertia coast")


def test_friction_and_walk() -> None:
    u = friction_u(C.T2)
    check(abs(u - (8 / 1.2) / 30) < 1e-12, "grass u")
    check(abs(friction_u(C.T1) - (8 / 0.5) / 30) < 1e-12, "ice u")
    dx, nv = walk_step(8.0, 0, u, True)
    check(nv < 8.0, "coast slows")
    check(dx > 0, "coast still moves right")
    _, nv2 = walk_step(8.0, 1, u, True)
    check(abs(nv2 - 8.0) < 1e-9, "hold D stays at V_MAX")


def test_collision() -> None:
    grass = Tile(0, 9, 1, 15, 1)
    x0, y0, x1, y1 = tile_aabb(grass)
    check((x0, y0, x1, y1) == (0, 288, 480, 320), "grass aabb")
    tile, lethal = hit_tile(64, 257, [grass])
    check(tile is grass and not lethal, "foot on grass")
    tile, _ = hit_tile(64, 200, [grass])
    check(tile is None, "air miss")
    pipe = Tile(36, 7, 10, 1, 1)
    x0, y0, x1, y1 = tile_aabb(pipe)
    check((x1 - x0, y1 - y0) == (64, 64), "pipe 2x2")
    cloud = Tile(80, 6, 2, 25, 1)
    _, lethal = hit_tile(80 * 32 + 4, 6 * 32 + 4, [cloud], world=3, is_shoot=False)
    check(not lethal, "world3 cloud is safe")
    pipe_hit, lethal = hit_tile(36 * 32 + 4, 7 * 32 + 4, [pipe], world=3, is_shoot=False)
    check(pipe_hit is pipe and lethal, "world3 pipe kills")
    _, lethal = hit_tile(36 * 32 + 4, 7 * 32 + 4, [pipe], world=3, is_shoot=True)
    check(not lethal, "flower grants pipe immunity")


def test_camera() -> None:
    x, x0, scrolled = apply_rail(200, 0, False)
    check(x == 192 and x0 == -8 and scrolled, "rail pin + x0")
    x, x0, scrolled = apply_rail(200, 0, True)
    check(x == 200 and x0 == 0 and not scrolled, "ending walks off rail")
    check(world_x(192, -8) == 200, "world_x")
    check(is_ending(3008, 1) is False, "ending exclusive")
    check(is_ending(3009, 1) is True, "ending 94 tiles + 1")
    check(is_ending(3329, 2) is True, "world2 104 tiles")
    check(abs(sky_step(8.0) - 8 * 0.01 * pixel_scale() / 5) < 1e-12, "sky step")


def test_world_tables() -> None:
    w1 = load_world(1, cap=False)
    w1c = load_world(1, cap=True)
    check(w1.authored_tile_count == 32, "world1 authored 32")
    check(len(w1c.runtime_tiles) == 30, "world1 runtime 30")
    check(len(w1.coins) == 20, "world1 20 coins")
    check(len(w1.enemies) == 10, "world1 10 enemies")
    check(w1.food_pixels == [(448, 160)], "world1 flower")
    check(w1.dropped[-1]["x"] == 75, "world1 last dropped water")
    check(w1.max_score == 250, "world1 max score")

    w2 = load_world(2, cap=False)
    w2c = load_world(2, cap=True)
    check(w2.authored_tile_count == 36, "world2 authored 36")
    check(len(w2c.runtime_tiles) == 30, "world2 runtime 30")
    ids = [t.id for t in w2c.runtime_tiles]
    check(12 not in ids, "world2 goal dropped by MAP_NUMBER")
    check(any(t.id == 12 for t in w2.tiles), "world2 authored goal exists")
    check(w2.food_pixels == [(1238, 102)], "world2 flower integer div")
    check(w2.max_score == 140, "world2 max score")

    w3 = generate_world3(2020)
    coins, pipes = catalog_seed_heights(2020)
    rng = MsvcRand(2020)
    got_c = [rng.random_ab(3, 7) for _ in range(10)]
    got_p = [rng.random_ab(1, 7) for _ in range(10)]
    check(got_c == coins, "seed 2020 coins match catalog")
    check(got_p == pipes, "seed 2020 pipes match catalog")
    check(w3.coins[0] == (5, coins[0]), "world3 first coin")
    check(len(w3.runtime_tiles) == 30, "world3 30 tiles")
    check(w3.food_pixels == [(10, 10)], "world3 flower is 10,10 px")


def test_msvc_known() -> None:
    rng = MsvcRand(1)
    first = [rng.rand() for _ in range(5)]
    check(first == [41, 18467, 6334, 26500, 19169], f"MSVC seed1 prefix {first}")


def test_savefile(tmp_path: Path) -> None:
    catalog = ROOT / "catalog" / "saves"
    v, st = read_record(catalog / "world3.dat")
    check(st == "ok" and v == 3, "checked-in style world 3")
    v, st = read_record(catalog / "invalid-zero.dat")
    check(st == "invalid" and v == 0, "zero invalid")
    v, st = read_record(catalog / "invalid-four.dat")
    check(st == "invalid" and v == 4, "four invalid")
    v, st = read_record(catalog / "garbage.dat")
    check(st == "garbage", "garbage")
    dest = tmp_path / "out.dat"
    write_record(dest, 2)
    v, st = read_record(dest)
    check(st == "ok" and v == 2, "roundtrip world 2")
    repo_save = REPO / "MaoLiAo" / "gameRecord.dat"
    v, st = read_record(repo_save)
    check(st == "ok" and v == 3, "repo gameRecord.dat is 3")


def test_commands() -> None:
    c = Command.from_keys("ADW")
    check(c.left and c.right and c.up, "A+D+W")
    check(Command.from_keys("K").up, "K is jump")
    check(CMD_LEFT == 1 and CMD_RIGHT == 2 and CMD_UP == 4, "bit values")
    frames = parse_replay("2 D\n1 DW\n# comment\n3 .\n")
    check(len(frames) == 6, "replay length")
    check(frames[2].up and frames[2].right, "DW frame")
    check(frames[3].bits == 0, "idle")


def test_sim_lands_and_walks() -> None:
    sim = HeroSim(world=load_world(1))
    idle = Command()
    for _ in range(90):
        step_hero(sim, idle)
    check(not sim.died, "idle fall did not pit")
    check(sim.y == 256, f"landed y=256 (got {sim.y})")
    check(sim.is_fly is False, "grounded after fall")
    right = Command.from_keys("D")
    for _ in range(90):
        step_hero(sim, right)
    check(sim.x == 192, f"pinned at XRIGHT (got {sim.x})")
    check(sim.x0 < 0, "camera scrolled")
    check(sim.wx > 192, "world-x advanced")


def test_replay_clears_first_gap() -> None:
    text = (ROOT / "replays" / "world1_opening.txt").read_text(encoding="utf-8")
    sim = run_replay(1, parse_replay(text))
    check(not sim.died, "opening replay survived")
    check(sim.wx > 576, f"past first gap (wx={sim.wx:.1f})")
    check(sim.y == 224, f"landed on y=8 shelf (y={sim.y})")
    check(sim.is_shoot, "jump clipped the first-cloud flower")


def test_gaps_world1() -> None:
    env = jump_envelope()
    check(env > 200, f"envelope {env:.1f} px")
    w1 = load_world(1)
    found = ledges(w1)
    first = next(g for g in found if g.left_tile == 14 and g.left_y == 9)
    check(first.width_px == 96, f"opening gap 96 (got {first.width_px})")
    check(first.right_tile == 18 and first.right_y == 8, "lands on raised shelf")
    check(first.jumpable_at_vmax, "opening gap jumpable")
    same = classify_gaps(w1)
    check(any(g.width_px >= 96 for g in same), "same-row gaps still reported")


def test_png_header(tmp_path: Path) -> None:
    from kit.raster import render_jump_png, render_png

    p = tmp_path / "w1.png"
    render_png(load_world(1), p)
    data = p.read_bytes()
    check(data.startswith(b"\x89PNG\r\n\x1a\n"), "world png signature")
    jp = tmp_path / "j.png"
    render_jump_png(jp)
    check(jp.read_bytes().startswith(b"\x89PNG\r\n\x1a\n"), "jump png signature")


def test_source_quirks_still_present() -> None:
    scene = source("scene.cpp")
    role = source("role.cpp")
    main = source("main.cpp")
    control = source("control.cpp")
    check("i <= sizeof(p) / sizeof(p[0])" in scene, "createCoin off-by-one")
    check("i <= sizeof(m) / sizeof(m[0])" in scene, "world3 map off-by-one")
    check("myHero.y = X0;" in role, "spawn uses X0 not Y0")
    check("while (true)" in main, "infinite loop")
    check("MessageBox(GetForegroundWindow()" in control, "MessageBox on bad save")
    check(re.search(r"world == 3 && myScene->getMap\(\)\[i\]\.id != 2", role),
          "world3 cloud exception")


def main() -> int:
    tmp = ROOT / "out" / "test-tmp"
    tmp.mkdir(parents=True, exist_ok=True)
    test_constants_vs_headers()
    test_jump()
    test_friction_and_walk()
    test_collision()
    test_camera()
    test_world_tables()
    test_msvc_known()
    test_savefile(tmp)
    test_commands()
    test_sim_lands_and_walks()
    test_replay_clears_first_gap()
    test_gaps_world1()
    test_png_header(tmp)
    test_source_quirks_still_present()
    print()
    if FAILED:
        print(f"{FAILED} failed")
        return 1
    print("all checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
