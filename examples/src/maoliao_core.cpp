#include "maoliao_core.hpp"

#include <cmath>
#include <cstdio>
#include <cstring>

namespace maoliao {

double inertia_move(double& v, double t, double a)
{
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

double inertia_move_pixels(double& v, double t, double a)
{
    return inertia_move(v, t, a) * kPixelScale;
}

bool point_in_aabb(int x, int y, const Aabb& box)
{
    return x >= box.left && y >= box.top && x <= box.right && y <= box.bottom;
}

void actor_vertices(int x, int y, Vec2i out[4])
{
    out[0].x = x + 1;
    out[0].y = y + 1;
    out[1].x = x + kWidth - 1;
    out[1].y = y + 1;
    out[2].x = x + 1;
    out[2].y = y + kHeight - 1;
    out[3].x = x + kWidth - 1;
    out[3].y = y + kHeight - 1;
}

bool actor_hits_aabb(int x, int y, const Aabb& box)
{
    Vec2i v[4];
    actor_vertices(x, y, v);
    for (int i = 0; i < 4; ++i) {
        if (point_in_aabb(v[i].x, v[i].y, box)) {
            return true;
        }
    }
    return false;
}

bool tile_is_solid(int id)
{
    return id > 0 && id < 11;
}

Aabb tile_aabb(const MapTile& tile)
{
    Aabb box;
    box.left = tile.x * kWidth;
    box.top = tile.y * kHeight;
    const int cell_w = (tile.id == 10 || tile.id == 8) ? (2 * kWidth) : kWidth;
    const int cell_h = (tile.id == 10 || tile.id == 8) ? (2 * kHeight) : kHeight;
    box.right = box.left + tile.x_amount * cell_w;
    box.bottom = box.top + tile.y_amount * cell_h;
    return box;
}

int pack_command(bool left, bool right, bool up, bool down, bool shoot,
                 bool esc)
{
    int c = 0;
    if (left) {
        c |= kCmdLeft;
    }
    if (right) {
        c |= kCmdRight;
    }
    if (up) {
        c |= kCmdUp;
    }
    if (down) {
        c |= kCmdDown;
    }
    if (shoot) {
        c |= kCmdShoot;
    }
    if (esc) {
        c |= kCmdEsc;
    }
    return c;
}

bool has_command(int bits, int mask)
{
    return (bits & mask) != 0;
}

double friction_u(int tile_id, int world)
{
    if (world == 3) {
        // scene.cpp world 3: ids 1,3,4,5,6 → T1; id 2 → T2; else T3.
        // id 2 must not fall into the 1..6 bucket.
        if (tile_id == 1 || (tile_id >= 3 && tile_id <= 6)) {
            return (kVMax / kT1) / kG;
        }
        if (tile_id == 2) {
            return (kVMax / kT2) / kG;
        }
        return (kVMax / kT3) / kG;
    }

    switch (tile_id) {
    case 1:
    case 3:
    case 4:
    case 5:
    case 2:
        return (kVMax / kT2) / kG;
    case 6:
        return (kVMax / kT1) / kG;
    default:
        return (kVMax / kT3) / kG;
    }
}

void assign_friction(MapTile& tile, int world)
{
    tile.u = friction_u(tile.id, world);
}

double friction_accel(double vx, double a, double u, bool on_floor)
{
    if (vx == 0.0) {
        return 0.0;
    }
    if (vx * a > 0.0) {
        return 0.0;
    }
    if (!on_floor) {
        return 0.0;
    }
    const double xabs = (vx < 0.0) ? -vx : vx;
    // role.cpp: k = Xabs/vX + 3 when vX < 0, else Xabs/vX - 3
    const double k = (vx < 0.0) ? (xabs / vx + 3.0) : (xabs / vx - 3.0);
    return k * kG * u;
}

HeroState make_hero()
{
    HeroState h;
    h.x = kX0;
    h.y = kX0;  // constructor uses X0 for both; Y0 is unused at spawn
    h.xx = h.x;
    h.yy = h.y;
    h.x0 = 0.0;
    h.vx = 0.0;
    h.vy = 0.0;
    h.turn = 1;
    h.is_fly = true;
    h.is_shoot = false;
    h.died = false;
    h.ending = false;
    h.passed = false;
    h.score = 0;
    return h;
}

void apply_jump(HeroState& hero)
{
    hero.is_fly = true;
    hero.vy = -std::sqrt(2.0 * kG * kRealHeight);
}

int world_x(const HeroState& hero)
{
    return hero.x - static_cast<int>(hero.x0);
}

int ending_distance(int world)
{
    switch (world) {
    case 1:
    case 3:
        return 94 * kWidth;
    case 2:
        return 104 * kWidth;
    default:
        return 0;
    }
}

bool is_ending_distance(int world, int distance)
{
    return distance > ending_distance(world);
}

void pin_camera(HeroState& hero)
{
    if (hero.x < kXLeft) {
        hero.x = kXLeft;
        hero.xx = hero.x;
        hero.vx = 0.0;
        return;
    }
    if (hero.x > kXRight && !hero.ending) {
        hero.x0 -= (hero.x - kXRight);
        hero.x = kXRight;
        hero.xx = hero.x;
    }
    if (hero.x > kXSize) {
        hero.passed = true;
    }
}

void tick_horizontal(HeroState& hero, int cmd, bool on_floor, double floor_u)
{
    double a = 0.0;
    if (has_command(cmd, kCmdLeft) && !hero.ending) {
        a -= kARole;
        hero.turn = -1;
    }
    if (has_command(cmd, kCmdRight) || hero.ending) {
        a += kARole;
        hero.turn = 1;
    }

    const double a1 = friction_accel(hero.vx, a, floor_u, on_floor);
    const double tmp = hero.vx;
    const double pixels = inertia_move_pixels(hero.vx, kTime, a + a1);
    if (tmp * hero.vx < 0.0) {
        hero.vx = 0.0;
    }

    double xabs = hero.vx;
    if (xabs < 0.0) {
        xabs = -xabs;
    }
    if (xabs > kVMax) {
        hero.vx = hero.vx / xabs * kVMax;
    }

    hero.xx += pixels;
    hero.x = static_cast<int>(hero.xx);

    if (is_ending_distance(1, world_x(hero))) {
        // Caller overrides world; tick_horizontal is world-agnostic besides
        // ending pin. Ending flag is set by the example / game loop.
    }
    pin_camera(hero);
}

bool tick_vertical(HeroState& hero, bool on_floor_probe)
{
    if (!hero.is_fly) {
        if (!on_floor_probe) {
            hero.is_fly = true;
        }
        return false;
    }

    hero.yy += inertia_move_pixels(hero.vy, kTime, kG);
    hero.y = static_cast<int>(hero.yy);

    if (on_floor_probe && hero.vy > 0.0) {
        hero.is_fly = false;
        hero.vy = 0.0;
        hero.y = (hero.y + kHeight / 2) / kHeight * kHeight;
        hero.yy = hero.y;
        return true;
    }
    if (hero.y > kYSize) {
        hero.died = true;
    }
    return false;
}

void bullet_step(Bullet& b)
{
    if (b.x == 0 && b.y == 0) {
        return;
    }
    b.x += kLengthIntervalBullet * b.turn;
}

bool bullet_expired_screen(const Bullet& b)
{
    if (b.x == 0 && b.y == 0) {
        return true;
    }
    if (b.x > kXSize || b.x < -kWidth) {
        return true;
    }
    if (b.x > kMaxDistance) {
        return true;
    }
    return false;
}

bool world_is_valid(int world)
{
    return world >= 1 && world <= 3;
}

bool write_save(const char* path, int world)
{
    if (!path || !world_is_valid(world)) {
        return false;
    }
    FILE* fp = std::fopen(path, "w");
    if (!fp) {
        return false;
    }
    const int n = std::fprintf(fp, "%d", world);
    std::fclose(fp);
    return n > 0;
}

bool read_save(const char* path, int& world)
{
    if (!path) {
        return false;
    }
    FILE* fp = std::fopen(path, "r");
    if (!fp) {
        return false;
    }
    int flag = 0;
    const int n = std::fscanf(fp, "%d", &flag);
    std::fclose(fp);
    if (n != 1 || !world_is_valid(flag)) {
        return false;
    }
    world = flag;
    return true;
}

int copied_map_count(int authored_count)
{
    return authored_count < kMapNumber ? authored_count : kMapNumber;
}

Enemy enemy_from_tile(const Enemy& tile_enemy)
{
    Enemy e;
    e.x = tile_enemy.x * kWidth;
    e.y = tile_enemy.y * kHeight;
    e.turn = tile_enemy.turn;
    return e;
}

// World 1 authored list — same literals as Scene::createMap(1).
const MapTile kWorld1Map[] = {
    {0, 9, 1, 15, 1, 0},   {18, 8, 1, 9, 1, 0},  {27, 9, 5, 37, 1, 0},
    {67, 9, 6, 4, 1, 0},   {74, 9, 5, 1, 1, 0},  {78, 9, 1, 28, 1, 0},
    {0, 10, 3, 15, 2, 0},  {18, 9, 3, 9, 3, 0},  {27, 10, 4, 37, 2, 0},
    {67, 10, 3, 4, 2, 0},  {74, 10, 4, 1, 2, 0}, {78, 10, 3, 28, 2, 0},
    {10, 6, 2, 4, 1, 0},   {40, 4, 2, 3, 1, 0},  {62, 6, 2, 1, 1, 0},
    {65, 4, 2, 5, 1, 0},   {36, 7, 10, 1, 1, 0}, {45, 7, 10, 1, 1, 0},
    {80, 7, 10, 1, 1, 0},  {90, 7, 10, 1, 1, 0}, {19, 6, 11, 1, 1, 0},
    {23, 6, 11, 1, 1, 0},  {32, 7, 11, 1, 1, 0}, {49, 7, 11, 1, 1, 0},
    {53, 7, 11, 1, 1, 0},  {57, 7, 11, 1, 1, 0}, {93, 7, 11, 1, 1, 0},
    {101, 7, 12, 1, 1, 0}, {15, 10, 13, 1, 1, 0},{64, 10, 13, 1, 1, 0},
    {71, 10, 13, 1, 1, 0}, {75, 10, 13, 1, 1, 0},
};

const int kWorld1MapCount = static_cast<int>(sizeof(kWorld1Map) / sizeof(kWorld1Map[0]));

const Vec2i kWorld1Coins[] = {
    {10, 5}, {11, 5}, {12, 5}, {13, 5}, {66, 3}, {67, 3}, {68, 3}, {69, 3},
    {74, 5}, {74, 6}, {74, 7}, {74, 8}, {82, 7}, {83, 8}, {84, 7}, {85, 8},
    {86, 7}, {87, 8}, {88, 7}, {89, 8},
};

const int kWorld1CoinCount =
    static_cast<int>(sizeof(kWorld1Coins) / sizeof(kWorld1Coins[0]));

const Vec2i kWorld1FoodPx[] = {{14 * kWidth, 5 * kHeight}};
const int kWorld1FoodCount = 1;

const Enemy kWorld1EnemiesTiles[] = {
    {3, 8, 1},  {18, 7, -1}, {25, 7, 1}, {28, 8, -1}, {33, 8, -1},
    {39, 8, 1}, {68, 3, 1},  {66, 8, -1}, {81, 6, 1}, {92, 6, 1},
};

const int kWorld1EnemyCount =
    static_cast<int>(sizeof(kWorld1EnemiesTiles) / sizeof(kWorld1EnemiesTiles[0]));

const MapTile kWorld2Map[] = {
    {18, 9, 2, 4, 1, 0},   {23, 6, 2, 6, 1, 0},  {25, 3, 2, 3, 1, 0},
    {31, 10, 2, 2, 1, 0},  {39, 7, 2, 1, 1, 0},  {39, 4, 2, 1, 1, 0},
    {96, 7, 2, 2, 1, 0},   {99, 5, 2, 2, 1, 0},  {102, 3, 2, 12, 1, 0},
    {0, 10, 5, 15, 1, 0},  {38, 10, 5, 10, 1, 0},{50, 10, 5, 1, 1, 0},
    {53, 10, 5, 1, 1, 0},  {56, 10, 5, 1, 1, 0}, {61, 10, 5, 10, 1, 0},
    {75, 10, 5, 1, 1, 0},  {82, 10, 5, 11, 1, 0},{0, 11, 4, 15, 2, 0},
    {38, 11, 4, 10, 2, 0}, {50, 11, 4, 1, 2, 0}, {53, 11, 4, 1, 2, 0},
    {56, 11, 4, 1, 2, 0},  {61, 11, 4, 10, 2, 0},{75, 11, 4, 1, 2, 0},
    {82, 11, 4, 11, 2, 0}, {67, 9, 2, 1, 1, 0},  {68, 8, 2, 1, 2, 0},
    {69, 7, 2, 1, 3, 0},   {70, 6, 2, 1, 4, 0},  {75, 6, 2, 1, 4, 0},
    {42, 8, 11, 1, 1, 0},  {111, 1, 12, 1, 1, 0},{4, 8, 14, 1, 1, 0},
    {9, 8, 14, 1, 1, 0},   {83, 8, 14, 1, 1, 0}, {89, 8, 14, 1, 1, 0},
};

const int kWorld2MapCount = static_cast<int>(sizeof(kWorld2Map) / sizeof(kWorld2Map[0]));

}  // namespace maoliao
