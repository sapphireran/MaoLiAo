"""ASCII preview of a MaoLiAo world (no EasyX, no bitmaps)."""

from __future__ import annotations

from .collision import tile_rect
from .constants import HEIGHT, WIDTH, XSIZE, YSIZE
from .maps import food_tile, load_tiles, world_payload

ROWS = YSIZE // HEIGHT  # 12


def _glyphs() -> dict[int, str]:
    tiles = load_tiles()["tiles"]
    return {int(k): v["glyph"] for k, v in tiles.items()}


def build_grid(world: int, seed: int | None = 1) -> tuple[list[list[str]], dict]:
    payload = world_payload(world, seed)
    glyphs = _glyphs()
    records = payload["maps_loaded"]
    max_col = 8
    for rec in records:
        factor = 2 if rec["id"] in (8, 10) else 1
        max_col = max(max_col, rec["x"] + rec["xAmount"] * factor + 2)
    for x, _y in payload["coins"]:
        max_col = max(max_col, x + 1)
    for enemy in payload["enemies"]:
        max_col = max(max_col, enemy["x"] + 1)
    grid = [["." for _ in range(max_col)] for _ in range(ROWS)]

    def stamp(col: int, row: int, ch: str) -> None:
        if 0 <= row < ROWS and 0 <= col < max_col:
            grid[row][col] = ch

    for rec in records:
        box = tile_rect(rec["x"], rec["y"], rec["xAmount"], rec["yAmount"], rec["id"])
        ch = glyphs.get(rec["id"], "?")
        for px in range(box.left, box.right, WIDTH):
            for py in range(box.top, box.bottom, HEIGHT):
                stamp(px // WIDTH, py // HEIGHT, ch)

    for x, y in payload["coins"]:
        stamp(x, y, "o")
    for enemy in payload["enemies"]:
        stamp(enemy["x"], enemy["y"], "E")
    for fp in payload["food_pixels"]:
        tx, ty = food_tile(fp)
        stamp(tx, ty, "F")

    # Spawn: Role sets (x, y) = (X0, X0) = (64, 64) → tile (2, 2)
    stamp(2, 2, "@")
    return grid, payload


def render(world: int, seed: int | None = 1, ruler_every: int = 10) -> str:
    grid, payload = build_grid(world, seed)
    cols = len(grid[0])
    lines: list[str] = []
    title = f"world {world} ({payload['name']})  ending at tile {payload['ending_tiles']}"
    if payload["seed"] is not None:
        title += f"  seed={payload['seed']}"
    lines.append(title)
    if payload["maps_truncated"]:
        lines.append(
            f"truncated {len(payload['maps_truncated'])} map records "
            f"(MAP_NUMBER={len(payload['maps_loaded'])} loaded)"
        )
    header = []
    for i in range(cols):
        header.append(str(i % 10) if i % ruler_every == 0 else " ")
    lines.append("    " + "".join(header))
    for y, row in enumerate(grid):
        lines.append(f"{y:02d}  " + "".join(row))
    lines.append("")
    lines.append(
        "legend: @ spawn  # grass  S snow  C cloud  P/= pipe/fill  "
        "u/n mouths  o coin  E enemy  F food  G goal  T tree  ~ water  , grass decor"
    )
    lines.append(
        f"window {XSIZE}x{YSIZE} px  tile {WIDTH}x{HEIGHT}  "
        f"authored maps {len(payload['maps_authored'])}  "
        f"loaded {len(payload['maps_loaded'])}"
    )
    return "\n".join(lines)
