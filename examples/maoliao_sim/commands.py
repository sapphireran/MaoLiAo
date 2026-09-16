"""Command bitfield from define.h / Control::GetCommand."""

from __future__ import annotations

from enum import IntEnum
from typing import Iterable, List

from . import constants as C


class Command(IntEnum):
    LEFT = C.CMD_LEFT
    RIGHT = C.CMD_RIGHT
    UP = C.CMD_UP
    DOWN = C.CMD_DOWN
    SHOOT = C.CMD_SHOOT
    ESC = C.CMD_ESC
    RETURN = C.VIR_RETURN
    RESTART = C.VIR_RESTART
    HOME = C.VIR_HOME


_NAME_TO_BIT = {
    "left": Command.LEFT,
    "a": Command.LEFT,
    "right": Command.RIGHT,
    "d": Command.RIGHT,
    "up": Command.UP,
    "w": Command.UP,
    "k": Command.UP,
    "down": Command.DOWN,
    "s": Command.DOWN,
    "shoot": Command.SHOOT,
    "j": Command.SHOOT,
    "esc": Command.ESC,
    "escape": Command.ESC,
    "return": Command.RETURN,
    "restart": Command.RESTART,
    "home": Command.HOME,
}

_BIT_TO_LABEL = {
    Command.LEFT: "CMD_LEFT",
    Command.RIGHT: "CMD_RIGHT",
    Command.UP: "CMD_UP",
    Command.DOWN: "CMD_DOWN",
    Command.SHOOT: "CMD_SHOOT",
    Command.ESC: "CMD_ESC",
    Command.RETURN: "VIR_RETURN",
    Command.RESTART: "VIR_RESTART",
    Command.HOME: "VIR_HOME",
}


def parse_command_names(names: Iterable[str]) -> int:
    bits = 0
    for raw in names:
        key = raw.strip().lower()
        if not key:
            continue
        if key not in _NAME_TO_BIT:
            raise ValueError(f"unknown command {raw!r}")
        bits |= int(_NAME_TO_BIT[key])
    return bits


def combine(*commands: Command) -> int:
    bits = 0
    for command in commands:
        bits |= int(command)
    return bits


def describe(bits: int) -> List[str]:
    labels = [label for bit, label in _BIT_TO_LABEL.items() if bits & int(bit)]
    return labels


def is_jump(bits: int, is_fly: bool, ending: bool, world: int) -> bool:
    """Role::action jump predicate."""
    if not (bits & Command.UP):
        return False
    if (not is_fly) and (not ending):
        return True
    return world == 3
