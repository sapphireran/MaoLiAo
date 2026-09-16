// Portable subset of MaoLiAo gameplay math.
// Constants and formulas match MaoLiAo/define.h, inertia.cpp, role.cpp,
// and scene.cpp. No EasyX / Win32 dependency.
//
// Trailing-semicolon macros (LIFE, F) and the MYDIFINE guard typo are
// not reproduced. Numeric values are the intended ones.

#ifndef MAOLIAO_CORE_HPP
#define MAOLIAO_CORE_HPP

#include <cstddef>

namespace maoliao {

// ---------------------------------------------------------------------------
// Constants (define.h)
// ---------------------------------------------------------------------------

constexpr int kXSize = 512;
constexpr int kYSize = 384;
constexpr int kWidth = 32;
constexpr int kHeight = 32;
constexpr int kX0 = 2 * kWidth;
constexpr int kY0 = 3 * kHeight;
constexpr double kTime = 0.01;
constexpr int kStep = 10;
constexpr int kEnemyStep = 1;
constexpr int kMapBg = 5;
constexpr int kXLeft = 0;
constexpr int kXRight = kWidth * 6;
constexpr double kRealHeight = 3.5;
constexpr double kUnrealHeight = 3 * kHeight + 5;  // 101
constexpr double kG = 30.0;
constexpr double kVMax = 8.0;
constexpr double kARole = 20.0;
constexpr double kT1 = 0.5;
constexpr double kT2 = 1.2;
constexpr double kT3 = 1.5;
constexpr double kSceneryRate = kTime * 0.3;
constexpr int kLife = 5;

constexpr int kCmdLeft = 1;
constexpr int kCmdRight = 2;
constexpr int kCmdUp = 4;
constexpr int kCmdDown = 8;
constexpr int kCmdShoot = 16;
constexpr int kCmdEsc = 32;
constexpr int kVirReturn = 64;
constexpr int kVirRestart = 128;
constexpr int kVirHome = 256;

constexpr int kMapNumber = 30;
constexpr int kCoinsNumber = 70;
constexpr int kFoodNumber = 5;
constexpr int kEnemyTote = 30;
constexpr int kBombNumber = 5;
constexpr int kBulletNumber = 30;
constexpr int kLengthIntervalBullet = 4;
constexpr double kTimeIntervalBullet = 0.2;
constexpr int kMaxDistance = 480;

constexpr double kPixelScale = kUnrealHeight / kRealHeight;

// ---------------------------------------------------------------------------
// Small types
// ---------------------------------------------------------------------------

struct Vec2i {
    int x;
    int y;
};

struct Aabb {
    int left;
    int top;
    int right;
    int bottom;
};

struct MapTile {
    int x;
    int y;
    int id;
    int x_amount;
    int y_amount;
    double u;
};

struct Enemy {
    int x;      // world pixels after createEnemy
    int y;
    int turn;   // +1 right, -1 left, 0 dead
};

struct Bullet {
    int x;      // screen pixels
    int y;
    int turn;
};

struct HeroState {
    int x;
    int y;
    double xx;
    double yy;
    double x0;
    double vx;
    double vy;
    int turn;
    bool is_fly;
    bool is_shoot;
    bool died;
    bool ending;
    bool passed;
    int score;
};

// ---------------------------------------------------------------------------
// Integrator (Inertia::move)
// ---------------------------------------------------------------------------

// s = v t + ½ a t², then v += a t. Returns unscaled displacement.
double inertia_move(double& v, double t, double a);

// Same step, then multiply by UNREAL/REAL so the result is pixels.
double inertia_move_pixels(double& v, double t, double a);

// ---------------------------------------------------------------------------
// Collision (Role::isHit / hitMap box construction)
// ---------------------------------------------------------------------------

bool point_in_aabb(int x, int y, const Aabb& box);

// Four inset vertices of a WIDTH×HEIGHT actor, matching role.cpp.
void actor_vertices(int x, int y, Vec2i out[4]);

bool actor_hits_aabb(int x, int y, const Aabb& box);

// Tile AABB. ids 8 and 10 use 2× tile size (pipes).
Aabb tile_aabb(const MapTile& tile);

bool tile_is_solid(int id);

// ---------------------------------------------------------------------------
// Commands
// ---------------------------------------------------------------------------

int pack_command(bool left, bool right, bool up, bool down, bool shoot,
                 bool esc);
bool has_command(int bits, int mask);

// ---------------------------------------------------------------------------
// Friction
// ---------------------------------------------------------------------------

// u = (V_MAX / T) / G for the tile's T bucket. World 3 maps ids 1–6 to T1.
double friction_u(int tile_id, int world);

// a1 in unscaled units. Zero if v == 0 or accelerating along v.
double friction_accel(double vx, double a, double u, bool on_floor);

// ---------------------------------------------------------------------------
// Hero helpers
// ---------------------------------------------------------------------------

HeroState make_hero();

// vY = -sqrt(2 G REAL_HEIGHT)
void apply_jump(HeroState& hero);

int world_x(const HeroState& hero);

// Distances from Scene::isEnding.
int ending_distance(int world);
bool is_ending_distance(int world, int distance);

// Pin screen x to [0, 192] and accumulate x0, matching Role::action.
void pin_camera(HeroState& hero);

// One horizontal gameplay step: input accel + friction + clamp + pin.
// floor_u is used only when on_floor is true.
void tick_horizontal(HeroState& hero, int cmd, bool on_floor, double floor_u);

// One vertical air/ground step. on_floor_probe is the result of
// hitMap(x, y+1). Returns true if a landing snap happened this tick.
bool tick_vertical(HeroState& hero, bool on_floor_probe);

// ---------------------------------------------------------------------------
// Bullets
// ---------------------------------------------------------------------------

void bullet_step(Bullet& b);
bool bullet_expired_screen(const Bullet& b);

// ---------------------------------------------------------------------------
// Save file (gameRecord.dat)
// ---------------------------------------------------------------------------

bool write_save(const char* path, int world);
bool read_save(const char* path, int& world);
bool world_is_valid(int world);

// ---------------------------------------------------------------------------
// Authored world tables (tile units unless noted)
// ---------------------------------------------------------------------------

extern const MapTile kWorld1Map[];
extern const int kWorld1MapCount;
extern const Vec2i kWorld1Coins[];
extern const int kWorld1CoinCount;
extern const Vec2i kWorld1FoodPx[];
extern const int kWorld1FoodCount;
extern const Enemy kWorld1EnemiesTiles[];
extern const int kWorld1EnemyCount;

extern const MapTile kWorld2Map[];
extern const int kWorld2MapCount;

// How many records Scene::createMap actually keeps.
int copied_map_count(int authored_count);

// Fill u on a tile the same way createMap does.
void assign_friction(MapTile& tile, int world);

// World-pixel position of a tile-space enemy.
Enemy enemy_from_tile(const Enemy& tile_enemy);

}  // namespace maoliao

#endif
