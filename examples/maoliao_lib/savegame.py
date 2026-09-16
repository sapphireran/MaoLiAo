"""gameRecord.dat: a single world integer, as written by Control::pauseClick."""

from __future__ import annotations

from pathlib import Path

REPO_RECORD = Path(__file__).resolve().parents[2] / "MaoLiAo" / "gameRecord.dat"


def parse(text: str) -> int:
    token = text.strip().split()[0] if text.strip() else ""
    if not token:
        raise ValueError("empty save")
    return int(token)


def is_valid_world(world: int) -> bool:
    """Same gate as 读档: reject flag <= 0 or flag >= 4."""
    return 1 <= world <= 3


def read_path(path: Path) -> int:
    return parse(path.read_text(encoding="utf-8"))


def write_path(path: Path, world: int) -> None:
    if not is_valid_world(world):
        raise ValueError(f"world {world} is outside 1..3")
    path.write_text(str(world), encoding="utf-8")


def load_repo_record() -> int:
    return read_path(REPO_RECORD)
