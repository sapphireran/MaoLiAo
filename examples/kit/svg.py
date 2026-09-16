"""SVG plan view. No third-party packages."""

from __future__ import annotations

from .constants import C
from .worlds import World

FILL = {
    1: "#3d9e4a",
    2: "#e8f4ff",
    3: "#8b5a2b",
    4: "#d9e6f2",
    5: "#f4f8fc",
    6: "#8ecae6",
    7: "#2d6a4f",
    8: "#1b4332",
    10: "#1b4332",
    11: "#74c69d",
    12: "#f4d35e",
    13: "#4ea8de",
    14: "#40916c",
}
STROKE = {
    2: "#9bb8cc",
    5: "#c5d4e0",
    6: "#219ebc",
    12: "#b08900",
}


def render_svg(world: World, *, scale: int = 8, cap: bool = True) -> str:
    tiles = world.runtime_tiles if cap else world.tiles
    max_x = 8
    max_y = 12
    for t in tiles:
        tw, th = (2, 2) if t.id in (8, 10) else (1, 1)
        max_x = max(max_x, t.x + t.xAmount * tw)
        max_y = max(max_y, t.y + t.yAmount * th)
    for cx, cy in world.coins:
        max_x = max(max_x, cx + 1)
        max_y = max(max_y, cy + 1)
    w = (max_x + 2) * scale
    h = (max_y + 2) * scale
    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" '
        f'viewBox="0 0 {w} {h}" font-family="monospace">',
        f'<rect width="{w}" height="{h}" fill="#1b2838"/>',
        f'<text x="8" y="14" fill="#e0e6ed" font-size="12">'
        f"world {world.index} {world.title}  "
        f"tiles {len(tiles)}/{world.authored_tile_count}</text>",
    ]

    def rect(tx, ty, tw, th, fill, stroke="#111", extra=""):
        parts.append(
            f'<rect x="{tx * scale}" y="{ty * scale}" width="{tw * scale}" '
            f'height="{th * scale}" fill="{fill}" stroke="{stroke}" '
            f'stroke-width="0.6" {extra}/>'
        )

    for t in tiles:
        tw, th = (2, 2) if t.id in (8, 10) else (1, 1)
        fill = FILL.get(t.id, "#888")
        stroke = STROKE.get(t.id, "#111")
        for j in range(t.xAmount * tw):
            for k in range(t.yAmount * th):
                rect(t.x + j, t.y + k, 1, 1, fill, stroke)
    for cx, cy in world.coins:
        parts.append(
            f'<circle cx="{(cx + 0.5) * scale}" cy="{(cy + 0.5) * scale}" '
            f'r="{scale * 0.28}" fill="#ffd166" stroke="#c9a227"/>'
        )
    for e in world.enemies:
        ex = e.x / C.WIDTH
        ey = e.y / C.HEIGHT
        parts.append(
            f'<rect x="{ex * scale + 1}" y="{ey * scale + 1}" '
            f'width="{scale - 2}" height="{scale - 2}" fill="#e63946" rx="1"/>'
        )
    for fx, fy in world.food_pixels:
        rect(fx / C.WIDTH, fy / C.HEIGHT, 1.5, 0.8, "#9b5de5", "#5a189a")
    rect(C.X0 / C.WIDTH, C.X0 / C.HEIGHT, 1, 1, "#00bbf9", "#0077b6")
    end_x = world.ending_px / C.WIDTH
    parts.append(
        f'<line x1="{end_x * scale}" y1="0" x2="{end_x * scale}" y2="{h}" '
        f'stroke="#ff9f1c" stroke-dasharray="4 3" stroke-width="1"/>'
    )
    parts.append("</svg>")
    return "\n".join(parts) + "\n"


def render_jump_svg(width: int = 640, height: int = 240) -> str:
    from .kinematics import apex, jump_profile

    samples = jump_profile()
    top = apex(samples)
    pad = 28
    max_t = samples[-1].t
    max_y = 110
    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" '
        f'viewBox="0 0 {width} {height}" font-family="monospace">',
        f'<rect width="{width}" height="{height}" fill="#0f1a24"/>',
        f'<text x="12" y="18" fill="#e0e6ed" font-size="13">'
        f"unobstructed jump  apex {top.rise_px:.3f} px @ frame {top.frame}</text>",
    ]
    y101 = pad + (1 - 101 / max_y) * (height - 2 * pad)
    parts.append(
        f'<line x1="{pad}" y1="{y101:.1f}" x2="{width - pad}" y2="{y101:.1f}" '
        f'stroke="#f4d35e" stroke-dasharray="5 4" stroke-width="1"/>'
    )
    pts = []
    for s in samples:
        px = pad + (s.t / max_t) * (width - 2 * pad)
        py = pad + (1 - s.rise_px / max_y) * (height - 2 * pad)
        pts.append(f"{px:.1f},{py:.1f}")
    parts.append(
        f'<polyline fill="none" stroke="#00bbf9" stroke-width="2" points="{" ".join(pts)}"/>'
    )
    parts.append(
        f'<text x="{pad}" y="{height - 8}" fill="#8d99ae" font-size="11">'
        f"0 s — {max_t:.2f} s   dashed = UNREAL_HEIGHT 101</text>"
    )
    parts.append("</svg>")
    return "\n".join(parts) + "\n"
