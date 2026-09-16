"""Headless ports of MaoLiAo V2.0 formulas and authored level tables.

Nothing here links EasyX or Win32. The numbers are copied from
``MaoLiAo/define.h``, ``inertia.cpp``, ``role.cpp``, ``scene.cpp``,
and ``control.cpp`` so docs and tests can run on Linux.
"""

from .constants import *  # noqa: F403
from .commands import Command, parse_command_names, combine, describe
from .inertia import move, jump_launch_vy, friction_u, integrate_jump
from .collision import actor_corners, rect_from_tile, is_hit, hit_map
from .save import read_record, write_record, valid_world
from .worlds import WORLD_SPECS, tiles_for, coins_for, enemies_for, food_for

__all__ = [
    "Command",
    "parse_command_names",
    "combine",
    "describe",
    "move",
    "jump_launch_vy",
    "friction_u",
    "integrate_jump",
    "actor_corners",
    "rect_from_tile",
    "is_hit",
    "hit_map",
    "read_record",
    "write_record",
    "valid_world",
    "WORLD_SPECS",
    "tiles_for",
    "coins_for",
    "enemies_for",
    "food_for",
]
