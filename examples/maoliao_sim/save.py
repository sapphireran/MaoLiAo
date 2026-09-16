"""gameRecord.dat: one decimal world number, matching Control save/load."""

from __future__ import annotations

from pathlib import Path
from typing import Union

PathLike = Union[str, Path]


def valid_world(world: int) -> bool:
    """control.cpp rejects ``flag >= 4 || flag <= 0``."""
    return world in (1, 2, 3)


def read_record(path: PathLike) -> int:
    text = Path(path).read_text(encoding="ascii", errors="strict").strip()
    if not text:
        raise ValueError("empty save file")
    # The game uses fscanf %d; accept a leading integer even if junk follows.
    token = text.split()[0]
    world = int(token)
    if not valid_world(world):
        raise ValueError(f"save world {world} is out of range 1..3")
    return world


def write_record(path: PathLike, world: int) -> None:
    if not valid_world(world):
        raise ValueError(f"save world {world} is out of range 1..3")
    Path(path).write_text(str(int(world)), encoding="ascii")
