"""Numbers copied from MaoLiAo/define.h and the related headers.

Keep this module free of gameplay logic so tests can import constants
without pulling in the inertia stepper.
"""

from __future__ import annotations

# --- screen / tiles (define.h) ---
XSIZE = 512
YSIZE = 384
WIDTH = 32
HEIGHT = 32
X0 = 2 * WIDTH  # 64; also mistakenly used as spawn y
Y0 = 3 * HEIGHT  # 96; documented spawn y, unused in Role::Role
TIME = 0.01
STEP = 10
ENEMY_STEP = 1
K_MAP_BG = 5
XLEFT = 0
XRIGHT = WIDTH * 6  # 192

# --- kinematics ---
REAL_HEIGHT = 3.5
UNREAL_HEIGHT = 3 * HEIGHT + 5  # 101
G = 30.0
V_MAX = 8.0
A_ROLE = 20.0
T1 = 0.5
T2 = 1.2
T3 = 1.5

# Macros that expand with a trailing semicolon in C++.
F = TIME * 0.3
LIFE = 5

# --- command bits ---
CMD_LEFT = 1
CMD_RIGHT = 2
CMD_UP = 4
CMD_DOWN = 8
CMD_SHOOT = 16
CMD_ESC = 32
VIR_RETURN = 64
VIR_RESTART = 128
VIR_HOME = 256

# --- role.h / scene.h limits ---
MAP_NUMBER = 30
COINS_NUMBER = 70
SCORE_NUMBER = 5
FOOD_NUMBER = 5
ENEMY_TOTE = 30
BOMB_NUMBER = 5
BULLET_NUMBER = 30
LEHGTH_INTERVAL_BULLET = 4  # typo preserved
TIME_INTERVAL_BULLET = 0.2
MAX_DISTANCE = 480

# Collision treats ids 8 and 10 as 2×2 cells.
PIPE_MOUTH_IDS = frozenset({8, 10})
SOLID_ID_MAX = 10  # hitMap walks id in (0, 11)
CLOUD_ID = 2
GOAL_ID = 12

# Scoring (role.cpp)
SCORE_COIN = 10
SCORE_ENEMY = 5

# Ending distances in pixels (scene.cpp isEnding)
ENDING_DISTANCE = {1: 94 * WIDTH, 2: 104 * WIDTH, 3: 94 * WIDTH}

PIXELS_PER_REAL = UNREAL_HEIGHT / REAL_HEIGHT  # 101 / 3.5


def friction_u(t_seconds: float) -> float:
    """Map tile u as filled in Scene::createMap: (V_MAX / T) / G."""
    return (V_MAX / t_seconds) / G


U_HIGH = friction_u(T1)  # ~0.5333
U_NORMAL = friction_u(T2)  # ~0.2222
U_DEFAULT = friction_u(T3)  # ~0.1778


def jump_launch_vy() -> float:
    """Role::action: vY = -sqrt(2 * G * REAL_HEIGHT)."""
    return -(2.0 * G * REAL_HEIGHT) ** 0.5


def real_to_pixels(distance_real: float) -> float:
    return distance_real * PIXELS_PER_REAL


def pixels_to_real(distance_px: float) -> float:
    return distance_px / PIXELS_PER_REAL
