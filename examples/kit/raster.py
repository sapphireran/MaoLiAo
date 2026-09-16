"""Minimal RGB PNG writer (stdlib only) for plan-view artifacts."""

from __future__ import annotations

import struct
import zlib
from pathlib import Path

from .constants import C
from .worlds import World

PALETTE = {
    1: (61, 158, 74),
    2: (232, 244, 255),
    3: (139, 90, 43),
    4: (217, 230, 242),
    5: (244, 248, 252),
    6: (142, 202, 230),
    7: (45, 106, 79),
    8: (27, 67, 50),
    10: (27, 67, 50),
    11: (116, 198, 157),
    12: (244, 211, 94),
    13: (78, 168, 222),
    14: (64, 145, 108),
}


def _png(rgb_rows: list[list[tuple[int, int, int]]]) -> bytes:
    h = len(rgb_rows)
    w = len(rgb_rows[0])
    raw = b"".join(
        b"\x00" + bytes(c for pix in row for c in pix) for row in rgb_rows
    )

    def chunk(tag: bytes, data: bytes) -> bytes:
        return (
            struct.pack(">I", len(data))
            + tag
            + data
            + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)
        )

    ihdr = struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0)
    return b"".join(
        [
            b"\x89PNG\r\n\x1a\n",
            chunk(b"IHDR", ihdr),
            chunk(b"IDAT", zlib.compress(raw, 9)),
            chunk(b"IEND", b""),
        ]
    )


def render_png(world: World, path: str | Path, *, scale: int = 8, cap: bool = True) -> None:
    tiles = world.runtime_tiles if cap else world.tiles
    max_x = 16
    max_y = 12
    for t in tiles:
        tw, th = (2, 2) if t.id in (8, 10) else (1, 1)
        max_x = max(max_x, t.x + t.xAmount * tw + 2)
        max_y = max(max_y, t.y + t.yAmount * th + 2)
    w, h = max_x * scale, max_y * scale
    bg = (15, 26, 36)
    rows = [[bg] * w for _ in range(h)]

    def fill(tx, ty, tw, th, color):
        x0, y0 = int(tx * scale), int(ty * scale)
        x1, y1 = int((tx + tw) * scale), int((ty + th) * scale)
        for y in range(max(0, y0), min(h, y1)):
            row = rows[y]
            for x in range(max(0, x0), min(w, x1)):
                row[x] = color

    for t in tiles:
        tw, th = (2, 2) if t.id in (8, 10) else (1, 1)
        color = PALETTE.get(t.id, (128, 128, 128))
        for j in range(t.xAmount * tw):
            for k in range(t.yAmount * th):
                fill(t.x + j, t.y + k, 1, 1, color)
    for cx, cy in world.coins:
        fill(cx + 0.25, cy + 0.25, 0.5, 0.5, (255, 209, 102))
    for e in world.enemies:
        fill(e.x / C.WIDTH + 0.15, e.y / C.HEIGHT + 0.15, 0.7, 0.7, (230, 57, 70))
    for fx, fy in world.food_pixels:
        fill(fx / C.WIDTH, fy / C.HEIGHT, 1.4, 0.8, (155, 93, 229))
    fill(C.X0 / C.WIDTH, C.X0 / C.HEIGHT, 1, 1, (0, 187, 249))
    Path(path).write_bytes(_png(rows))


def render_jump_png(path: str | Path, width: int = 640, height: int = 240) -> None:
    from .kinematics import apex, jump_profile

    samples = jump_profile()
    top = apex(samples)
    bg = (15, 26, 36)
    line = (0, 187, 249)
    ref = (244, 211, 94)
    rows = [[bg] * width for _ in range(height)]
    pad = 28
    max_t = samples[-1].t
    max_y = 110.0

    def plot(x, y, color):
        if 0 <= y < height and 0 <= x < width:
            rows[y][x] = color

    y101 = int(pad + (1 - 101 / max_y) * (height - 2 * pad))
    for x in range(pad, width - pad):
        if (x // 5) % 2 == 0:
            plot(x, y101, ref)
    prev = None
    for s in samples:
        px = int(pad + (s.t / max_t) * (width - 2 * pad))
        py = int(pad + (1 - s.rise_px / max_y) * (height - 2 * pad))
        if prev:
            x0, y0 = prev
            steps = max(abs(px - x0), abs(py - y0), 1)
            for i in range(steps + 1):
                plot(x0 + (px - x0) * i // steps, y0 + (py - y0) * i // steps, line)
        prev = (px, py)
        plot(px, py, line)
    Path(path).write_bytes(_png(rows))
    _ = top
