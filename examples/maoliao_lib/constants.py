"""Macros from MaoLiAo/define.h and capacities from role.h / scene.h."""

# Screen and tile
XSIZE = 512
YSIZE = 384
WIDTH = 32
HEIGHT = 32

# Spawn. Role::Role uses X0 for both axes and ignores Y0.
X0 = 2 * WIDTH
Y0 = 3 * HEIGHT
SPAWN_X = X0
SPAWN_Y = X0

# Time and step
TIME = 0.01
STEP = 10
ENEMY_STEP = 1
K_MAP_BG = 5

# Camera lock
XLEFT = 0
XRIGHT = WIDTH * 6

# Jump / run (pre-scale "meters")
REAL_HEIGHT = 3.5
UNREAL_HEIGHT = 3 * HEIGHT + 5
G = 30.0
V_MAX = 8.0
A_ROLE = 20.0
T1 = 0.5
T2 = 1.2
T3 = 1.5

# define.h writes `#define F TIME*0.3;` and `#define LIFE 5;`
F = TIME * 0.3
LIFE = 5

PIXELS_PER_METER = UNREAL_HEIGHT / REAL_HEIGHT

# role.h / scene.h capacities
ENEMY_TOTE = 30
BOMB_NUMBER = 5
BULLET_NUMBER = 30
BULLET_INTERVAL = WIDTH
LENGTH_INTERVAL_BULLET = 4  # LEHGTH_INTERVAL_BULLET in the source
TIME_INTERVAL_BULLET = 0.2
MAX_DISTANCE = 480
MAP_NUMBER = 30
COINS_NUMBER = 70
SCORE_NUMBER = 5
FOOD_NUMBER = 5

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

# Scoring
SCORE_COIN = 10
SCORE_ENEMY = 5

# World 3 Flappy rule: any solid tile except this id kills unless is_shoot.
SAFE_TILE_WORLD3 = 2

# Pipe mouths that occupy 2x2 cells in both blit and hitMap.
PIPE_DOUBLE_IDS = frozenset({8, 10})

# Solid for hitMap: id in 1..10
def is_solid_id(tile_id: int) -> bool:
    return 1 <= tile_id <= 10
