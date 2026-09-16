"""gameRecord.dat: one integer world index."""

from __future__ import annotations

from pathlib import Path


def valid_world(value: int) -> bool:
    return value in (1, 2, 3)


def read_record(path: str | Path) -> tuple[int | None, str]:
    p = Path(path)
    if not p.exists():
        return None, "missing"
    text = p.read_text(encoding="utf-8", errors="replace").strip()
    try:
        value = int(text)
    except ValueError:
        return None, "garbage"
    if not valid_world(value):
        return value, "invalid"
    return value, "ok"


def write_record(path: str | Path, world: int) -> None:
    if not valid_world(world):
        raise ValueError(f"world must be 1..3, got {world}")
    Path(path).write_text(str(world), encoding="ascii")
