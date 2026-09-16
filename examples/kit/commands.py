"""Command bitset and a tiny replay DSL."""

from __future__ import annotations

from dataclasses import dataclass


CMD_LEFT = 1
CMD_RIGHT = 2
CMD_UP = 4
CMD_DOWN = 8
CMD_SHOOT = 16
CMD_ESC = 32
VIR_RETURN = 64
VIR_RESTART = 128
VIR_HOME = 256

_CHAR = {
    "A": CMD_LEFT,
    "D": CMD_RIGHT,
    "W": CMD_UP,
    "K": CMD_UP,
    "S": CMD_DOWN,
    "J": CMD_SHOOT,
    "E": CMD_ESC,
}


@dataclass(frozen=True)
class Command:
    bits: int = 0

    @property
    def left(self) -> bool:
        return bool(self.bits & CMD_LEFT)

    @property
    def right(self) -> bool:
        return bool(self.bits & CMD_RIGHT)

    @property
    def up(self) -> bool:
        return bool(self.bits & CMD_UP)

    @property
    def down(self) -> bool:
        return bool(self.bits & CMD_DOWN)

    @property
    def shoot(self) -> bool:
        return bool(self.bits & CMD_SHOOT)

    @classmethod
    def from_keys(cls, keys: str) -> "Command":
        bits = 0
        for ch in keys.upper():
            if ch in _CHAR:
                bits |= _CHAR[ch]
        return cls(bits)

    def names(self) -> list[str]:
        out = []
        for name, bit in (
            ("left", CMD_LEFT),
            ("right", CMD_RIGHT),
            ("up", CMD_UP),
            ("down", CMD_DOWN),
            ("shoot", CMD_SHOOT),
            ("esc", CMD_ESC),
            ("return", VIR_RETURN),
            ("restart", VIR_RESTART),
            ("home", VIR_HOME),
        ):
            if self.bits & bit:
                out.append(name)
        return out


def parse_replay(text: str) -> list[Command]:
    """Lines of `frames keys`. `#` comments. `keys` may be `.` for idle.

    Example::

        40 D
        1  DW
        80 D
        10 .
    """
    frames: list[Command] = []
    for raw in text.splitlines():
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        n = int(parts[0])
        keys = "" if len(parts) == 1 or parts[1] == "." else parts[1]
        cmd = Command.from_keys(keys)
        frames.extend([cmd] * n)
    return frames
