#!/usr/bin/env python3
"""Level toolkit for the personal MaoLiAo EasyX project.

Mirrors constants and rules from MaoLiAo/define.h, scene.h, scene.cpp,
role.h, and role.cpp. The game itself does not import this file.
"""

from __future__ import annotations

import argparse
import json
import math
import sys
from pathlib import Path
from typing import Any, Iterable

# --- engine constants (define.h / scene.h / role.h) ---------------------

XSIZE = 512
YSIZE = 384
WIDTH = 32
HEIGHT = 32
X0 = 2 * WIDTH
Y0 = 3 * HEIGHT
TIME = 0.01
G = 30.0
REAL_HEIGHT = 3.5
UNREAL_HEIGHT = 3 * HEIGHT + 5
V_MAX = 8.0
A_ROLE = 20.0
T1 = 0.5
T2 = 1.2
T3 = 1.5
XLEFT = 0
XRIGHT = WIDTH * 6
MAP_NUMBER = 30
COINS_NUMBER = 70
FOOD_NUMBER = 5
ENEMY_TOTE = 30
K_MAP_BG = 5

SOLID_IDS = set(range(1, 11))
SCENERY_IDS = {11, 12, 13, 14}
PIPE_MOUTH_IDS = {8, 10}

TILE_NAMES = {
    1: "grass-surface",
    2: "cloud",
    3: "dirt-fill",
    4: "snow-fill",
    5: "snow-surface",
    6: "slick / pipe-shaft",
    7: "pipe-extension",
    8: "pipe-mouth-down",
    9: "pipe-mouth-up-unused",
    10: "pipe-mouth-up",
    11: "bg-grass",
    12: "goal-sign",
    13: "water",
    14: "tree",
}

GLYPH = {
    1: "#",
    2: "C",
    3: "=",
    4: "=",
    5: "#",
    6: "/",
    7: "|",
    8: "P",
    9: "P",
    10: "P",
    11: ",",
    12: "G",
    13: "~",
    14: "T",
}


class LevelError(ValueError):
    pass


def metres_to_pixels(metres: float) -> float:
    return metres * UNREAL_HEIGHT / REAL_HEIGHT


def jump_takeoff_speed() -> float:
    return -math.sqrt(2.0 * G * REAL_HEIGHT)


def jump_apex_pixels() -> float:
    return float(UNREAL_HEIGHT)


def jump_apex_tiles() -> float:
    return jump_apex_pixels() / HEIGHT


def friction_u(tile_id: int, profile: str) -> float:
    """Reproduce Scene::createMap's u assignment."""
    if profile == "pipes":
        if tile_id in (1, 3, 4, 5, 6):
            t = T1
        elif tile_id == 2:
            t = T2
        else:
            t = T3
    elif profile == "overworld":
        if tile_id in (1, 2, 3, 4, 5):
            t = T2
        elif tile_id == 6:
            t = T1
        else:
            t = T3
    else:
        raise LevelError(f"unknown friction_profile {profile!r}")
    return (V_MAX / t) / G


def friction_accel(tile_id: int, profile: str, vx_sign: int) -> float:
    """k * G * u with the +3/-3 hack from Role::action."""
    u = friction_u(tile_id, profile)
    k = 4.0 if vx_sign < 0 else -2.0
    return k * G * u


def cell_size(tile_id: int) -> tuple[int, int]:
    """Collision box size in tiles, matching Role::hitMap."""
    if tile_id in PIPE_MOUTH_IDS:
        return 2, 2
    return 1, 1


def expand_cells(tile: dict[str, Any]) -> list[tuple[int, int]]:
    """Top-left tile cells covered by the collision box (not the blit)."""
    cw, ch = cell_size(int(tile["id"]))
    xa = max(0, int(tile["x_amount"]))
    ya = max(0, int(tile["y_amount"]))
    cells = []
    origin_x = int(tile["x"])
    origin_y = int(tile["y"])
    width = xa * cw
    height = ya * ch
    for j in range(width):
        for k in range(height):
            cells.append((origin_x + j, origin_y + k))
    return cells


def food_pixels(item: dict[str, Any]) -> tuple[float, float]:
    unit = item.get("unit")
    if unit == "tiles":
        return float(item["x"]) * WIDTH, float(item["y"]) * HEIGHT
    if unit == "pixels":
        return float(item["x"]), float(item["y"])
    raise LevelError("food.unit must be 'tiles' or 'pixels'")


def load_level(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise LevelError(f"{path}: root must be an object")
    data["_path"] = str(path)
    return data


def is_shipped(level: dict[str, Any]) -> bool:
    return int(level.get("world", 0)) in (1, 2, 3) and str(level.get("id", "")).startswith(
        "world-"
    )


def build_pipe_column(x: int, height: int) -> list[dict[str, Any]]:
    """World 3 generator from Scene::createMap."""
    return [
        {"x": x, "y": height - 2, "id": 8, "x_amount": 1, "y_amount": 1, "kind": "pipe-mouth-down"},
        {
            "x": x,
            "y": 0,
            "id": 7,
            "x_amount": 1,
            "y_amount": max(0, height - 2),
            "kind": "pipe-shaft-top",
        },
        {"x": x, "y": 4 + height, "id": 10, "x_amount": 1, "y_amount": 1, "kind": "pipe-mouth-up"},
        {
            "x": x,
            "y": height + 6,
            "id": 7,
            "x_amount": 1,
            "y_amount": max(0, 6 - height),
            "kind": "pipe-shaft-bottom",
        },
    ]


def generate_world3(heights: Iterable[int]) -> list[dict[str, Any]]:
    tiles: list[dict[str, Any]] = []
    for i, h in enumerate(heights):
        tiles.extend(build_pipe_column(i * 10 + 10, int(h)))
    tiles.append({"x": 80, "y": 6, "id": 2, "x_amount": 25, "y_amount": 1, "kind": "cloud-runway"})
    tiles.append({"x": 101, "y": 4, "id": 12, "x_amount": 1, "y_amount": 1, "kind": "goal-sign"})
    return tiles


def _issue(level: dict[str, Any], severity: str, code: str, message: str) -> dict[str, str]:
    return {
        "level": str(level.get("id", "?")),
        "severity": severity,
        "code": code,
        "message": message,
    }


def validate(level: dict[str, Any]) -> list[dict[str, str]]:
    issues: list[dict[str, str]] = []
    required = ("id", "world", "name", "ending_tiles", "friction_profile", "tiles", "coins", "food", "enemies")
    for key in required:
        if key not in level:
            issues.append(_issue(level, "error", "missing-field", f"missing {key}"))
            return issues

    profile = level["friction_profile"]
    if profile not in ("overworld", "pipes"):
        issues.append(_issue(level, "error", "friction", f"bad friction_profile {profile!r}"))

    tiles = level["tiles"]
    coins = level["coins"]
    food = level["food"]
    enemies = level["enemies"]

    n_tiles = len(tiles)
    if n_tiles > MAP_NUMBER:
        severity = "warning" if is_shipped(level) else "error"
        issues.append(
            _issue(
                level,
                severity,
                "map-cap",
                f"{n_tiles} tiles authored; MAP_NUMBER is {MAP_NUMBER} "
                f"(C++ copies the first {MAP_NUMBER} only)",
            )
        )
    if len(coins) > COINS_NUMBER:
        issues.append(_issue(level, "error", "coin-cap", f"{len(coins)} coins > {COINS_NUMBER}"))
    if len(food) > FOOD_NUMBER:
        issues.append(_issue(level, "error", "food-cap", f"{len(food)} food > {FOOD_NUMBER}"))
    if len(enemies) > ENEMY_TOTE:
        issues.append(_issue(level, "error", "enemy-cap", f"{len(enemies)} enemies > {ENEMY_TOTE}"))

    if int(level["ending_tiles"]) <= 0:
        issues.append(_issue(level, "error", "ending", "ending_tiles must be > 0"))

    for i, tile in enumerate(tiles):
        tid = int(tile["id"])
        if tid < 1 or tid > 14:
            issues.append(_issue(level, "error", "tile-id", f"tiles[{i}] id {tid} not in 1..14"))
        if int(tile["x_amount"]) < 0 or int(tile["y_amount"]) < 0:
            issues.append(_issue(level, "error", "amount", f"tiles[{i}] negative amount"))

    occupied: dict[tuple[int, int], int] = {}
    solids: list[tuple[int, int]] = []
    for tile in tiles:
        tid = int(tile["id"])
        if tid not in SOLID_IDS:
            continue
        for cell in expand_cells(tile):
            if cell in occupied and occupied[cell] != tid:
                issues.append(
                    _issue(
                        level,
                        "warning",
                        "overlap",
                        f"solid overlap at {cell} (ids {occupied[cell]} and {tid})",
                    )
                )
            occupied[cell] = tid
            solids.append(cell)

    if not solids:
        issues.append(_issue(level, "error", "no-floor", "no solid tiles; hero will fall"))

    for i, coin in enumerate(coins):
        if coin["x"] == 0 and coin["y"] == 0:
            issues.append(
                _issue(level, "error", "coin-origin", f"coins[{i}] is (0,0); Scene treats that as empty")
            )
        if coin["y"] < 0:
            issues.append(_issue(level, "warning", "coin-y", f"coins[{i}] y={coin['y']} is above the map"))
        if solids:
            nearest = min(abs(int(coin["y"]) - sy) for _sx, sy in solids)
            if nearest > 4:
                issues.append(
                    _issue(
                        level,
                        "warning",
                        "coin-height",
                        f"coins[{i}] at y={coin['y']} is {nearest} tiles from the nearest solid row "
                        f"(jump budget is {jump_apex_tiles():.2f} tiles)",
                    )
                )

    for i, item in enumerate(food):
        try:
            food_pixels(item)
        except LevelError as exc:
            issues.append(_issue(level, "error", "food-unit", f"food[{i}]: {exc}"))

    for i, enemy in enumerate(enemies):
        turn = int(enemy["turn"])
        if turn not in (-1, 1):
            issues.append(_issue(level, "error", "enemy-turn", f"enemies[{i}] turn must be ±1"))

    proc = level.get("procedural")
    if proc:
        heights = proc.get("frozen_heights") or []
        expected = generate_world3(heights)
        authored = [
            (int(t["x"]), int(t["y"]), int(t["id"]), int(t["x_amount"]), int(t["y_amount"]))
            for t in tiles
        ]
        built = [
            (int(t["x"]), int(t["y"]), int(t["id"]), int(t["x_amount"]), int(t["y_amount"]))
            for t in expected
        ]
        if authored != built:
            issues.append(
                _issue(
                    level,
                    "warning",
                    "procedural-mismatch",
                    "tiles[] does not match generate_world3(frozen_heights)",
                )
            )

    return issues


def render_ascii(level: dict[str, Any], max_width: int | None = None) -> str:
    tiles = level["tiles"]
    if not tiles:
        return "(empty)"

    cells: dict[tuple[int, int], str] = {}
    xs: list[int] = []
    ys: list[int] = []
    for tile in tiles:
        tid = int(tile["id"])
        glyph = GLYPH.get(tid, "?")
        # draw uses 1-tile steps even for 64 px blits; show the collision footprint
        covered = expand_cells(tile)
        if not covered and tid in SOLID_IDS:
            continue
        if not covered:
            cells[(int(tile["x"]), int(tile["y"]))] = glyph
            xs.append(int(tile["x"]))
            ys.append(int(tile["y"]))
            continue
        for cell in covered:
            # prefer solids over scenery if both claim a cell
            prev = cells.get(cell)
            if prev in {",", "T", "~", "G"} or prev is None:
                cells[cell] = glyph
            xs.append(cell[0])
            ys.append(cell[1])

    for coin in level["coins"]:
        key = (int(coin["x"]), int(coin["y"]))
        if cells.get(key) in (None, ",", "T", "~"):
            cells[key] = "o"
            xs.append(key[0])
            ys.append(key[1])

    for enemy in level["enemies"]:
        key = (int(enemy["x"]), int(enemy["y"]))
        cells[key] = "E"
        xs.append(key[0])
        ys.append(key[1])

    for item in level["food"]:
        px, py = food_pixels(item)
        key = (int(px // WIDTH), int(py // HEIGHT))
        cells[key] = "F"
        xs.append(key[0])
        ys.append(key[1])

    ending = int(level["ending_tiles"])
    xs.append(ending)
    min_x, max_x = min(xs), max(xs)
    min_y, max_y = min(ys), max(ys)
    if max_width is not None:
        max_x = min(max_x, min_x + max_width - 1)

    lines = [
        f"{level['name']}  world={level['world']}  ending@{ending}  "
        f"jump={jump_apex_tiles():.2f} tiles",
        "legend: # floor  C cloud  P pipe  | shaft  / slick  o coin  E enemy  "
        "F food  G goal  ~ water  T tree",
        "",
    ]
    for y in range(min_y, max_y + 1):
        row = []
        for x in range(min_x, max_x + 1):
            if x == ending:
                row.append(">")
            else:
                row.append(cells.get((x, y), " "))
        lines.append(f"{y:3d}|{''.join(row)}")
    axis = "   +" + "".join("-" if (x - min_x) % 10 else "+" for x in range(min_x, max_x + 1))
    nums = "    " + "".join(str((x // 10) % 10) if x % 10 == 0 else " " for x in range(min_x, max_x + 1))
    lines.append(axis)
    lines.append(nums)
    return "\n".join(lines)


def export_cpp(level: dict[str, Any]) -> str:
    world = int(level["world"])
    profile = level["friction_profile"]
    lines = [
        f"// generated from {level.get('_path', level['id'])}",
        f"// {level['name']} — paste into the world == {world} branches.",
        f"// friction_profile = {profile}",
        "",
        f"// Scene::createMap  (world == {world})",
        "Map m[] = {",
    ]
    for tile in level["tiles"]:
        kind = tile.get("kind", TILE_NAMES.get(int(tile["id"]), ""))
        lines.append(
            f"    {{ {int(tile['x'])}, {int(tile['y'])}, {int(tile['id'])}, "
            f"{int(tile['x_amount'])}, {int(tile['y_amount'])} }},  // {kind}"
        )
    lines.append("};")
    lines.append("int i = 0;")
    lines.append("int n = sizeof(m) / sizeof(m[0]);")
    lines.append("while (i < n && i < MAP_NUMBER)")
    lines.append("{")
    if profile == "pipes":
        lines.append("    switch (m[i].id)")
        lines.append("    {")
        lines.append("    case 1: case 3: case 4: case 5: case 6:")
        lines.append("        m[i].u = (V_MAX / T1) / G; break;")
        lines.append("    case 2:")
        lines.append("        m[i].u = (V_MAX / T2) / G; break;")
        lines.append("    default:")
        lines.append("        m[i].u = (V_MAX / T3) / G; break;")
        lines.append("    }")
    else:
        lines.append("    switch (m[i].id)")
        lines.append("    {")
        lines.append("    case 1: case 2: case 3: case 4: case 5:")
        lines.append("        m[i].u = (V_MAX / T2) / G; break;")
        lines.append("    case 6:")
        lines.append("        m[i].u = (V_MAX / T1) / G; break;")
        lines.append("    default:")
        lines.append("        m[i].u = (V_MAX / T3) / G; break;")
        lines.append("    }")
    lines.append("    map[i] = m[i];")
    lines.append("    i++;")
    lines.append("}")
    lines.append("")
    lines.append(f"// Scene::createCoin  (world == {world})")
    lines.append("POINT p[] = {")
    for coin in level["coins"]:
        lines.append(f"    {{ {int(coin['x'])}, {int(coin['y'])} }},")
    lines.append("};")
    lines.append("for (int i = 0; i < (int)(sizeof(p) / sizeof(p[0])) && i < COINS_NUMBER; i++)")
    lines.append("{")
    lines.append("    coins[i].x = p[i].x;")
    lines.append("    coins[i].y = p[i].y;")
    lines.append("}")
    lines.append("")
    lines.append(f"// Scene::createFood  (world == {world})")
    lines.append("POINT f[] = {")
    for item in level["food"]:
        px, py = food_pixels(item)
        lines.append(f"    {{ {int(px)}, {int(py)} }},  // {item.get('unit')}")
    lines.append("};")
    lines.append("for (int i = 0; i < (int)(sizeof(f) / sizeof(f[0])) && i < FOOD_NUMBER; i++)")
    lines.append("{")
    lines.append("    food[i].x = f[i].x;")
    lines.append("    food[i].y = f[i].y;")
    lines.append("}")
    lines.append("")
    lines.append(f"// Role::createEnemy  (world == {world})")
    lines.append("Enemy emy[] = {")
    for enemy in level["enemies"]:
        lines.append(f"    {{ {int(enemy['x'])}, {int(enemy['y'])}, {int(enemy['turn'])} }},")
    lines.append("};")
    lines.append("for (int i = 0; i < (int)(sizeof(emy) / sizeof(emy[0])) && i < ENEMY_TOTE; i++)")
    lines.append("{")
    lines.append("    myEnemy[i].x = emy[i].x * WIDTH;")
    lines.append("    myEnemy[i].y = emy[i].y * HEIGHT;")
    lines.append("    myEnemy[i].turn = emy[i].turn;")
    lines.append("}")
    lines.append("")
    lines.append(f"// Scene::isEnding  case {world}:")
    lines.append(f"// if (distance > {int(level['ending_tiles'])} * WIDTH) return true;")
    return "\n".join(lines) + "\n"


def stats(level: dict[str, Any]) -> dict[str, Any]:
    tiles = level["tiles"]
    solids = [t for t in tiles if int(t["id"]) in SOLID_IDS]
    scenery = [t for t in tiles if int(t["id"]) in SCENERY_IDS]
    ids: dict[int, int] = {}
    for t in tiles:
        ids[int(t["id"])] = ids.get(int(t["id"]), 0) + 1
    max_x = 0
    for t in tiles:
        cw, _ = cell_size(int(t["id"]))
        max_x = max(max_x, int(t["x"]) + int(t["x_amount"]) * cw)
    return {
        "id": level["id"],
        "world": level["world"],
        "name": level["name"],
        "tiles": len(tiles),
        "tiles_installed": min(len(tiles), MAP_NUMBER),
        "tiles_dropped": max(0, len(tiles) - MAP_NUMBER),
        "solids": len(solids),
        "scenery": len(scenery),
        "coins": len(level["coins"]),
        "food": len(level["food"]),
        "enemies": len(level["enemies"]),
        "ending_tiles": level["ending_tiles"],
        "ending_pixels": int(level["ending_tiles"]) * WIDTH,
        "max_tile_x": max_x,
        "id_histogram": ids,
        "shipped": is_shipped(level),
    }


def format_stats(s: dict[str, Any]) -> str:
    hist = ", ".join(f"{k}:{v}" for k, v in sorted(s["id_histogram"].items()))
    dropped = f"  dropped={s['tiles_dropped']}" if s["tiles_dropped"] else ""
    return (
        f"{s['id']:28} world={s['world']:<2} tiles={s['tiles']:<3}"
        f"{dropped}  coins={s['coins']:<3} food={s['food']}  "
        f"enemies={s['enemies']:<2}  end={s['ending_tiles']}t/{s['ending_pixels']}px  "
        f"ids[{hist}]"
    )


def print_legend() -> str:
    rows = ["id  solid  name                 u(overworld)  u(pipes)  collide"]
    for tid in range(1, 15):
        cw, ch = cell_size(tid)
        rows.append(
            f"{tid:2d}  {'yes' if tid in SOLID_IDS else 'no ':3}  "
            f"{TILE_NAMES[tid]:20}  "
            f"{friction_u(tid, 'overworld'):12.4f}  "
            f"{friction_u(tid, 'pipes'):8.4f}  "
            f"{cw}x{ch} tiles"
        )
    return "\n".join(rows)


def print_jump() -> str:
    v0 = jump_takeoff_speed()
    apex_t = abs(v0) / G
    return "\n".join(
        [
            f"G                 {G}",
            f"REAL_HEIGHT       {REAL_HEIGHT} m",
            f"UNREAL_HEIGHT     {UNREAL_HEIGHT} px",
            f"takeoff vY        {v0:.6f} m/s",
            f"apex time         {apex_t:.4f} s  ({apex_t / TIME:.1f} frames)",
            f"apex pixels       {jump_apex_pixels():.1f}",
            f"apex tiles        {jump_apex_tiles():.3f}",
            f"V_MAX             {V_MAX} m/s  →  {metres_to_pixels(V_MAX):.3f} px/s",
            f"px per frame @Vmax {metres_to_pixels(V_MAX) * TIME:.3f}",
            f"XRIGHT rail       {XRIGHT} px",
            f"u mid/high/low    {friction_u(1, 'overworld'):.4f} / "
            f"{friction_u(6, 'overworld'):.4f} / {friction_u(10, 'overworld'):.4f}",
            f"a1 right/left @id1 {friction_accel(1, 'overworld', +1):.3f} / "
            f"{friction_accel(1, 'overworld', -1):.3f}",
        ]
    )


def inset_quad(x: int, y: int, x0: float) -> list[tuple[float, float]]:
    """Hero corners used by Role::hitMap / hitCoins / hitEnemy."""
    return [
        (-x0 + x + 1, y + 1),
        (-x0 + x + WIDTH - 1, y + 1),
        (-x0 + x + 1, y + HEIGHT - 1),
        (-x0 + x + WIDTH - 1, y + HEIGHT - 1),
    ]


def is_hit(quad: list[tuple[float, float]], box: tuple[float, float, float, float]) -> bool:
    x0, y0, x1, y1 = box
    for px, py in quad:
        if x0 <= px <= x1 and y0 <= py <= y1:
            return True
    return False


def _paths(values: list[str]) -> list[Path]:
    out: list[Path] = []
    for value in values:
        path = Path(value)
        if path.is_dir():
            out.extend(sorted(path.glob("*.json")))
        else:
            out.append(path)
    return out


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = parser.add_subparsers(dest="cmd", required=True)

    p_val = sub.add_parser("validate", help="check JSON against engine caps and rules")
    p_val.add_argument("paths", nargs="+")

    p_ren = sub.add_parser("render", help="ASCII view of solids / coins / enemies")
    p_ren.add_argument("paths", nargs="+")
    p_ren.add_argument("--width", type=int, default=None)

    p_exp = sub.add_parser("export-cpp", help="print paste-ready C++ fragments")
    p_exp.add_argument("paths", nargs="+")

    p_st = sub.add_parser("stats", help="one-line census per file")
    p_st.add_argument("paths", nargs="+")

    sub.add_parser("legend", help="tile id table with friction")
    sub.add_parser("jump", help="print the jump / friction budget")

    p_w3 = sub.add_parser("pipes", help="generate world-3 columns from heights")
    p_w3.add_argument("heights", nargs="+", type=int)

    args = parser.parse_args(argv)

    if args.cmd == "legend":
        print(print_legend())
        return 0
    if args.cmd == "jump":
        print(print_jump())
        return 0
    if args.cmd == "pipes":
        tiles = generate_world3(args.heights)
        print(json.dumps(tiles, indent=2))
        return 0

    paths = _paths(args.paths)
    rc = 0
    for path in paths:
        level = load_level(path)
        if args.cmd == "validate":
            found = validate(level)
            if not found:
                print(f"OK  {level['id']}  ({path})")
                continue
            for item in found:
                print(f"{item['severity'].upper():7} {item['level']}  {item['code']}: {item['message']}")
                if item["severity"] == "error":
                    rc = 1
        elif args.cmd == "render":
            print(render_ascii(level, max_width=args.width))
            print()
        elif args.cmd == "export-cpp":
            print(export_cpp(level))
        elif args.cmd == "stats":
            print(format_stats(stats(level)))
        else:
            raise SystemExit(f"unknown command {args.cmd}")
    return rc


if __name__ == "__main__":
    sys.exit(main())
