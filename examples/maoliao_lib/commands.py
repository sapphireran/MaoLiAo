"""Bit mask helpers matching Control::GetCommand and the VIR_* pause results."""

from __future__ import annotations

from .constants import (
    CMD_DOWN,
    CMD_ESC,
    CMD_LEFT,
    CMD_RIGHT,
    CMD_SHOOT,
    CMD_UP,
    VIR_HOME,
    VIR_RESTART,
    VIR_RETURN,
)

CMD = {
    "left": CMD_LEFT,
    "right": CMD_RIGHT,
    "up": CMD_UP,
    "down": CMD_DOWN,
    "shoot": CMD_SHOOT,
    "esc": CMD_ESC,
}

VIRTUAL = {
    "return": VIR_RETURN,
    "restart": VIR_RESTART,
    "home": VIR_HOME,
}

HARDWARE = {
    "left": "A",
    "right": "D",
    "up": "W or K",
    "down": "S",
    "shoot": "J",
    "esc": "Esc",
}

PAUSE_ROWS = (
    ("返回游戏", "Return", VIR_RETURN),
    ("重新开始", "start again", VIR_RESTART),
    ("退出游戏", "The menu", VIR_HOME),
    ("进行存档", "Write Data", VIR_RETURN),
)

TITLE_ROWS = (
    ("开始", "Start", "leave_menu"),
    ("介绍", "Introduction", "intro"),
    ("指导", "Directions", "howto"),
    ("退出", "Exit", "exit_process"),
    ("读档", "Read Load", "load"),
)


def pack(*names: str) -> int:
    mask = 0
    for name in names:
        if name not in CMD:
            raise KeyError(f"unknown command {name!r}")
        mask |= CMD[name]
    return mask


def unpack(mask: int) -> tuple[str, ...]:
    names = []
    for name, bit in CMD.items():
        if mask & bit:
            names.append(name)
    return tuple(names)


def has(mask: int, name: str) -> bool:
    return bool(mask & CMD[name])


def is_virtual(mask: int) -> bool:
    return bool(mask & (VIR_RETURN | VIR_RESTART | VIR_HOME))
