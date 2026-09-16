"""Sprite-sheet math from Role::show / Scene::show. No BMP decoding."""

from __future__ import annotations

from dataclasses import dataclass

from .constants import F, HEIGHT, STEP, TIME, WIDTH


@dataclass(frozen=True)
class Blit:
    w: int
    h: int
    color_x: int
    color_y: int
    mask_x: int
    mask_y: int


def hero_walk_iframe(role_pos: int) -> int:
    """role.cpp: rolePos / STEP % 4 chooses frame 1 or 2."""
    cell = role_pos // STEP
    if cell % 2 == 0 and cell % 4 != 0:
        return 2
    if cell % 4 == 0:
        return 1
    return 1  # odd STEP cells keep the last even frame in C++; default 1 here


def hero_blit(turn: int, iframe: int, died: bool = False) -> Blit:
    if died:
        return Blit(WIDTH, HEIGHT, 2 * WIDTH, 0, 2 * WIDTH, HEIGHT)
    if turn == 1:
        return Blit(WIDTH, HEIGHT, (iframe - 1) * WIDTH, 0, (iframe - 1) * WIDTH, HEIGHT)
    return Blit(
        WIDTH,
        HEIGHT,
        (iframe - 1) * WIDTH + 3 * WIDTH,
        0,
        (iframe - 1) * WIDTH + 3 * WIDTH,
        HEIGHT,
    )


def coin_src(iframe: int) -> Blit:
    col = (int(iframe) - 1) * WIDTH
    return Blit(WIDTH, HEIGHT, col, 8 * HEIGHT, col, 9 * HEIGHT)


def score_sparkle_src(iframe: int) -> Blit:
    col = (int(iframe) - 1) * WIDTH
    return Blit(WIDTH, HEIGHT, col, 10 * HEIGHT, col, 11 * HEIGHT)


def enemy_src(iframe: int) -> Blit:
    col = (int(iframe) - 1) * WIDTH
    return Blit(WIDTH, HEIGHT, col, 0, col, HEIGHT)


def bullet_src(iframe: int) -> Blit:
    col = (int(iframe) - 1) * WIDTH
    return Blit(WIDTH, HEIGHT, col, 2 * HEIGHT, col, 3 * HEIGHT)


def bomb_src(iframe: int) -> tuple[Blit, tuple[int, int]]:
    """64×64, drawn centered at (x - WIDTH/2, y - HEIGHT/2)."""
    col = (int(iframe) - 1) * 2 * WIDTH
    blit = Blit(2 * WIDTH, 2 * HEIGHT, col, 4 * HEIGHT, col, 6 * HEIGHT)
    return blit, (-WIDTH // 2, -HEIGHT // 2)


def map_src(tile_id: int) -> tuple[int, int, int, int]:
    """(w, h, x0, y0) in map.bmp. Scenery ids are not on this sheet."""
    y0 = (tile_id - 1) * HEIGHT
    if tile_id == 7:
        return (2 * WIDTH, HEIGHT, 0, y0)
    if tile_id in (8, 10):
        return (2 * WIDTH, 2 * HEIGHT, 0, y0)
    return (WIDTH, HEIGHT, 0, y0)


def scenery_src(tile_id: int, iframe: float) -> tuple[int, int, int, int, int]:
    """(w, h, x, color_y, mask_y) in scenery.bmp."""
    y0 = (tile_id - 11) * 4 * HEIGHT
    x = (int(iframe) - 1) * 3 * WIDTH
    return (3 * WIDTH, 2 * HEIGHT, x, y0, y0 + 2 * HEIGHT)


def sky_y(world: int) -> int:
    return -(world - 1) * 384  # YSIZE


def wrap_iframe(value: float, period: int, speed: float) -> float:
    value += speed
    if int(value) == period:
        return 1.0
    return value


# Speeds copied from the C++ iframe ticks.
COIN_SPEED = TIME * 7
ENEMY_SPEED = TIME * 5
BOMB_SPEED = TIME * 10
BULLET_SPEED = TIME * 10
SCORE_SPEED = TIME * 8
FOOD_SPEED = TIME * 7
SCENERY_SPEED = F

MCI_ALIASES = {
    "music_bg": "背景音乐.mp3",
    "music_win": "胜利.mp3",
    "music_passedAll": "通关.mp3",
    "music_end": "游戏结束.mp3",
    "music_died": "死亡1.mp3",
    "music_jump": "跳.mp3",
    "music_coin": "金币.mp3",
    "music_tread": "踩敌人.mp3",
    "music_getWeapon": "吃到武器.mp3",
    "music_bullet": "子弹.mp3",
    "music_boom": "子弹撞墙.mp3",
    "music_boom2": "子弹打到敌人.mp3",
}

HOME_STRIPS = {
    "home": 0,
    "game_over": -384,
    "level_card": -768,
    "all_clear": -1152,
}
