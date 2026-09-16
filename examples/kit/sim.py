"""Frame stepper for the hero. Enemies and bullets are left to the C++ game.

This is enough to land, walk, jump, scroll, collect coins, eat the flower,
trip the ending rail, and fall in a pit — the checks the docs actually make.
"""

from __future__ import annotations

from dataclasses import dataclass, field
import math

from .camera import is_ending, world_x
from .collision import Tile, hit_rect, hit_tile
from .commands import Command
from .constants import C
from .kinematics import inertia_move, jump_launch_vy, metres_to_pixels, walk_step
from .worlds import World, load_world


@dataclass
class HeroSim:
    world: World
    x: float = float(C.X0)
    y: float = float(C.X0)
    xx: float = float(C.X0)
    yy: float = float(C.X0)
    x0: float = 0.0
    vX: float = 0.0
    vY: float = 0.0
    is_fly: bool = True
    is_shoot: bool = False
    ending: bool = False
    passed: bool = False
    died: bool = False
    score: int = 0
    coins_left: list = field(default_factory=list)
    food_left: list = field(default_factory=list)
    frame: int = 0

    def __post_init__(self):
        if not self.coins_left:
            self.coins_left = [tuple(c) for c in self.world.coins]
        if not self.food_left:
            self.food_left = [tuple(p) for p in self.world.food_pixels]

    @property
    def wx(self) -> float:
        return world_x(self.x, self.x0)

    @property
    def tiles(self) -> list[Tile]:
        return self.world.runtime_tiles


def _probe(sim: HeroSim, x: float, y: float):
    return hit_tile(world_x(x, sim.x0), y, sim.tiles, world=sim.world.index, is_shoot=sim.is_shoot)


def step_hero(sim: HeroSim, cmd: Command | None = None) -> HeroSim:
    cmd = cmd or Command()
    if sim.died or sim.passed:
        sim.frame += 1
        return sim

    direction = 0
    want_up = cmd.up and (not sim.is_fly and not sim.ending or sim.world.index == 3)

    if want_up:
        sim.is_fly = True
        sim.vY = jump_launch_vy()

    if sim.is_fly:
        dy, sim.vY = inertia_move(sim.vY, C.TIME, C.G)
        sim.yy = sim.yy + metres_to_pixels(dy)
        sim.y = int(sim.yy)
        tile, lethal = _probe(sim, sim.x, sim.y + 1)
        if lethal:
            sim.died = True
        if tile is not None:
            if sim.vY > 0:
                sim.is_fly = False
            sim.vY = 0.0
            sim.y = (sim.y + C.HEIGHT // 2) // C.HEIGHT * C.HEIGHT
            sim.yy = float(sim.y)
        elif sim.vY > 0 and sim.y > C.YSIZE:
            sim.died = True
    else:
        tile, lethal = _probe(sim, sim.x, sim.y + 1)
        if lethal:
            sim.died = True
        if tile is None:
            sim.is_fly = True

    if cmd.left and not sim.ending:
        direction -= 1
    if cmd.right or sim.ending:
        direction += 1

    foot, _ = _probe(sim, sim.x, sim.y + 1)
    u = foot.u if foot is not None else 0.0
    dx, sim.vX = walk_step(sim.vX, direction, u, foot is not None)
    sim.xx = sim.xx + dx
    sim.x = int(sim.xx)

    if is_ending(sim.wx, sim.world.index):
        sim.ending = True
    if sim.x > C.XSIZE:
        sim.passed = True

    blocked, lethal = _probe(sim, sim.x, sim.y)
    if lethal:
        sim.died = True
    if blocked is not None:
        if sim.x > C.XRIGHT:
            sim.x = float(C.XRIGHT)
        else:
            sim.x = int(sim.xx - dx)
        sim.xx = float(sim.x)
        sim.vX = 0.0

    if abs(sim.vX) > C.V_MAX:
        sim.vX = math.copysign(C.V_MAX, sim.vX)

    if sim.x < C.XLEFT:
        sim.x = float(C.XLEFT)
        sim.xx = sim.x
        sim.vX = 0.0
    elif sim.x > C.XRIGHT and not sim.ending:
        sim.x0 -= sim.x - C.XRIGHT
        sim.x = float(C.XRIGHT)
        sim.xx = sim.x

    _collect(sim)
    sim.frame += 1
    return sim


def _collect(sim: HeroSim) -> None:
    wx, y = sim.wx, sim.y
    kept = []
    for cx, cy in sim.coins_left:
        box = (cx * C.WIDTH, cy * C.HEIGHT, cx * C.WIDTH + C.WIDTH, cy * C.HEIGHT + C.HEIGHT)
        if hit_rect(wx, y, box):
            sim.score += C.SCORE_COIN
        else:
            kept.append((cx, cy))
    sim.coins_left = kept
    food_kept = []
    for fx, fy in sim.food_left:
        box = (fx, fy, fx + 3 * C.WIDTH / 2 + 4, fy + 4 * C.HEIGHT / 5)
        if hit_rect(wx, y, box):
            sim.is_shoot = True
        else:
            food_kept.append((fx, fy))
    sim.food_left = food_kept


def run_replay(world_index: int, commands, *, seed: int = 2020) -> HeroSim:
    sim = HeroSim(world=load_world(world_index, seed=seed))
    for cmd in commands:
        step_hero(sim, cmd)
        if sim.died or sim.passed:
            break
    return sim
