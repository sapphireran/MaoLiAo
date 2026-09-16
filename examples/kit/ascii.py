"""ASCII plan of a world. One character = one tile."""

from __future__ import annotations

from .constants import C
from .worlds import World

GLYPH = {
    1: "=",
    2: "~",
    3: "#",
    4: ".",
    5: "=",
    6: "I",
    7: "|",
    8: "v",
    10: "^",
    11: ",",
    12: "G",
    13: "w",
    14: "T",
}


def render_ascii(world: World, *, max_x: int | None = None, show_actors: bool = True) -> str:
    tiles = world.runtime_tiles
    if not tiles and not world.coins:
        return ""
    max_tx = max((t.x + t.xAmount * (2 if t.id in (8, 10) else 1) for t in tiles), default=8)
    max_ty = max((t.y + t.yAmount * (2 if t.id in (8, 10) else 1) for t in tiles), default=8)
    if world.coins:
        max_tx = max(max_tx, max(c[0] for c in world.coins) + 1)
        max_ty = max(max_ty, max(c[1] for c in world.coins) + 1)
    if max_x is not None:
        max_tx = min(max_tx, max_x)
    grid = [[" " for _ in range(max_tx + 2)] for _ in range(max_ty + 2)]

    def plot(x, y, ch):
        if 0 <= y < len(grid) and 0 <= x < len(grid[0]):
            grid[y][x] = ch

    for t in tiles:
        tw, th = (2, 2) if t.id in (8, 10) else (1, 1)
        ch = GLYPH.get(t.id, "?")
        for j in range(t.xAmount * tw):
            for k in range(t.yAmount * th):
                plot(t.x + j, t.y + k, ch)
    if show_actors:
        for cx, cy in world.coins:
            plot(cx, cy, "o")
        for e in world.enemies:
            plot(int(e.x // C.WIDTH), int(e.y // C.HEIGHT), "e")
        for fx, fy in world.food_pixels:
            plot(int(fx // C.WIDTH), int(fy // C.HEIGHT), "*")
        plot(C.X0 // C.WIDTH, C.X0 // C.HEIGHT, "@")
    lines = [f"world {world.index} {world.title} tiles={len(tiles)}/{world.authored_tile_count}"]
    for y, row in enumerate(grid):
        lines.append(f"{y:2d}|" + "".join(row))
    return "\n".join(lines) + "\n"
