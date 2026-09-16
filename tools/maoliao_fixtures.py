#!/usr/bin/env python3
"""Shared helpers for MaoLiAo fixture extract / render / validate."""

from __future__ import annotations

import json
import math
import re
from pathlib import Path
from typing import Any, Iterable

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "MaoLiAo"
FIXTURES = ROOT / "fixtures"

MAP_CAP = 30

ASCII_BY_ID = {
    1: "#",
    2: "C",
    3: "~",
    4: "~",
    5: "=",
    6: "P",
    7: "v",
    8: "v",
    10: "^",
}


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def strip_cpp_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    out = []
    for line in text.splitlines():
        out.append(re.sub(r"//.*", "", line))
    return "\n".join(out)


def _eval_cpp_number(expr: str, macros: dict[str, float]) -> float:
    expr = expr.strip().rstrip(";").replace(" ", "")
    if not expr:
        raise ValueError("empty macro")
    # Only arithmetic we expect from define.h / food literals.
    allowed = set("0123456789.+-*/()_")
    tokens = re.findall(r"[A-Z_][A-Z0-9_]*|[0-9]*\.?[0-9]+|[+\-*/()]", expr)
    rebuilt = []
    for tok in tokens:
        if re.fullmatch(r"[A-Z_][A-Z0-9_]*", tok):
            if tok not in macros:
                raise ValueError(f"unknown macro {tok} in {expr}")
            rebuilt.append(str(macros[tok]))
        else:
            if any(ch not in allowed for ch in tok):
                raise ValueError(f"bad token {tok}")
            rebuilt.append(tok)
    joined = "".join(rebuilt)
    if not re.fullmatch(r"[0-9.+\-*/()]+", joined):
        raise ValueError(f"refused to eval {expr} -> {joined}")
    return float(eval(joined, {"__builtins__": {}}, {}))  # noqa: S307 — guarded charset


def parse_define_h(text: str | None = None) -> dict[str, float]:
    raw = text if text is not None else (SRC / "define.h").read_text(encoding="utf-8", errors="replace")
    macros: dict[str, float] = {}
    pending: list[tuple[str, str]] = []
    for match in re.finditer(r"^#define\s+([A-Z_][A-Z0-9_]*)(?:[ \t]+(\S.*?))?\s*$", raw, re.M):
        name, rhs = match.group(1), (match.group(2) or "").strip()
        if not rhs or name in {"MYDEFINE", "MYDIFINE", "max", "random"}:
            continue
        if name.startswith("CMD_") or name.startswith("VIR_"):
            macros[name] = float(rhs.split()[0])
            continue
        rhs = rhs.split("//")[0].strip().rstrip(";")
        pending.append((name, rhs))
    # First pass: literals
    rest = []
    for name, rhs in pending:
        try:
            macros[name] = float(rhs)
        except ValueError:
            rest.append((name, rhs))
    # Resolve expressions that only need already-known macros.
    changed = True
    while rest and changed:
        changed = False
        still = []
        for name, rhs in rest:
            try:
                macros[name] = _eval_cpp_number(rhs, macros)
                changed = True
            except ValueError:
                still.append((name, rhs))
        rest = still
    if rest:
        raise ValueError(f"unresolved macros: {rest}")
    return macros


def _brace_groups(blob: str) -> list[str]:
    """Split a `{ a, b }, { c, d }` list into inner strings."""
    groups = []
    depth = 0
    start = None
    for i, ch in enumerate(blob):
        if ch == "{":
            if depth == 0:
                start = i + 1
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0 and start is not None:
                groups.append(blob[start:i])
                start = None
    return groups


def _nums(inner: str) -> list[float]:
    parts = [p.strip() for p in inner.split(",") if p.strip()]
    out = []
    for p in parts:
        out.append(float(p) if re.search(r"[.eE]", p) else int(p))
    return out


def _function_body(text: str, signature: str) -> str:
    idx = text.find(signature)
    if idx < 0:
        raise ValueError(f"missing {signature}")
    idx = text.find("{", idx)
    depth = 0
    for j in range(idx, len(text)):
        if text[j] == "{":
            depth += 1
        elif text[j] == "}":
            depth -= 1
            if depth == 0:
                return text[idx + 1 : j]
    raise ValueError(f"unbalanced {signature}")


def _world_branch(body: str, world: int) -> str:
    if world == 1:
        start = body.find("if (world == 1)")
    else:
        start = body.find(f"else if (world == {world})")
    if start < 0:
        raise ValueError(f"no world {world} branch")
    start = body.find("{", start)
    depth = 0
    for j in range(start, len(body)):
        if body[j] == "{":
            depth += 1
        elif body[j] == "}":
            depth -= 1
            if depth == 0:
                return body[start + 1 : j]
    raise ValueError(f"unbalanced world {world}")


def extract_map_tiles(world: int) -> list[dict[str, int]]:
    if world == 3:
        raise ValueError("world 3 maps are procedural")
    text = strip_cpp_comments((SRC / "scene.cpp").read_text(encoding="utf-8", errors="replace"))
    body = _function_body(text, "void Scene::createMap")
    branch = _world_branch(body, world)
    m = re.search(r"Map\s+m\[\s*\]\s*=\s*\{", branch)
    if not m:
        raise ValueError(f"no Map m[] in world {world}")
    blob = branch[m.end() :]
    # cut at the matching close of the initializer
    depth = 1
    end = 0
    for i, ch in enumerate(blob):
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                end = i
                break
    tiles = []
    for inner in _brace_groups(blob[:end]):
        nums = [int(float(n)) for n in _nums(inner)]
        if len(nums) < 5:
            continue
        tiles.append(
            {
                "x": nums[0],
                "y": nums[1],
                "id": nums[2],
                "xAmount": nums[3],
                "yAmount": nums[4],
            }
        )
    return tiles


def extract_points(fn_sig: str, world: int, macros: dict[str, float]) -> list[dict[str, int]]:
    text = strip_cpp_comments((SRC / "scene.cpp").read_text(encoding="utf-8", errors="replace"))
    body = _function_body(text, fn_sig)
    branch = _world_branch(body, world)
    m = re.search(r"POINT\s+p\[\s*\]\s*=\s*\{", branch)
    if not m:
        raise ValueError(f"no POINT p[] in {fn_sig} world {world}")
    blob = branch[m.end() :]
    depth = 1
    end = 0
    for i, ch in enumerate(blob):
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                end = i
                break
    points = []
    for inner in _brace_groups(blob[:end]):
        parts = [p.strip() for p in inner.split(",") if p.strip()]
        if len(parts) < 2:
            continue
        x = int(_eval_cpp_number(parts[0], macros))
        y = int(_eval_cpp_number(parts[1], macros))
        points.append({"x": x, "y": y})
    return points


def extract_enemies(world: int) -> list[dict[str, int]]:
    text = strip_cpp_comments((SRC / "role.cpp").read_text(encoding="utf-8", errors="replace"))
    body = _function_body(text, "void Role::createEnemy")
    branch = _world_branch(body, world)
    m = re.search(r"Enemy\s+emy\[\s*\]\s*=\s*\{", branch)
    if not m:
        raise ValueError(f"no Enemy emy[] in world {world}")
    blob = branch[m.end() :]
    depth = 1
    end = 0
    for i, ch in enumerate(blob):
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                end = i
                break
    enemies = []
    for inner in _brace_groups(blob[:end]):
        nums = [int(float(n)) for n in _nums(inner)]
        if len(nums) < 3:
            continue
        enemies.append({"x": nums[0], "y": nums[1], "turn": nums[2]})
    return enemies


def extract_ending_tiles() -> dict[int, int]:
    text = strip_cpp_comments((SRC / "scene.cpp").read_text(encoding="utf-8", errors="replace"))
    body = _function_body(text, "bool Scene::isEnding")
    found = {}
    for world, dist in re.findall(r"case\s+(\d+):\s*if\s*\(\s*distance\s*>\s*(\d+)\s*\*\s*WIDTH", body):
        found[int(world)] = int(dist)
    return found


def materialize_world3(pipe_heights: Iterable[int], coin_heights: Iterable[int], gen: dict) -> dict:
    tiles = []
    for i, height in enumerate(pipe_heights):
        x = i * 10 + 10
        tiles.append({"x": x, "y": height - 2, "id": 8, "xAmount": 1, "yAmount": 1})
        tiles.append({"x": x, "y": 0, "id": 7, "xAmount": 1, "yAmount": max(0, height - 2)})
        tiles.append({"x": x, "y": 4 + height, "id": 10, "xAmount": 1, "yAmount": 1})
        tiles.append({"x": x, "y": height + 6, "id": 7, "xAmount": 1, "yAmount": max(0, 6 - height)})
    tiles.append(dict(gen["runway"]))
    tiles.append(dict(gen["flag"]))
    coins = [{"x": k * 10 + 5, "y": h} for k, h in enumerate(coin_heights)]
    return {"tiles": tiles, "coins": coins}


def runtime_tiles(authored: list[dict]) -> list[dict]:
    return authored[:MAP_CAP]


def dropped_tiles(authored: list[dict]) -> list[dict]:
    return authored[MAP_CAP:]


def tile_eq(a: dict, b: dict) -> bool:
    return all(a[k] == b[k] for k in ("x", "y", "id", "xAmount", "yAmount"))


def render_ascii(
    tiles: list[dict],
    coins: list[dict] | None = None,
    enemies: list[dict] | None = None,
    food: list[dict] | None = None,
    width: int | None = None,
    height: int | None = None,
) -> str:
    cells: dict[tuple[int, int], str] = {}
    max_x = 0
    max_y = 0
    min_y = 0
    for t in tiles:
        if t["id"] >= 11 or t["id"] == 9:
            continue
        ch = ASCII_BY_ID.get(t["id"])
        if not ch or t["yAmount"] <= 0:
            continue
        # Pipe mouths occupy 2x2 collision cells.
        x_span = t["xAmount"] * (2 if t["id"] in (8, 10) else 1)
        y_span = t["yAmount"] * (2 if t["id"] in (8, 10) else 1)
        if t["id"] == 7:
            x_span = max(1, t["xAmount"] * 2)
        for dx in range(x_span):
            for dy in range(y_span):
                x = t["x"] + dx
                y = t["y"] + dy
                cells[(x, y)] = ch
                max_x = max(max_x, x)
                max_y = max(max_y, y)
                min_y = min(min_y, y)
    for c in coins or []:
        cells[(c["x"], c["y"])] = "o"
        max_x = max(max_x, c["x"])
        max_y = max(max_y, c["y"])
        min_y = min(min_y, c["y"])
    for e in enemies or []:
        cells[(e["x"], e["y"])] = "e" if e["turn"] > 0 else "E"
        max_x = max(max_x, e["x"])
        max_y = max(max_y, e["y"])
        min_y = min(min_y, e["y"])
    for f in food or []:
        if f.get("unit") == "px":
            tx, ty = f["x"] // 32, f["y"] // 32
        else:
            tx, ty = f["x"], f["y"]
        cells[(tx, ty)] = "*"
        max_x = max(max_x, tx)
        max_y = max(max_y, ty)
        min_y = min(min_y, ty)
    if width is None:
        width = max_x + 2
    if height is None:
        height = max(max_y + 1, 12)
        if min_y < 0:
            # Keep negative rows (world 3 mouth at y=-1) in the dump.
            pass
    y0 = min(0, min_y)
    lines = [
        f"# width={width} y={y0}..{height - 1}  #=grass C=cloud ~=under ==snow P=pipe v=down ^=up o=coin e/E=enemy *=flower"
    ]
    for y in range(y0, height):
        row = []
        for x in range(width):
            row.append(cells.get((x, y), "."))
        lines.append(f"{y:3d} |" + "".join(row) + "|")
    return "\n".join(lines) + "\n"


def level_payload(world: int) -> dict:
    if world == 3:
        doc = load_json(FIXTURES / "levels" / "world-3.json")
        return {
            "tiles": doc["example"]["tiles"],
            "coins": doc["example"]["coins"],
            "enemies": doc["example"]["enemies"],
            "food": doc["example"]["food"],
        }
    doc = load_json(FIXTURES / "levels" / f"world-{world}.json")
    return {
        "tiles": runtime_tiles(doc["authored_tiles"]),
        "coins": doc["coins"],
        "enemies": doc["enemies"],
        "food": doc["food"],
    }


def classify_save(raw: bytes) -> tuple[str, int | None]:
    if raw == b"":
        return "undefined-parse", None
    try:
        text = raw.decode("ascii")
    except UnicodeDecodeError:
        return "undefined-parse", None
    if not re.fullmatch(r"\s*-?\d+\s*", text):
        return "undefined-parse", None
    value = int(text.strip())
    if value <= 0 or value >= 4:
        return "missing-archive", value
    return "valid", value


def derived_from_macros(macros: dict[str, float]) -> dict[str, float]:
    g = macros["G"]
    vmax = macros["V_MAX"]
    return {
        "high": (vmax / macros["T1"]) / g,
        "medium": (vmax / macros["T2"]) / g,
        "low": (vmax / macros["T3"]) / g,
        "jump_v0": -math.sqrt(2 * g * macros["REAL_HEIGHT"]),
        "pixel_scale": macros["UNREAL_HEIGHT"] / macros["REAL_HEIGHT"],
    }


def almost(a: float, b: float, eps: float = 1e-9) -> bool:
    return abs(a - b) <= eps * max(1.0, abs(a), abs(b))
