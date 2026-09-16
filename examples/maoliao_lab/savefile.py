"""gameRecord.dat: a single ASCII world index.

The C++ write path is fprintf_s("%d", world). The read path rejects
values outside 1..3. Lives, score, and isShoot are not serialized.
"""

from __future__ import annotations

from pathlib import Path

MIN_WORLD = 1
MAX_WORLD = 3


class SaveError(ValueError):
    pass


def encode(world: int) -> bytes:
    if not MIN_WORLD <= world <= MAX_WORLD:
        raise SaveError(f"world {world} is outside {MIN_WORLD}..{MAX_WORLD}")
    return str(world).encode("ascii")


def decode(payload: bytes | str) -> int:
    text = payload.decode("ascii") if isinstance(payload, (bytes, bytearray)) else payload
    text = text.strip()
    if not text:
        raise SaveError("empty save")
    try:
        flag = int(text)
    except ValueError as exc:
        raise SaveError(f"not an int: {text!r}") from exc
    if flag >= 4 or flag <= 0:
        raise SaveError(f"rejected by control.cpp: {flag}")
    return flag


def write_file(path: Path | str, world: int) -> None:
    Path(path).write_bytes(encode(world))


def read_file(path: Path | str) -> int:
    return decode(Path(path).read_bytes())
