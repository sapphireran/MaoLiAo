#pragma once
#ifndef MAOLIAO_MODEL_HPP
#define MAOLIAO_MODEL_HPP

// Portable reconstruction of MaoLiAo's numbers and rules.
// Windows / EasyX are not required. Values match MaoLiAo/define.h and the
// formulas in inertia.cpp, role.cpp, scene.cpp, and control.cpp.

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

namespace maoliao {

// --- screen and tiles (define.h) -------------------------------------------

constexpr int XSIZE = 512;
constexpr int YSIZE = 384;
constexpr int WIDTH = 32;
constexpr int HEIGHT = 32;
constexpr int X0 = 2 * WIDTH;          // 64; spawn x (and, in the game, spawn y)
constexpr int Y0 = 3 * HEIGHT;         // 96; authored but unused by Role::Role
constexpr double TIME = 0.01;
constexpr int STEP = 10;
constexpr int ENEMY_STEP = 1;
constexpr int K_MAP_BG = 5;
constexpr int XLEFT = 0;
constexpr int XRIGHT = WIDTH * 6;      // 192
constexpr double REAL_HEIGHT = 3.5;
constexpr double UNREAL_HEIGHT = 3 * HEIGHT + 5; // 101
constexpr double G = 30.0;
constexpr double V_MAX = 8.0;
constexpr double A_ROLE = 20.0;
constexpr double T1 = 0.5;
constexpr double T2 = 1.2;
constexpr double T3 = 1.5;
constexpr int LIFE = 5;

constexpr int MAP_NUMBER = 30;
constexpr int COINS_NUMBER = 70;
constexpr int FOOD_NUMBER = 5;
constexpr int ENEMY_TOTE = 30;
constexpr int BULLET_NUMBER = 30;
constexpr int BOMB_NUMBER = 5;
constexpr int MAX_DISTANCE = 480;
constexpr int BULLET_STEP = 4;         // LEHGTH_INTERVAL_BULLET
constexpr double BULLET_INTERVAL = 0.2;

constexpr int SCORE_COIN = 10;
constexpr int SCORE_ENEMY = 5;

constexpr int ENDING_W1 = 94 * WIDTH;
constexpr int ENDING_W2 = 104 * WIDTH;
constexpr int ENDING_W3 = 94 * WIDTH;

// --- command bits ----------------------------------------------------------

constexpr int CMD_LEFT = 1;
constexpr int CMD_RIGHT = 2;
constexpr int CMD_UP = 4;
constexpr int CMD_DOWN = 8;
constexpr int CMD_SHOOT = 16;
constexpr int CMD_ESC = 32;
constexpr int VIR_RETURN = 64;
constexpr int VIR_RESTART = 128;
constexpr int VIR_HOME = 256;

inline bool has_bit(int mask, int bit) { return (mask & bit) != 0; }

inline bool is_virtual_command(int mask) {
    return mask == VIR_RETURN || mask == VIR_RESTART || mask == VIR_HOME;
}

inline const char* command_name(int bit) {
    switch (bit) {
    case CMD_LEFT: return "CMD_LEFT";
    case CMD_RIGHT: return "CMD_RIGHT";
    case CMD_UP: return "CMD_UP";
    case CMD_DOWN: return "CMD_DOWN";
    case CMD_SHOOT: return "CMD_SHOOT";
    case CMD_ESC: return "CMD_ESC";
    case VIR_RETURN: return "VIR_RETURN";
    case VIR_RESTART: return "VIR_RESTART";
    case VIR_HOME: return "VIR_HOME";
    default: return "?";
    }
}

inline int encode_keys(bool left, bool right, bool up, bool down, bool shoot, bool esc) {
    int c = 0;
    if (left) c |= CMD_LEFT;
    if (right) c |= CMD_RIGHT;
    if (up) c |= CMD_UP;
    if (down) c |= CMD_DOWN;
    if (shoot) c |= CMD_SHOOT;
    if (esc) c |= CMD_ESC;
    return c;
}

// --- integrator (Inertia::move) --------------------------------------------

inline double integrate(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

inline double metres_to_pixels(double metres) {
    return metres * UNREAL_HEIGHT / REAL_HEIGHT;
}

inline double jump_speed() {
    return -std::sqrt(2.0 * G * REAL_HEIGHT);
}

inline double friction_u(double time_to_vmax) {
    return (V_MAX / time_to_vmax) / G;
}

inline double tile_u_world12(int id) {
    switch (id) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        return friction_u(T2);
    case 6:
        return friction_u(T1);
    default:
        return friction_u(T3);
    }
}

inline double tile_u_world3(int id) {
    switch (id) {
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
        return friction_u(T1);
    case 2:
        return friction_u(T2);
    default:
        return friction_u(T3);
    }
}

// Horizontal friction scale used in Role::action (the +3/-3 helper).
inline double friction_k(double v_x) {
    if (v_x == 0.0) return 0.0;
    const double xabs = (v_x < 0.0) ? -v_x : v_x;
    if (v_x < 0.0) return xabs / v_x + 3.0; // -1 + 3 = 2
    return xabs / v_x - 3.0;                //  1 - 3 = -2
}

struct Vec2 {
    double x = 0;
    double y = 0;
};

struct Point {
    int x = 0;
    int y = 0;
};

struct Box {
    int x0 = 0;
    int y0 = 0;
    int x1 = 0;
    int y1 = 0;
};

inline Box tile_box(int tile_x, int tile_y, int x_amount, int y_amount, int id) {
    Box b;
    b.x0 = tile_x * WIDTH;
    b.y0 = tile_y * HEIGHT;
    const int cell_w = (id == 8 || id == 10) ? 2 * WIDTH : WIDTH;
    const int cell_h = (id == 8 || id == 10) ? 2 * HEIGHT : HEIGHT;
    b.x1 = b.x0 + x_amount * cell_w;
    b.y1 = b.y0 + y_amount * cell_h;
    return b;
}

inline bool collides_as_tile(int id) { return id > 0 && id < 11; }

// Four inset corners vs a [tl, br] box — Role::isHit.
inline bool corner_hits(const Point corners[4], const Box& box) {
    for (int i = 0; i < 4; ++i) {
        if (corners[i].x >= box.x0 && corners[i].y >= box.y0 &&
            corners[i].x <= box.x1 && corners[i].y <= box.y1) {
            return true;
        }
    }
    return false;
}

inline void hero_corners_world(int screen_x, int screen_y, double x0, Point out[4]) {
    const int xmap = static_cast<int>(x0);
    out[0] = {-xmap + screen_x + 1, screen_y + 1};
    out[1] = {-xmap + screen_x + WIDTH - 1, screen_y + 1};
    out[2] = {-xmap + screen_x + 1, screen_y + HEIGHT - 1};
    out[3] = {-xmap + screen_x + WIDTH - 1, screen_y + HEIGHT - 1};
}

inline bool hero_hits_box(int screen_x, int screen_y, double x0, const Box& box) {
    Point c[4];
    hero_corners_world(screen_x, screen_y, x0, c);
    return corner_hits(c, box);
}

// World-3 contact kill: any colliding tile except clouds, unless armed.
inline bool world3_contact_kills(int tile_id, bool is_shoot) {
    if (is_shoot) return false;
    if (tile_id == 2) return false;
    return collides_as_tile(tile_id);
}

// --- camera ----------------------------------------------------------------

struct CameraStep {
    double x0;
    int screen_x;
    double sky_dx;
};

inline CameraStep apply_rail(double x0, double xx, double v_x, bool ending) {
    CameraStep s;
    s.x0 = x0;
    s.screen_x = static_cast<int>(xx);
    s.sky_dx = 0.0;
    if (s.screen_x < XLEFT) {
        s.screen_x = XLEFT;
        return s;
    }
    if (s.screen_x > XRIGHT && !ending) {
        s.x0 -= (s.screen_x - XRIGHT);
        s.screen_x = XRIGHT;
        if (v_x > 0.0) {
            s.sky_dx = -(std::fabs(v_x) * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG);
        }
    }
    return s;
}

inline int world_distance(int screen_x, double x0) {
    return screen_x - static_cast<int>(x0);
}

inline int ending_distance(int world) {
    switch (world) {
    case 1: return ENDING_W1;
    case 2: return ENDING_W2;
    case 3: return ENDING_W3;
    default: return 0;
    }
}

inline bool is_ending(int world, int distance) {
    return distance > ending_distance(world);
}

// --- maps ------------------------------------------------------------------

struct MapRec {
    int x = 0;
    int y = 0;
    int id = 0;
    int x_amount = 0;
    int y_amount = 0;
};

inline const char* tile_name(int id) {
    switch (id) {
    case 1: return "grass-surface";
    case 2: return "cloud";
    case 3: return "dirt";
    case 4: return "snow-under";
    case 5: return "snow-surface";
    case 6: return "pipe-mid";
    case 7: return "pipe-shaft";
    case 8: return "pipe-mouth-down";
    case 9: return "pipe-mouth-up-alt";
    case 10: return "pipe-mouth-up";
    case 11: return "bg-grass";
    case 12: return "victory-sign";
    case 13: return "water";
    case 14: return "tree";
    default: return "unknown";
    }
}

inline std::vector<MapRec> world1_map() {
    return {
        {0, 9, 1, 15, 1},   {18, 8, 1, 9, 1},  {27, 9, 5, 37, 1},
        {67, 9, 6, 4, 1},   {74, 9, 5, 1, 1},  {78, 9, 1, 28, 1},
        {0, 10, 3, 15, 2},  {18, 9, 3, 9, 3},  {27, 10, 4, 37, 2},
        {67, 10, 3, 4, 2},  {74, 10, 4, 1, 2}, {78, 10, 3, 28, 2},
        {10, 6, 2, 4, 1},   {40, 4, 2, 3, 1},  {62, 6, 2, 1, 1},
        {65, 4, 2, 5, 1},   {36, 7, 10, 1, 1}, {45, 7, 10, 1, 1},
        {80, 7, 10, 1, 1},  {90, 7, 10, 1, 1}, {19, 6, 11, 1, 1},
        {23, 6, 11, 1, 1},  {32, 7, 11, 1, 1}, {49, 7, 11, 1, 1},
        {53, 7, 11, 1, 1},  {57, 7, 11, 1, 1}, {93, 7, 11, 1, 1},
        {101, 7, 12, 1, 1}, {15, 10, 13, 1, 1},{64, 10, 13, 1, 1},
        {71, 10, 13, 1, 1}, {75, 10, 13, 1, 1},
    };
}

inline std::vector<MapRec> world2_map() {
    return {
        {18, 9, 2, 4, 1},   {23, 6, 2, 6, 1},  {25, 3, 2, 3, 1},
        {31, 10, 2, 2, 1},  {39, 7, 2, 1, 1},  {39, 4, 2, 1, 1},
        {96, 7, 2, 2, 1},   {99, 5, 2, 2, 1},  {102, 3, 2, 12, 1},
        {0, 10, 5, 15, 1},  {38, 10, 5, 10, 1},{50, 10, 5, 1, 1},
        {53, 10, 5, 1, 1},  {56, 10, 5, 1, 1}, {61, 10, 5, 10, 1},
        {75, 10, 5, 1, 1},  {82, 10, 5, 11, 1},{0, 11, 4, 15, 2},
        {38, 11, 4, 10, 2}, {50, 11, 4, 1, 2}, {53, 11, 4, 1, 2},
        {56, 11, 4, 1, 2},  {61, 11, 4, 10, 2},{75, 11, 4, 1, 2},
        {82, 11, 4, 11, 2}, {67, 9, 2, 1, 1},  {68, 8, 2, 1, 2},
        {69, 7, 2, 1, 3},   {70, 6, 2, 1, 4},  {75, 6, 2, 1, 4},
        {42, 8, 11, 1, 1},  {111, 1, 12, 1, 1},{4, 8, 14, 1, 1},
        {9, 8, 14, 1, 1},   {83, 8, 14, 1, 1}, {89, 8, 14, 1, 1},
    };
}

// Deterministic world-3 pipes for a given height list (the game uses rand()).
inline std::vector<MapRec> world3_map_from_heights(const int height[7]) {
    std::vector<MapRec> m;
    m.reserve(30);
    for (int i = 0; i < 7; ++i) {
        const int x = i * 10 + 10;
        const int h = height[i];
        m.push_back({x, h - 2, 8, 1, 1});
        m.push_back({x, 0, 7, 1, std::max(0, h - 2)});
        m.push_back({x, 4 + h, 10, 1, 1});
        m.push_back({x, h + 6, 7, 1, std::max(0, 6 - h)});
    }
    m.push_back({80, 6, 2, 25, 1});
    m.push_back({101, 4, 12, 1, 1});
    return m;
}

inline std::vector<MapRec> loaded_map(const std::vector<MapRec>& authored) {
    std::vector<MapRec> live;
    for (const auto& rec : authored) {
        if (static_cast<int>(live.size()) >= MAP_NUMBER) break;
        if (rec.id <= 0 || rec.id >= 15) break;
        live.push_back(rec);
    }
    return live;
}

inline std::vector<Point> world1_coins() {
    return {
        {10, 5}, {11, 5}, {12, 5}, {13, 5}, {66, 3}, {67, 3}, {68, 3}, {69, 3},
        {74, 5}, {74, 6}, {74, 7}, {74, 8}, {82, 7}, {83, 8}, {84, 7}, {85, 8},
        {86, 7}, {87, 8}, {88, 7}, {89, 8},
    };
}

inline std::vector<Point> world2_coins() {
    return {
        {25, 2}, {26, 2}, {27, 2}, {32, 4}, {32, 5}, {32, 6},
        {32, 7}, {32, 8}, {50, 6}, {53, 6}, {56, 6},
    };
}

struct EnemyRec {
    int tile_x = 0;
    int tile_y = 0;
    int turn = 0;
};

inline std::vector<EnemyRec> world1_enemies() {
    return {
        {3, 8, 1}, {18, 7, -1}, {25, 7, 1}, {28, 8, -1}, {33, 8, -1},
        {39, 8, 1}, {68, 3, 1}, {66, 8, -1}, {81, 6, 1}, {92, 6, 1},
    };
}

inline std::vector<EnemyRec> world2_enemies() {
    return {
        {18, 8, 1}, {24, 5, 1}, {28, 5, -1}, {44, 9, 1}, {97, 6, 1}, {99, 4, 1},
    };
}

inline std::vector<EnemyRec> world3_enemies() {
    return {
        {24, 4, -1}, {34, 6, 1}, {37, 3, -1}, {43, 5, -1},
        {63, 7, 1}, {67, 8, -1}, {86, 5, 1},
    };
}

// --- scoring / save --------------------------------------------------------

enum class Pickup { Coin, Stomp, Shot };

inline int score_for(Pickup p) {
    return (p == Pickup::Coin) ? SCORE_COIN : SCORE_ENEMY;
}

inline int apply_pickup(int score, Pickup p) { return score + score_for(p); }

struct SaveParse {
    bool ok = false;
    int world = 0;
    std::string error;
};

inline SaveParse parse_save(const std::string& text) {
    SaveParse r;
    std::istringstream in(text);
    int flag = 0;
    if (!(in >> flag)) {
        r.error = "not an integer";
        return r;
    }
    if (flag <= 0 || flag >= 4) {
        r.error = "world out of range";
        r.world = flag;
        return r;
    }
    r.ok = true;
    r.world = flag;
    return r;
}

inline std::string format_save(int world) { return std::to_string(world); }

// --- jump trajectory -------------------------------------------------------

struct JumpSample {
    int tick = 0;
    double v = 0;
    double metres = 0;
    double pixels = 0;
};

inline std::vector<JumpSample> simulate_jump(int max_ticks = 200) {
    std::vector<JumpSample> out;
    double v = jump_speed();
    double metres = 0.0;
    out.push_back({0, v, 0.0, 0.0});
    for (int i = 1; i <= max_ticks; ++i) {
        metres += integrate(v, TIME, G);
        out.push_back({i, v, metres, metres_to_pixels(metres)});
        if (v > 0.0 && metres >= 0.0) break; // back at / below takeoff
    }
    return out;
}

inline const JumpSample& apex(const std::vector<JumpSample>& samples) {
    return *std::min_element(samples.begin(), samples.end(),
                             [](const JumpSample& a, const JumpSample& b) {
                                 return a.metres < b.metres;
                             });
}

// --- run along the rail ----------------------------------------------------

struct RunSample {
    int tick = 0;
    double v_x = 0;
    double xx = 0;
    double x0 = 0;
    int screen_x = 0;
    int distance = 0;
};

inline std::vector<RunSample> simulate_hold_right(int ticks, bool on_ground = true,
                                                  double u = friction_u(T2)) {
    (void)u;
    std::vector<RunSample> out;
    double v = 0.0;
    double xx = static_cast<double>(X0);
    double x0 = 0.0;
    out.push_back({0, v, xx, x0, static_cast<int>(xx), world_distance(static_cast<int>(xx), x0)});
    for (int i = 1; i <= ticks; ++i) {
        double a = A_ROLE;
        double a1 = 0.0;
        if (on_ground && v * a <= 0.0 && v != 0.0) {
            a1 = friction_k(v) * G * u;
        }
        const double tmp = v;
        const double h = metres_to_pixels(integrate(v, TIME, a + a1));
        if (tmp * v < 0.0) v = 0.0;
        xx += h;
        if (std::fabs(v) > V_MAX) v = (v / std::fabs(v)) * V_MAX;
        auto cam = apply_rail(x0, xx, v, false);
        x0 = cam.x0;
        xx = cam.screen_x;
        out.push_back({i, v, xx, x0, cam.screen_x, world_distance(cam.screen_x, x0)});
    }
    return out;
}

} // namespace maoliao

#endif
