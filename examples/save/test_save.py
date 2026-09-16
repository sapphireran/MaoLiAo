#!/usr/bin/env python3
"""Read the checked-in gameRecord.dat and round-trip a temp save."""

from __future__ import annotations

import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao_lib.savegame import (  # noqa: E402
    REPO_RECORD,
    is_valid_world,
    load_repo_record,
    parse,
    read_path,
    write_path,
)


def test_repo_record_is_world_three() -> None:
    assert REPO_RECORD.is_file()
    world = load_repo_record()
    assert world == 3
    assert is_valid_world(world)


def test_parse_strips_whitespace() -> None:
    assert parse("2\n") == 2
    assert parse("  1") == 1


def test_reject_out_of_range() -> None:
    assert not is_valid_world(0)
    assert not is_valid_world(4)
    assert not is_valid_world(-1)


def test_round_trip(tmp: Path) -> None:
    write_path(tmp, 2)
    assert tmp.read_text(encoding="utf-8") == "2"
    assert read_path(tmp) == 2


def test_write_rejects_four() -> None:
    try:
        write_path(Path("."), 4)
    except ValueError:
        return
    raise AssertionError("expected ValueError")


def main() -> int:
    test_repo_record_is_world_three()
    print("ok  test_repo_record_is_world_three")
    test_parse_strips_whitespace()
    print("ok  test_parse_strips_whitespace")
    test_reject_out_of_range()
    print("ok  test_reject_out_of_range")
    with tempfile.TemporaryDirectory() as folder:
        test_round_trip(Path(folder) / "gameRecord.dat")
    print("ok  test_round_trip")
    test_write_rejects_four()
    print("ok  test_write_rejects_four")
    print("5 save tests passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
