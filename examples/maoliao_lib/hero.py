"""A tick of Role::action without sprites, audio, or EasyX.

The integrator, camera lock, coin / food / enemy rules, and world-3
lethal tiles follow the C++ control flow. Bullets are omitted — they
are advanced from Role::show, not from action.
"""

from __future__ import annotations

from dataclasses import dataclass, field

from .collision import Tile, hit_coins, hit_enemy, hit_food, hit_map
from .commands import has
from .constants import (
    A_ROLE,
    LIFE,
    SCORE_COIN,
    SCORE_ENEMY,
    SPAWN_X,
    SPAWN_Y,
    TIME,
    XLEFT,
    XRIGHT,
    XSIZE,
    YSIZE,
)
from .inertia import integrate_horizontal, integrate_vertical, jump_speed
from .worlds import (
    coins_for,
    enemies_for,
    food_for,
    is_ending,
    loaded_map,
)


@dataclass
class HeroState:
    x: float = float(SPAWN_X)
    y: float = float(SPAWN_Y)
    xx: float = float(SPAWN_X)
    yy: float = float(SPAWN_Y)
    x0: float = 0.0
    vx: float = 0.0
    vy: float = 0.0
    turn: int = 1
    is_fly: bool = True
    is_shoot: bool = False
    died: bool = False
    ending: bool = False
    passed: bool = False
    score: int = 0
    world_x: float = float(SPAWN_X)


@dataclass
class GameState:
    world: int = 1
    life: int = LIFE
    hero: HeroState = field(default_factory=HeroState)
    tiles: list[Tile] = field(default_factory=list)
    coins: list[tuple[int, int]] = field(default_factory=list)
    foods: list[tuple[float, float]] = field(default_factory=list)
    enemies: list[tuple[float, float, int]] = field(default_factory=list)
    seed: int = 1

    @classmethod
    def fresh(cls, world: int = 1, seed: int = 1) -> "GameState":
        return cls(
            world=world,
            life=LIFE,
            hero=HeroState(),
            tiles=loaded_map(world, seed),
            coins=coins_for(world, seed),
            foods=food_for(world),
            enemies=enemies_for(world),
            seed=seed,
        )


def _snap_y(y: float) -> float:
    return float(int(y + 16) // 32 * 32)


def tick(state: GameState, key: int) -> GameState:
    """Mutates and returns `state` after one TIME step."""
    hero = state.hero
    tiles = state.tiles
    world = state.world
    run = 0.0
    if has(key, "left") and not hero.ending:
        run -= A_ROLE
        hero.turn = -1
    if has(key, "right") or hero.ending:
        run += A_ROLE
        hero.turn = 1

    can_jump = (has(key, "up") and not hero.is_fly and not hero.ending) or (
        has(key, "up") and world == 3
    )
    if can_jump:
        hero.is_fly = True
        hero.vy = jump_speed()

    ground = hit_map(
        hero.x, hero.y + 1, tiles, hero.x0, world=world, is_shoot=hero.is_shoot
    )
    if ground and ground.died:
        hero.died = True

    if hero.is_fly:
        hero.yy, hero.vy = integrate_vertical(hero.yy, hero.vy)
        hero.y = float(int(hero.yy))
        landed = hit_map(
            hero.x, hero.y + 1, tiles, hero.x0, world=world, is_shoot=hero.is_shoot
        )
        if landed:
            if landed.died:
                hero.died = True
            if hero.vy > 0:
                hero.is_fly = False
            hero.vy = 0.0
            hero.y = _snap_y(hero.y)
            hero.yy = hero.y
        elif hero.vy > 0:
            idx = hit_enemy(hero.x, hero.y, state.enemies, hero.x0)
            if idx is not None:
                hero.score += SCORE_ENEMY
                ex, ey, _ = state.enemies[idx]
                state.enemies[idx] = (ex, ey, 0)
            if hero.y > YSIZE:
                hero.died = True
    else:
        if ground is None:
            hero.is_fly = True

    u = None
    support = hit_map(
        hero.x, hero.y + 1, tiles, hero.x0, world=1, is_shoot=hero.is_shoot
    )
    if support is not None:
        u = support.tile.u

    hero.xx, hero.vx = integrate_horizontal(hero.xx, hero.vx, run, u)
    hero.x = float(int(hero.xx))

    hero.world_x = -hero.x0 + hero.x
    if is_ending(world, int(hero.world_x)):
        hero.ending = True
    if hero.x > XSIZE:
        hero.passed = True

    blocked = hit_map(hero.x, hero.y, tiles, hero.x0, world=1, is_shoot=hero.is_shoot)
    if blocked is not None:
        if hero.x > XRIGHT:
            hero.x = float(XRIGHT)
        hero.xx = hero.x
        hero.vx = 0.0

    if hero.x < XLEFT:
        hero.x = float(XLEFT)
        hero.xx = hero.x
        hero.vx = 0.0
    elif hero.x > XRIGHT and not hero.ending:
        hero.x0 -= hero.x - XRIGHT
        hero.x = float(XRIGHT)
        hero.xx = hero.x

    coin = hit_coins(hero.x, hero.y, state.coins, hero.x0)
    if coin is not None:
        hero.score += SCORE_COIN
        state.coins[coin] = (0, 0)

    food = hit_food(hero.x, hero.y, state.foods, hero.x0)
    if food is not None:
        hero.is_shoot = True
        state.foods[food] = (0.0, 0.0)

    emy = hit_enemy(hero.x, hero.y, state.enemies, hero.x0)
    if emy is not None and hero.vy <= 0:
        hero.died = True

    _ = TIME  # documents that this function is one TIME step
    return state
