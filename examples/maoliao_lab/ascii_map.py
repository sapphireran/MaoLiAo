"""ASCII dump of a world's solid tiles. Not a renderer — a study aid."""

from __future__ import annotations

from .constants import HEIGHT, YSIZE
from .tiles import TILES

GLYPH = {
    1: "=",
    2: "~",
    3: "#",
    4: ":",
    5: "=",
    6: "=",
    7: "|",
    8: "v",
    9: "^",
    10: "^",
    11: ",",
    12: "G",
    13: "~",
    14: "T",
}

PIPE_WIDE = {7, 8, 10}
PIPE_TALL = {8, 10}


def bounds(tiles: list[dict]) -> tuple[int, int]:
    max_x = 0
    max_y = max(YSIZE // HEIGHT, 12)
    for t in tiles:
        w_cells = t["xAmount"] * (2 if t["id"] in PIPE_WIDE else 1)
        h_cells = t["yAmount"] * (2 if t["id"] in PIPE_TALL else 1)
        max_x = max(max_x, t["x"] + w_cells)
        max_y = max(max_y, t["y"] + h_cells)
    return max_x + 1, max_y + 1


def raster(tiles: list[dict], actors: dict | None = None, *, scenery: bool = True) -> list[str]:
    width, height = bounds(tiles)
    grid = [[" " for _ in range(width)] for _ in range(height)]
    ordered = sorted(tiles, key=lambda t: (TILES.get(t["id"], {}).get("solid", False), t["id"]))
    for t in ordered:
        if not scenery and t["id"] >= 11:
            continue
        glyph = GLYPH.get(t["id"], "?")
        x_span = t["xAmount"] * (2 if t["id"] in PIPE_WIDE else 1)
        y_span = t["yAmount"] * (2 if t["id"] in PIPE_TALL else 1)
        for dy in range(max(y_span, 1)):
            for dx in range(max(x_span, 1)):
                x, y = t["x"] + dx, t["y"] + dy
                if 0 <= y < height and 0 <= x < width:
                    grid[y][x] = glyph
    if actors:
        for cx, cy in actors.get("coins", []):
            if 0 <= cy < height and 0 <= cx < width and grid[cy][cx] == " ":
                grid[cy][cx] = "o"
        for e in actors.get("enemies", []):
            if 0 <= e["y"] < height and 0 <= e["x"] < width:
                grid[e["y"]][e["x"]] = "e"
    return ["".join(row).rstrip() for row in grid]


def render(tiles: list[dict], actors: dict | None = None, *, scenery: bool = True) -> str:
    lines = raster(tiles, actors, scenery=scenery)
    numbered = [f"{y:02d} {line}" for y, line in enumerate(lines)]
    width = max((len(line) for line in numbered), default=3)
    header = "   " + "".join(str(i % 10) for i in range(max(0, width - 3)))
    return "\n".join([header, *numbered])
