"""Gap list vs the unobstructed jump envelope."""

from __future__ import annotations

from dataclasses import dataclass

from .collision import Tile
from .constants import C
from .kinematics import jump_profile
from .worlds import World


def _solid_cells(tiles: list[Tile]) -> set[tuple[int, int]]:
    cells: set[tuple[int, int]] = set()
    for t in tiles:
        if not t.solid:
            continue
        tw, th = (2, 2) if t.id in (8, 10) else (1, 1)
        for j in range(t.xAmount * tw):
            for k in range(t.yAmount * th):
                cells.add((t.x + j, t.y + k))
    return cells


def solid_spans(world: World, y: int) -> list[tuple[int, int]]:
    """Inclusive [x0, x1] tile ranges that have a solid at row y."""
    xs = sorted({x for (x, ty) in _solid_cells(world.runtime_tiles) if ty == y})
    if not xs:
        return []
    spans = []
    start = prev = xs[0]
    for x in xs[1:]:
        if x == prev + 1:
            prev = x
        else:
            spans.append((start, prev))
            start = prev = x
    spans.append((start, prev))
    return spans


@dataclass(frozen=True)
class Gap:
    y: int
    left_tile: int
    right_tile: int
    width_px: int
    jumpable_at_vmax: bool
    note: str


def gaps(world: World) -> list[Gap]:
    """Gaps on rows that actually carry walkable tops (solid with air above)."""
    cells = _solid_cells(world.runtime_tiles)
    tops: dict[int, set[int]] = {}
    for x, y in cells:
        if (x, y - 1) not in cells:
            tops.setdefault(y, set()).add(x)
    env = jump_envelope()
    out: list[Gap] = []
    for y, xs in sorted(tops.items()):
        ordered = sorted(xs)
        for a, b in zip(ordered, ordered[1:]):
            if b == a + 1:
                continue
            width = (b - (a + 1)) * C.WIDTH
            jumpable = width < env
            note = "clear at V_MAX" if jumpable else "wider than flat jump"
            out.append(Gap(y, a, b, width, jumpable, note))
    return out


def jump_envelope() -> float:
    """Horizontal pixels covered by a flat-ground jump that starts at V_MAX."""
    samples = jump_profile()
    # frames until y returns to >= 0 after the apex
    return abs(C.V_MAX) * C.TIME * (C.UNREAL_HEIGHT / C.REAL_HEIGHT) * len(samples)


def classify_gaps(world: World) -> list[Gap]:
    return gaps(world)


@dataclass(frozen=True)
class Ledge:
    left_tile: int
    left_y: int
    right_tile: int
    right_y: int
    width_px: int
    rise_tiles: int
    jumpable_at_vmax: bool
    note: str


def _tops(world: World) -> list[tuple[int, int]]:
    cells = _solid_cells(world.runtime_tiles)
    return sorted((x, y) for x, y in cells if (x, y - 1) not in cells)


def ledges(world: World, row_slop: int = 2) -> list[Ledge]:
    """Gaps from the right edge of one walkable top to the next top on the right.

    Adjacent rows count (the world-1 opening goes from y=9 grass onto y=8 dirt).
    """
    tops = _tops(world)
    by_x: dict[int, list[int]] = {}
    for x, y in tops:
        by_x.setdefault(x, []).append(y)
    xs = sorted(by_x)
    env = jump_envelope()
    out: list[Ledge] = []
    seen = set()
    for x in xs:
        if x + 1 in by_x:
            continue
        nxt = next((n for n in xs if n > x + 1), None)
        if nxt is None:
            continue
        for y in by_x[x]:
            partners = [ny for ny in by_x[nxt] if abs(ny - y) <= row_slop]
            if not partners:
                continue
            ny = min(partners, key=lambda p: abs(p - y))
            key = (x, y, nxt, ny)
            if key in seen:
                continue
            seen.add(key)
            width = (nxt - (x + 1)) * C.WIDTH
            rise = y - ny
            jumpable = width < env
            note = "clear at V_MAX" if jumpable else "wider than flat jump"
            if rise > 0:
                note += f", up {rise} tile"
            elif rise < 0:
                note += f", down {-rise} tile"
            out.append(Ledge(x, y, nxt, ny, width, rise, jumpable, note))
    return out
