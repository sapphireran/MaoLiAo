#!/usr/bin/env python3
"""Command bit packing matches define.h and the pause / title tables."""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao_lib.commands import (  # noqa: E402
    HARDWARE,
    PAUSE_ROWS,
    TITLE_ROWS,
    has,
    is_virtual,
    pack,
    unpack,
)
from maoliao_lib.constants import (  # noqa: E402
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


def test_individual_bits() -> None:
    assert pack("left") == CMD_LEFT == 1
    assert pack("right") == CMD_RIGHT == 2
    assert pack("up") == CMD_UP == 4
    assert pack("down") == CMD_DOWN == 8
    assert pack("shoot") == CMD_SHOOT == 16
    assert pack("esc") == CMD_ESC == 32


def test_chords() -> None:
    mask = pack("left", "shoot")
    assert mask == 17
    assert unpack(mask) == ("left", "shoot")
    assert has(mask, "left") and has(mask, "shoot")
    assert not has(mask, "right")


def test_virtual_results() -> None:
    assert VIR_RETURN == 64
    assert VIR_RESTART == 128
    assert VIR_HOME == 256
    assert is_virtual(VIR_HOME)
    assert not is_virtual(pack("left"))


def test_pause_row_2_is_home_not_exit() -> None:
    label, hover, bit = PAUSE_ROWS[2]
    assert label == "退出游戏"
    assert hover == "The menu"
    assert bit == VIR_HOME


def test_title_has_five_rows() -> None:
    assert [row[0] for row in TITLE_ROWS] == ["开始", "介绍", "指导", "退出", "读档"]
    assert HARDWARE["up"] == "W or K"


def test_unknown_name_raises() -> None:
    try:
        pack("dash")
    except KeyError:
        return
    raise AssertionError("expected KeyError")


def main() -> int:
    testers = [
        test_individual_bits,
        test_chords,
        test_virtual_results,
        test_pause_row_2_is_home_not_exit,
        test_title_has_five_rows,
        test_unknown_name_raises,
    ]
    for fn in testers:
        fn()
        print(f"ok  {fn.__name__}")
    print(f"{len(testers)} input tests passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
