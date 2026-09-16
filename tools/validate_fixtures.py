#!/usr/bin/env python3
"""Compare fixtures/ against MaoLiAo C++ literals and ASCII goldens."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from maoliao_fixtures import (  # noqa: E402
    FIXTURES,
    MAP_CAP,
    ROOT,
    SRC,
    almost,
    classify_save,
    derived_from_macros,
    dropped_tiles,
    extract_ending_tiles,
    extract_enemies,
    extract_map_tiles,
    extract_points,
    level_payload,
    load_json,
    materialize_world3,
    parse_define_h,
    render_ascii,
    runtime_tiles,
    tile_eq,
)


class Report:
    def __init__(self, quiet: bool = False) -> None:
        self.quiet = quiet
        self.passed = 0
        self.failures: list[str] = []

    def ok(self, name: str) -> None:
        self.passed += 1
        if not self.quiet:
            print(f"  pass  {name}")

    def fail(self, name: str, detail: str) -> None:
        self.failures.append(f"{name}: {detail}")
        print(f"  FAIL  {name}: {detail}")

    def check(self, name: str, cond: bool, detail: str = "") -> None:
        if cond:
            self.ok(name)
        else:
            self.fail(name, detail or "assertion failed")


def _diff_maps(label: str, got: list[dict], exp: list[dict], report: Report) -> None:
    if len(got) != len(exp):
        report.fail(label, f"count {len(got)} != {len(exp)}")
        return
    for i, (a, b) in enumerate(zip(got, exp)):
        if a != b and not (set(a) >= {"x", "y"} and a.get("x") == b.get("x") and a.get("y") == b.get("y") and a.get("id", b.get("id")) == b.get("id") and a.get("turn", b.get("turn")) == b.get("turn") and a.get("xAmount", b.get("xAmount")) == b.get("xAmount") and a.get("yAmount", b.get("yAmount")) == b.get("yAmount")):
            report.fail(label, f"index {i}: source {a} != fixture {b}")
            return
    report.ok(label)


def check_physics(report: Report) -> None:
    macros = parse_define_h()
    doc = load_json(FIXTURES / "physics.json")
    for name, value in doc["macros"].items():
        if name not in macros:
            report.fail(f"physics.{name}", "missing in define.h")
            continue
        report.check(
            f"physics.{name}",
            almost(float(value), float(macros[name])),
            f"define.h={macros[name]} fixture={value}",
        )
    derived = derived_from_macros(macros)
    fr = doc["derived"]["friction"]
    for band in ("high", "medium", "low"):
        report.check(
            f"friction.{band}",
            almost(derived[band], fr[band]),
            f"computed={derived[band]} fixture={fr[band]}",
        )
    report.check(
        "jump_v0",
        almost(derived["jump_v0"], doc["derived"]["jump_v0"]),
        f"{derived['jump_v0']} != {doc['derived']['jump_v0']}",
    )
    report.check(
        "pixel_scale",
        almost(derived["pixel_scale"], doc["derived"]["pixel_scale"]),
        f"{derived['pixel_scale']} != {doc['derived']['pixel_scale']}",
    )
    endings = extract_ending_tiles()
    for world, tiles in doc["derived"]["ending_distance_tiles"].items():
        report.check(
            f"ending.world-{world}",
            endings.get(int(world)) == tiles,
            f"source={endings.get(int(world))} fixture={tiles}",
        )
    for name, bit in doc["key_bits"].items():
        report.check(f"key.{name}", almost(macros[name], bit), f"{macros.get(name)} != {bit}")
    report.check("MAP_NUMBER cap", doc["caps"]["MAP_NUMBER"] == MAP_CAP, str(doc["caps"]["MAP_NUMBER"]))


def check_tiles_catalog(report: Report) -> None:
    catalog = {t["id"]: t for t in load_json(FIXTURES / "tiles.json")["tiles"]}
    report.check("tile ids 1-14", set(catalog) == set(range(1, 15)), str(sorted(catalog)))
    for i in range(1, 15):
        t = catalog[i]
        report.check(f"tile-{i}.name", bool(t.get("name")), "empty name")
        report.check(f"tile-{i}.collides-bool", isinstance(t.get("collides"), bool), str(t.get("collides")))


def check_authored_world(world: int, report: Report) -> None:
    macros = parse_define_h()
    doc = load_json(FIXTURES / "levels" / f"world-{world}.json")
    src_tiles = extract_map_tiles(world)
    src_coins = extract_points("void Scene::createCoin", world, macros)
    src_food = extract_points("void Scene::createFood", world, macros)
    src_enemies = extract_enemies(world)

    _diff_maps(f"w{world}.tiles", src_tiles, doc["authored_tiles"], report)
    _diff_maps(f"w{world}.coins", src_coins, doc["coins"], report)
    _diff_maps(f"w{world}.enemies", src_enemies, doc["enemies"], report)

    food_src = [{"x": p["x"], "y": p["y"]} for p in src_food]
    food_fix = [{"x": p["x"], "y": p["y"]} for p in doc["food"]]
    _diff_maps(f"w{world}.food", food_src, food_fix, report)

    authored = doc["authored_tiles"]
    runtime = runtime_tiles(authored)
    dropped = dropped_tiles(authored)
    report.check(
        f"w{world}.runtime-len",
        len(runtime) == min(len(authored), MAP_CAP),
        f"{len(runtime)} authored={len(authored)}",
    )
    report.check(
        f"w{world}.dropped-len",
        len(dropped) == max(0, len(authored) - MAP_CAP),
        f"{len(dropped)}",
    )
    if world == 1:
        report.check("w1.authored-32", len(authored) == 32, str(len(authored)))
        report.check("w1.dropped-water", len(dropped) == 2 and all(t["id"] == 13 for t in dropped), str(dropped))
    if world == 2:
        report.check("w2.authored-36", len(authored) == 36, str(len(authored)))
        report.check(
            "w2.dropped-includes-flag",
            any(t["id"] == 12 for t in dropped),
            "victory sign still inside runtime cap",
        )
        report.check(
            "w2.dropped-trees",
            sum(1 for t in dropped if t["id"] == 14) == 4,
            str(dropped),
        )

    catalog = {t["id"] for t in load_json(FIXTURES / "tiles.json")["tiles"]}
    for i, t in enumerate(authored):
        report.check(f"w{world}.tile[{i}].id", t["id"] in catalog, str(t["id"]))
        for key in ("x", "y", "xAmount", "yAmount"):
            report.check(f"w{world}.tile[{i}].{key}-int", isinstance(t[key], int), str(t[key]))


def check_world3(report: Report) -> None:
    macros = parse_define_h()
    doc = load_json(FIXTURES / "levels" / "world-3.json")
    gen = doc["generation"]
    example = doc["example"]
    report.check("w3.column-used", gen["column_count_used"] == 7, str(gen["column_count_used"]))
    report.check("w3.height-range", gen["height_min"] == 1 and gen["height_max"] == 6, str(gen))
    report.check("w3.coin-y-range", gen["coin_y_min"] == 3 and gen["coin_y_max"] == 6, str(gen))
    heights = example["pipe_heights"]
    report.check("w3.example-7-heights", len(heights) == 7, str(heights))
    report.check(
        "w3.example-heights-in-range",
        all(gen["height_min"] <= h <= gen["height_max"] for h in heights),
        str(heights),
    )
    rebuilt = materialize_world3(heights, example["coin_heights"], gen)
    if len(rebuilt["tiles"]) != len(example["tiles"]):
        report.fail("w3.materialize.count", f"{len(rebuilt['tiles'])} != {len(example['tiles'])}")
    else:
        bad = None
        for i, (a, b) in enumerate(zip(rebuilt["tiles"], example["tiles"])):
            if not tile_eq(a, b):
                bad = f"index {i}: {a} != {b}"
                break
        if bad:
            report.fail("w3.materialize.tiles", bad)
        else:
            report.ok("w3.materialize.tiles")
    _diff_maps("w3.materialize.coins", rebuilt["coins"], example["coins"], report)
    report.check("w3.tile-count-30", len(example["tiles"]) == 30, str(len(example["tiles"])))

    src_enemies = extract_enemies(3)
    _diff_maps("w3.enemies", src_enemies, example["enemies"], report)
    src_food = extract_points("void Scene::createFood", 3, macros)
    _diff_maps("w3.food", src_food, [{"x": p["x"], "y": p["y"]} for p in example["food"]], report)
    report.check("w3.food-is-debug-px", example["food"][0]["x"] == 10 and example["food"][0]["y"] == 10, str(example["food"]))


def check_ascii(report: Report) -> None:
    for world in (1, 2, 3):
        data = level_payload(world)
        got = render_ascii(data["tiles"], data["coins"], data["enemies"], data["food"])
        path = FIXTURES / "expected" / f"world-{world}.ascii.txt"
        if not path.exists():
            report.fail(f"ascii.w{world}", f"missing {path} (run render_level.py --write-expected)")
            continue
        exp = path.read_text(encoding="utf-8")
        if got != exp:
            report.fail(f"ascii.w{world}", f"golden mismatch ({len(got)} vs {len(exp)} chars)")
        else:
            report.ok(f"ascii.w{world}")
            report.check(f"ascii.w{world}.has-tiles", any(ch in got for ch in "#C=~P^v"), "empty silhouette")


def check_saves(report: Report, list_saves: bool) -> None:
    manifest = load_json(FIXTURES / "saves" / "manifest.json")
    for entry in manifest["files"]:
        path = FIXTURES / "saves" / entry["file"]
        if not path.exists():
            report.fail(f"save.{entry['file']}", "file missing")
            continue
        raw = path.read_bytes()
        cls, value = classify_save(raw)
        if list_saves:
            print(f"    {entry['file']:24} {cls:18} {value!r}  bytes={raw!r}")
        report.check(
            f"save.{entry['file']}.class",
            cls == entry["class"],
            f"got {cls} expected {entry['class']} raw={raw!r}",
        )
        if entry["class"] == "valid":
            report.check(f"save.{entry['file']}.world", value == entry["world"], f"{value}")

    live = (SRC / "gameRecord.dat").read_bytes()
    cls, value = classify_save(live)
    report.check("checked-in-save-valid-world-3", cls == "valid" and value == 3, f"{cls} {value} {live!r}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--quiet", action="store_true")
    parser.add_argument("--list-saves", action="store_true")
    args = parser.parse_args()
    report = Report(quiet=args.quiet)

    if not (SRC / "define.h").exists():
        print("MaoLiAo/define.h missing", file=sys.stderr)
        return 2

    print("physics")
    check_physics(report)
    print("tiles")
    check_tiles_catalog(report)
    print("world 1")
    check_authored_world(1, report)
    print("world 2")
    check_authored_world(2, report)
    print("world 3")
    check_world3(report)
    print("ascii")
    check_ascii(report)
    print("saves")
    check_saves(report, args.list_saves)

    print()
    if report.failures:
        print(f"FAILED: {len(report.failures)} failed, {report.passed} passed.")
        for item in report.failures:
            print(f"  - {item}")
        return 1
    print(f"OK: {report.passed} checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
