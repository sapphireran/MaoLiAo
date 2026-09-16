"""Headless MaoLiAo reference kit.

Ports the numbers and the hero tick from the 2020 EasyX sources so the
docs can be checked on a machine that does not have Visual Studio.
"""

from .constants import C, pixel_scale, friction_u
from .kinematics import inertia_move, jump_launch_vy, jump_profile, coast_step
from .collision import inset_corners, aabb_contains_point, hit_tile, tile_aabb
from .commands import Command, parse_replay
from .worlds import load_world, generate_world3, World
from .camera import apply_rail, is_ending, world_x
from .savefile import read_record, write_record, valid_world
from .sim import HeroSim, step_hero
from .reachability import solid_spans, gaps, jump_envelope, classify_gaps, ledges, Ledge
from .ascii import render_ascii
from .svg import render_svg
from .raster import render_png, render_jump_png

__all__ = [
    "C",
    "pixel_scale",
    "friction_u",
    "inertia_move",
    "jump_launch_vy",
    "jump_profile",
    "coast_step",
    "inset_corners",
    "aabb_contains_point",
    "hit_tile",
    "tile_aabb",
    "Command",
    "parse_replay",
    "load_world",
    "generate_world3",
    "World",
    "apply_rail",
    "is_ending",
    "world_x",
    "read_record",
    "write_record",
    "valid_world",
    "HeroSim",
    "step_hero",
    "solid_spans",
    "gaps",
    "jump_envelope",
    "classify_gaps",
    "ledges",
    "Ledge",
    "render_ascii",
    "render_svg",
]
