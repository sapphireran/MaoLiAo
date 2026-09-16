"""Checks that catch MAP_NUMBER overflow, bad ids, and short runways."""

from __future__ import annotations

from dataclasses import dataclass, asdict

from .constants import (
    COINS_NUMBER,
    ENEMY_TOTE,
    FOOD_NUMBER,
    MAP_NUMBER,
    WIDTH,
)
from .maps import colliding_extent_tiles, world_payload


@dataclass
class Finding:
    world: int
    severity: str
    code: str
    message: str


def lint_world(world: int, seed: int | None = 1) -> list[Finding]:
    payload = world_payload(world, seed)
    findings: list[Finding] = []
    authored = payload["maps_authored"]
    loaded = payload["maps_loaded"]

    if len(authored) > MAP_NUMBER:
        findings.append(
            Finding(
                world,
                "warning",
                "truncated_records",
                f"{len(authored)} Map records authored, MAP_NUMBER={MAP_NUMBER}; "
                f"{len(payload['maps_truncated'])} dropped: "
                + ", ".join(
                    f"id={r['id']}@({r['x']},{r['y']})" for r in payload["maps_truncated"]
                ),
            )
        )

    for rec in authored:
        if rec["id"] < 1 or rec["id"] > 14:
            findings.append(
                Finding(world, "error", "bad_tile_id", f"id {rec['id']} at ({rec['x']},{rec['y']})")
            )
        if rec["xAmount"] < 0 or rec["yAmount"] < 0:
            findings.append(
                Finding(world, "error", "negative_amount", f"{rec}")
            )

    if len(payload["coins"]) > COINS_NUMBER:
        findings.append(
            Finding(
                world,
                "error",
                "coin_overflow",
                f"{len(payload['coins'])} coins > COINS_NUMBER={COINS_NUMBER}",
            )
        )
    if len(payload["enemies"]) > ENEMY_TOTE:
        findings.append(
            Finding(
                world,
                "error",
                "enemy_overflow",
                f"{len(payload['enemies'])} enemies > ENEMY_TOTE={ENEMY_TOTE}",
            )
        )
    if len(payload["food_pixels"]) > FOOD_NUMBER:
        findings.append(
            Finding(
                world,
                "error",
                "food_overflow",
                f"{len(payload['food_pixels'])} food > FOOD_NUMBER={FOOD_NUMBER}",
            )
        )

    extent = colliding_extent_tiles(loaded)
    ending = payload["ending_tiles"]
    if extent < ending:
        findings.append(
            Finding(
                world,
                "warning",
                "short_runway",
                f"colliding tiles end near column {extent}, but isEnding fires at {ending}",
            )
        )

    for fp in payload["food_pixels"]:
        if fp[0] < WIDTH and fp[1] < HEIGHT:
            findings.append(
                Finding(
                    world,
                    "warning",
                    "food_near_origin",
                    f"food at pixels {fp} is inside the first tile (world 3 uses {{10,10}})",
                )
            )

    seen = set()
    for rec in loaded:
        key = (rec["x"], rec["y"], rec["id"])
        if key in seen:
            findings.append(
                Finding(world, "info", "duplicate_map_cell", f"repeat {key}")
            )
        seen.add(key)

    return findings


def lint_all(seed: int | None = 1) -> list[Finding]:
    out: list[Finding] = []
    for world in (1, 2, 3):
        out.extend(lint_world(world, seed))
    return out


def findings_as_dicts(items: list[Finding]) -> list[dict]:
    return [asdict(item) for item in items]
