"""Command bit flags matching define.h / Control::GetCommand."""

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

FLAG_NAMES: tuple[tuple[int, str], ...] = (
    (CMD_LEFT, "CMD_LEFT"),
    (CMD_RIGHT, "CMD_RIGHT"),
    (CMD_UP, "CMD_UP"),
    (CMD_DOWN, "CMD_DOWN"),
    (CMD_SHOOT, "CMD_SHOOT"),
    (CMD_ESC, "CMD_ESC"),
    (VIR_RETURN, "VIR_RETURN"),
    (VIR_RESTART, "VIR_RESTART"),
    (VIR_HOME, "VIR_HOME"),
)

KEYS: dict[str, int] = {
    "A": CMD_LEFT,
    "D": CMD_RIGHT,
    "W": CMD_UP,
    "K": CMD_UP,
    "S": CMD_DOWN,
    "J": CMD_SHOOT,
    "ESC": CMD_ESC,
}


def decode(mask: int) -> list[str]:
    names = [name for bit, name in FLAG_NAMES if mask & bit]
    leftover = mask
    for bit, _ in FLAG_NAMES:
        leftover &= ~bit
    if leftover:
        names.append(f"unknown:0x{leftover:x}")
    return names


def encode(*names: str) -> int:
    lookup = {name: bit for bit, name in FLAG_NAMES}
    mask = 0
    for name in names:
        key = name.upper()
        if key in lookup:
            mask |= lookup[key]
        elif key in KEYS:
            mask |= KEYS[key]
        else:
            raise KeyError(f"unknown command {name!r}")
    return mask


def from_keys(held: str) -> int:
    """Encode a chord such as 'A+W+J' or 'Esc'."""
    mask = 0
    if not held.strip():
        return 0
    for token in held.replace(",", "+").split("+"):
        token = token.strip().upper()
        if not token:
            continue
        if token not in KEYS:
            raise KeyError(f"unknown key {token!r}")
        mask |= KEYS[token]
    return mask
