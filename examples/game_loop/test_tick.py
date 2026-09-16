#!/usr/bin/env python3
"""Hero tick: land on world-1 grass, run, pick a coin, die to an enemy."""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from maoliao_lib.commands import pack  # noqa: E402
from maoliao_lib.constants import LIFE, SCORE_COIN, XRIGHT  # noqa: E402
from maoliao_lib.hero import GameState, tick  # noqa: E402


def _settle(state: GameState, frames: int = 80) -> GameState:
    for _ in range(frames):
        tick(state, 0)
        if state.hero.died:
            break
    return state


def test_spawn_falls_onto_world1_grass() -> None:
    state = GameState.fresh(1)
    assert state.hero.is_fly
    _settle(state)
    assert not state.hero.died
    assert not state.hero.is_fly
    # Grass top at y=9 → snapped hero.y is 8*32 = 256.
    assert state.hero.y == 256


def test_hold_right_scrolls_camera() -> None:
    state = GameState.fresh(1)
    _settle(state)
    right = pack("right")
    for _ in range(200):
        tick(state, right)
    assert state.hero.x == float(XRIGHT)
    assert state.hero.x0 < 0
    assert state.hero.world_x > XRIGHT


def test_walk_into_first_cloud_coins() -> None:
    state = GameState.fresh(1)
    _settle(state)
    # Cloud coins sit at tiles (10..13, 5). Jump from the grass and drift right.
    # A simpler path: teleport the integrator onto the cloud and tick once.
    state.hero.x = state.hero.xx = 10 * 32 + 4
    state.hero.y = state.hero.yy = 5 * 32 + 4
    state.hero.is_fly = True
    state.hero.vy = 0.0
    before = state.hero.score
    tick(state, 0)
    assert state.hero.score == before + SCORE_COIN
    assert state.coins[0] == (0, 0)


def test_touching_an_enemy_while_grounded_kills() -> None:
    state = GameState.fresh(1)
    _settle(state)
    ex, ey, turn = state.enemies[0]
    state.hero.x = state.hero.xx = ex + 4
    state.hero.y = state.hero.yy = ey + 4
    state.hero.vy = 0.0
    state.hero.is_fly = False
    tick(state, 0)
    assert state.hero.died


def test_stomp_awards_five() -> None:
    state = GameState.fresh(1)
    ex, ey, turn = state.enemies[0]
    state.hero.x = state.hero.xx = ex + 4
    state.hero.y = state.hero.yy = ey - 8
    state.hero.vy = 4.0
    state.hero.is_fly = True
    tick(state, 0)
    assert not state.hero.died
    assert state.hero.score == 5
    assert state.enemies[0][2] == 0


def test_lives_start_at_five() -> None:
    state = GameState.fresh(2)
    assert state.life == LIFE
    assert state.world == 2


def main() -> int:
    testers = [
        test_spawn_falls_onto_world1_grass,
        test_hold_right_scrolls_camera,
        test_walk_into_first_cloud_coins,
        test_touching_an_enemy_while_grounded_kills,
        test_stomp_awards_five,
        test_lives_start_at_five,
    ]
    for fn in testers:
        fn()
        print(f"ok  {fn.__name__}")
    print(f"{len(testers)} tick tests passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
