"""Portable pieces of 猫里奥: numbers, kinematics, AABB, worlds, save file.

These modules copy formulas and tables from MaoLiAo/*.cpp so they can run
without EasyX. The Win32 game remains the source of truth.
"""

from .constants import *  # noqa: F403
from . import collision, commands, hero, inertia, savegame, worlds

__all__ = [
    "collision",
    "commands",
    "hero",
    "inertia",
    "savegame",
    "worlds",
]
