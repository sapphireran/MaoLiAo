"""Command bit packing from define.h / Control::GetCommand."""

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

KEY_TO_BIT = {
    "A": CMD_LEFT,
    "D": CMD_RIGHT,
    "W": CMD_UP,
    "K": CMD_UP,
    "S": CMD_DOWN,
    "J": CMD_SHOOT,
    "ESC": CMD_ESC,
}

BIT_TO_NAME = {
    CMD_LEFT: "left",
    CMD_RIGHT: "right",
    CMD_UP: "up",
    CMD_DOWN: "down",
    CMD_SHOOT: "shoot",
    CMD_ESC: "esc",
    VIR_RETURN: "vir_return",
    VIR_RESTART: "vir_restart",
    VIR_HOME: "vir_home",
}


def pack(keys: str | list[str]) -> int:
    """OR together bits. ``keys`` may be 'AWJ' or ['A', 'W', 'J'] or 'ESC'."""
    if isinstance(keys, str) and keys.upper() == "ESC":
        return CMD_ESC
    if isinstance(keys, str):
        tokens: list[str] = list(keys.upper())
    else:
        tokens = [k.upper() for k in keys]
    bits = 0
    for token in tokens:
        if token == "ESC":
            bits |= CMD_ESC
            continue
        bits |= KEY_TO_BIT[token]
    return bits


def unpack(bits: int) -> list[str]:
    names = []
    for mask in sorted(BIT_TO_NAME):
        if bits & mask:
            names.append(BIT_TO_NAME[mask])
    return names


def sticky_sample(previous: int, buffer_hit: bool, new_bits: int) -> int:
    """Model Control::getKey: only replace the cache when _kbhit() is true.

    Esc handling is left to the caller (pause overlay).
    """
    if buffer_hit:
        return new_bits
    return previous
