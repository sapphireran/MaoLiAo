"""Macros from MaoLiAo/define.h and related caps from the headers."""

# Screen and tiles
XSIZE = 512
YSIZE = 384
WIDTH = 32
HEIGHT = 32
X0 = 2 * WIDTH
Y0 = 3 * HEIGHT

# Timing and motion
TIME = 0.01
STEP = 10
ENEMY_STEP = 1
K_MAP_BG = 5
XLEFT = 0
XRIGHT = WIDTH * 6  # 192

# Jump / gravity
REAL_HEIGHT = 3.5
UNREAL_HEIGHT = 3 * HEIGHT + 5  # 101
G = 30.0
V_MAX = 8.0
A_ROLE = 20.0
T1 = 0.5
T2 = 1.2
T3 = 1.5
PIXEL_SCALE = UNREAL_HEIGHT / REAL_HEIGHT

# Life and scenery frame rate (macros include a trailing semicolon in C)
LIFE = 5
F = TIME * 0.3

# Command bits
CMD_LEFT = 1
CMD_RIGHT = 2
CMD_UP = 4
CMD_DOWN = 8
CMD_SHOOT = 16
CMD_ESC = 32
VIR_RETURN = 64
VIR_RESTART = 128
VIR_HOME = 256

# Caps from role.h / scene.h
MAP_NUMBER = 30
COINS_NUMBER = 70
SCORE_NUMBER = 5
FOOD_NUMBER = 5
ENEMY_TOTE = 30
BOMB_NUMBER = 5
BULLET_NUMBER = 30
BULLET_INTERVAL = WIDTH
LENGTH_INTERVAL_BULLET = 4  # LEHGTH_INTERVAL_BULLET in the source
TIME_INTERVAL_BULLET = 0.2
MAX_DISTANCE = 480

# Scoring
SCORE_COIN = 10
SCORE_ENEMY = 5

# Ending distances in pixels (-x0 + x)
ENDING = {
    1: 94 * WIDTH,
    2: 104 * WIDTH,
    3: 94 * WIDTH,
}
