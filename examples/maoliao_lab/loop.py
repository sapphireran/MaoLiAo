"""One-tick model of main.cpp's inner loop, without graphics or MCI.

This is a study harness: it steps inertia, camera, ending, and a toy
ground plane so `run_lab tick` can print a timeline. It is not a port of
every branch in Role::action.
"""

from __future__ import annotations

from dataclasses import dataclass, field

from .camera import Camera, is_ending, is_passed, world_x
from .commands import unpack
from .constants import (
    A_ROLE,
    CMD_LEFT,
    CMD_RIGHT,
    CMD_UP,
    G,
    LIFE,
    PIXELS_PER_REAL,
    SCORE_COIN,
    TIME,
    V_MAX,
    X0,
    XRIGHT,
    YSIZE,
    jump_launch_vy,
)
from .friction import cap_vx, friction_accel, k_from_vx
from .inertia import step
from .tiles import friction_for


@dataclass
class TickState:
    world: int = 1
    life: int = LIFE
    score: int = 0
    screen_x: float = float(X0)
    screen_y: float = float(X0)  # Role::Role uses X0 for y
    vx: float = 0.0
    vy: float = 0.0
    is_fly: bool = True
    is_shoot: bool = False
    ending: bool = False
    passed: bool = False
    died: bool = False
    camera: Camera = field(default_factory=Camera)
    t: float = 0.0
    frames: int = 0
    ground_y: float = 9 * 32  # world 1 grass top in pixels
    ground_u: float = field(default_factory=lambda: friction_for(1, 1))


@dataclass
class TickLog:
    frame: int
    t: float
    keys: str
    x: float
    y: float
    world_x: float
    vx: float
    vy: float
    fly: bool
    ending: bool
    passed: bool
    died: bool


def _apply_jump(state: TickState, bits: int) -> None:
    want = bool(bits & CMD_UP)
    if not want:
        return
    if (not state.is_fly and not state.ending) or state.world == 3:
        state.is_fly = True
        state.vy = jump_launch_vy()


def _integrate_vertical(state: TickState) -> None:
    if not state.is_fly:
        if state.screen_y + 1 < state.ground_y:
            state.is_fly = True
        return
    dy_real, state.vy = step(state.vy, TIME, G)
    state.screen_y += dy_real * PIXELS_PER_REAL
    if state.screen_y + 32 >= state.ground_y and state.vy > 0:
        state.is_fly = False
        state.vy = 0.0
        state.screen_y = state.ground_y - 32
    if state.screen_y > YSIZE:
        state.died = True


def _integrate_horizontal(state: TickState, bits: int) -> None:
    a = 0.0
    if (bits & CMD_LEFT) and not state.ending:
        a -= A_ROLE
    if (bits & CMD_RIGHT) or state.ending:
        a += A_ROLE
    airborne = state.is_fly
    a1 = friction_accel(state.vx, a, state.ground_u, airborne=airborne)
    prev = state.vx
    dx_real, state.vx = step(state.vx, TIME, a + a1)
    if prev * state.vx < 0:
        state.vx = 0.0
    state.vx = cap_vx(state.vx)
    state.screen_x += dx_real * PIXELS_PER_REAL
    prev_x0 = state.camera.x0
    state.screen_x = state.camera.pin_hero_screen_x(state.screen_x, state.vx, state.ending)
    state.camera.step_parallax(state.vx, state.screen_x, prev_x0)
    dist = world_x(state.screen_x, state.camera.x0)
    if is_ending(dist, state.world):
        state.ending = True
    if is_passed(state.screen_x):
        state.passed = True


def tick(state: TickState, bits: int = 0) -> TickLog:
    if not state.died and not state.passed:
        _apply_jump(state, bits)
        _integrate_vertical(state)
        _integrate_horizontal(state, bits)
    state.t += TIME
    state.frames += 1
    return TickLog(
        frame=state.frames,
        t=state.t,
        keys=",".join(unpack(bits)) or "-",
        x=state.screen_x,
        y=state.screen_y,
        world_x=world_x(state.screen_x, state.camera.x0),
        vx=state.vx,
        vy=state.vy,
        fly=state.is_fly,
        ending=state.ending,
        passed=state.passed,
        died=state.died,
    )


def collect_coin(state: TickState) -> None:
    state.score += SCORE_COIN


# Silence unused import warnings in some linters; k_from_vx is part of the public lab.
__all__ = ["TickState", "TickLog", "tick", "collect_coin", "k_from_vx", "XRIGHT", "V_MAX"]
