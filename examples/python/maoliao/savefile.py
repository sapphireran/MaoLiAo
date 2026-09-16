"""gameRecord.dat — a single ASCII integer world index."""

from __future__ import annotations

from pathlib import Path


class SaveError(ValueError):
    """Invalid or missing MaoLiAo save file."""


def read_world(path: str | Path) -> int:
    path = Path(path)
    if not path.is_file():
        raise SaveError(f"missing save file: {path}")
    raw = path.read_text(encoding="ascii", errors="replace").strip()
    if not raw:
        raise SaveError("empty save file")
    # The game uses fscanf "%d"; accept a leading integer even if junk follows.
    token = raw.replace("\x00", " ").split()[0]
    try:
        world = int(token, 10)
    except ValueError as exc:
        raise SaveError(f"not an integer: {raw!r}") from exc
    if world < 1 or world > 3:
        raise SaveError(f"world {world} is outside 1..3 (title screen MessageBox)")
    return world


def write_world(path: str | Path, world: int) -> None:
    if world < 1 or world > 3:
        raise SaveError(f"world {world} is outside 1..3")
    path = Path(path)
    path.write_text(f"{world}", encoding="ascii")
