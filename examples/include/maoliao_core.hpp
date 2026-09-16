// Portable extract of MaoLiAo constants and small helpers.
// Copied from MaoLiAo/define.h, inertia.cpp, role.cpp, scene.cpp.
// No EasyX, no Win32. Used only by the personal examples/.

#pragma once

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace maoliao {

constexpr int kXSize = 512;
constexpr int kYSize = 384;
constexpr int kWidth = 32;
constexpr int kHeight = 32;
constexpr int kX0 = 2 * kWidth;
constexpr int kY0 = 3 * kHeight; // authored, unused by Role::Role
constexpr double kTime = 0.01;
constexpr int kStep = 10;
constexpr int kEnemyStep = 1;
constexpr int kMapBg = 5;
constexpr int kXLeft = 0;
constexpr int kXRight = kWidth * 6;
constexpr double kRealHeight = 3.5;
constexpr double kUnrealHeight = 3 * kHeight + 5; // 101
constexpr double kG = 30.0;
constexpr double kVMax = 8.0;
constexpr double kARole = 20.0;
constexpr double kT1 = 0.5;
constexpr double kT2 = 1.2;
constexpr double kT3 = 1.5;
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
constexpr int kBulletNumber = 30;
constexpr int kMaxDistance = 480;
constexpr int kLengthIntervalBullet = 4;
constexpr double kTimeIntervalBullet = 0.2;

constexpr double kPxPerMeter = kUnrealHeight / kRealHeight;

inline double friction_u(int tile_id, int world) {
    // Scene::createMap switch, worlds 1-2 vs 3.
    if (world == 3) {
        switch (tile_id) {
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
            return (kVMax / kT1) / kG;
        case 2:
            return (kVMax / kT2) / kG;
        default:
            return (kVMax / kT3) / kG;
        }
    }
    switch (tile_id) {
    case 1:
    case 3:
    case 4:
    case 5:
        return (kVMax / kT2) / kG;
    case 6:
        return (kVMax / kT1) / kG;
    case 2:
        return (kVMax / kT2) / kG;
    default:
        return (kVMax / kT3) / kG;
    }
}

// Inertia::move — updates v, returns displacement in "meters".
inline double inertia_move(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

inline double jump_impulse() {
    return -std::sqrt(2.0 * kG * kRealHeight);
}

inline int friction_k(double vx) {
    // Role::action: k = Xabs/vX - 3 when vx >= 0, else Xabs/vX + 3.
    if (vx == 0.0) {
        return 0;
    }
    const double xabs = (vx < 0.0) ? -vx : vx;
    if (vx < 0.0) {
        return static_cast<int>(xabs / vx + 3);
    }
    return static_cast<int>(xabs / vx - 3);
}

struct Point {
    int x = 0;
    int y = 0;
};

struct MapRec {
    int x = 0;
    int y = 0;
    int id = 0;
    int x_amount = 0;
    int y_amount = 0;
    double u = 0.0;
};

struct EnemyRec {
    int x = 0;
    int y = 0;
    int turn = 0;
};

// Role::isHit — any of 4 vertices inside the axis-aligned box p2[0]..p2[1].
inline bool is_hit(const Point p1[4], const Point p2[2]) {
    for (int i = 0; i < 4; ++i) {
        if (p1[i].x >= p2[0].x && p1[i].y >= p2[0].y && p1[i].x <= p2[1].x &&
            p1[i].y <= p2[1].y) {
            return true;
        }
    }
    return false;
}

inline void hero_vertices(int screen_x, int screen_y, int x0, Point out[4]) {
    out[0] = {-x0 + screen_x + 1, screen_y + 1};
    out[1] = {-x0 + screen_x + kWidth - 1, screen_y + 1};
    out[2] = {-x0 + screen_x + 1, screen_y + kHeight - 1};
    out[3] = {-x0 + screen_x + kWidth - 1, screen_y + kHeight - 1};
}

inline void tile_box(const MapRec& m, Point out[2]) {
    out[0] = {m.x * kWidth, m.y * kHeight};
    if (m.id == 10 || m.id == 8) {
        out[1] = {m.x * kWidth + m.x_amount * 2 * kWidth,
                  m.y * kHeight + m.y_amount * 2 * kHeight};
    } else {
        out[1] = {m.x * kWidth + m.x_amount * kWidth,
                  m.y * kHeight + m.y_amount * kHeight};
    }
}

inline bool hit_map(int screen_x, int screen_y, int x0, const MapRec* maps, int n,
                    int world, const MapRec** hit = nullptr) {
    Point r[4];
    hero_vertices(screen_x, screen_y, x0, r);
    for (int i = 0; i < n; ++i) {
        if (!(maps[i].id > 0 && maps[i].id < 11)) {
            continue;
        }
        Point m[2];
        tile_box(maps[i], m);
        if (is_hit(r, m)) {
            if (hit) {
                *hit = &maps[i];
            }
            (void)world;
            return true;
        }
    }
    if (hit) {
        *hit = nullptr;
    }
    return false;
}

inline bool is_ending(int world, int distance_px) {
    switch (world) {
    case 1:
    case 3:
        return distance_px > 94 * kWidth;
    case 2:
        return distance_px > 104 * kWidth;
    default:
        return false;
    }
}

inline int parse_save_world(const char* text, bool* ok) {
    int flag = 0;
    if (std::sscanf(text, "%d", &flag) != 1) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    if (flag >= 4 || flag <= 0) {
        if (ok) {
            *ok = false;
        }
        return flag;
    }
    if (ok) {
        *ok = true;
    }
    return flag;
}

inline std::string format_save_world(int world) {
    return std::to_string(world);
}

// --- authored tables (same literals as scene.cpp / role.cpp) ---

inline std::vector<MapRec> world1_maps() {
    const MapRec raw[] = {
        {0, 9, 1, 15, 1},   {18, 8, 1, 9, 1},  {27, 9, 5, 37, 1}, {67, 9, 6, 4, 1},
        {74, 9, 5, 1, 1},   {78, 9, 1, 28, 1}, {0, 10, 3, 15, 2}, {18, 9, 3, 9, 3},
        {27, 10, 4, 37, 2}, {67, 10, 3, 4, 2}, {74, 10, 4, 1, 2}, {78, 10, 3, 28, 2},
        {10, 6, 2, 4, 1},   {40, 4, 2, 3, 1},  {62, 6, 2, 1, 1},  {65, 4, 2, 5, 1},
        {36, 7, 10, 1, 1},  {45, 7, 10, 1, 1}, {80, 7, 10, 1, 1}, {90, 7, 10, 1, 1},
        {19, 6, 11, 1, 1},  {23, 6, 11, 1, 1}, {32, 7, 11, 1, 1}, {49, 7, 11, 1, 1},
        {53, 7, 11, 1, 1},  {57, 7, 11, 1, 1}, {93, 7, 11, 1, 1}, {101, 7, 12, 1, 1},
        {15, 10, 13, 1, 1}, {64, 10, 13, 1, 1}, {71, 10, 13, 1, 1}, {75, 10, 13, 1, 1},
    };
    std::vector<MapRec> out;
    const int n = static_cast<int>(sizeof(raw) / sizeof(raw[0]));
    for (int i = 0; i < n && static_cast<int>(out.size()) < kMapNumber; ++i) {
        MapRec m = raw[i];
        m.u = friction_u(m.id, 1);
        out.push_back(m);
    }
    return out;
}

inline std::vector<MapRec> world2_maps(int* truncated = nullptr) {
    const MapRec raw[] = {
        {18, 9, 2, 4, 1},   {23, 6, 2, 6, 1},   {25, 3, 2, 3, 1},   {31, 10, 2, 2, 1},
        {39, 7, 2, 1, 1},   {39, 4, 2, 1, 1},   {96, 7, 2, 2, 1},   {99, 5, 2, 2, 1},
        {102, 3, 2, 12, 1}, {0, 10, 5, 15, 1},  {38, 10, 5, 10, 1}, {50, 10, 5, 1, 1},
        {53, 10, 5, 1, 1},  {56, 10, 5, 1, 1},  {61, 10, 5, 10, 1}, {75, 10, 5, 1, 1},
        {82, 10, 5, 11, 1}, {0, 11, 4, 15, 2},  {38, 11, 4, 10, 2}, {50, 11, 4, 1, 2},
        {53, 11, 4, 1, 2},  {56, 11, 4, 1, 2},  {61, 11, 4, 10, 2}, {75, 11, 4, 1, 2},
        {82, 11, 4, 11, 2}, {67, 9, 2, 1, 1},   {68, 8, 2, 1, 2},   {69, 7, 2, 1, 3},
        {70, 6, 2, 1, 4},   {75, 6, 2, 1, 4},   {42, 8, 11, 1, 1},  {111, 1, 12, 1, 1},
        {4, 8, 14, 1, 1},   {9, 8, 14, 1, 1},   {83, 8, 14, 1, 1},  {89, 8, 14, 1, 1},
    };
    const int n = static_cast<int>(sizeof(raw) / sizeof(raw[0]));
    std::vector<MapRec> out;
    for (int i = 0; i < n && static_cast<int>(out.size()) < kMapNumber; ++i) {
        MapRec m = raw[i];
        m.u = friction_u(m.id, 2);
        out.push_back(m);
    }
    if (truncated) {
        *truncated = n - static_cast<int>(out.size());
    }
    return out;
}

inline std::vector<MapRec> world3_maps(unsigned seed, int columns = 7) {
    std::srand(seed);
    int height[10];
    int x[10];
    for (int i = 0; i < 10; ++i) {
        height[i] = std::rand() % 6 + 1; // random(1,7)
        x[i] = i * 10 + 10;
    }
    std::vector<MapRec> raw;
    const int use = columns < 10 ? columns : 10;
    for (int i = 0; i < use; ++i) {
        const int h = height[i];
        const int mx = x[i];
        const int top_shaft = h - 2 > 0 ? h - 2 : 0;
        const int bot_shaft = 6 - h > 0 ? 6 - h : 0;
        raw.push_back({mx, h - 2, 8, 1, 1});
        raw.push_back({mx, 0, 7, 1, top_shaft});
        raw.push_back({mx, 4 + h, 10, 1, 1});
        raw.push_back({mx, h + 6, 7, 1, bot_shaft});
    }
    raw.push_back({80, 6, 2, 25, 1});
    raw.push_back({101, 4, 12, 1, 1});
    std::vector<MapRec> out;
    for (std::size_t i = 0; i < raw.size() && static_cast<int>(out.size()) < kMapNumber; ++i) {
        MapRec m = raw[i];
        m.u = friction_u(m.id, 3);
        out.push_back(m);
    }
    return out;
}

inline std::vector<Point> world1_coins() {
    return {{10, 5}, {11, 5}, {12, 5}, {13, 5}, {66, 3}, {67, 3}, {68, 3},
            {69, 3}, {74, 5}, {74, 6}, {74, 7}, {74, 8}, {82, 7}, {83, 8},
            {84, 7}, {85, 8}, {86, 7}, {87, 8}, {88, 7}, {89, 8}};
}

inline std::vector<Point> world2_coins() {
    return {{25, 2}, {26, 2}, {27, 2}, {32, 4}, {32, 5}, {32, 6},
            {32, 7}, {32, 8}, {50, 6}, {53, 6}, {56, 6}};
}

inline std::vector<Point> world3_coins(unsigned seed) {
    std::srand(seed);
    int height[10];
    for (int i = 0; i < 10; ++i) {
        height[i] = std::rand() % 4 + 3; // random(3,7)
    }
    return {{5, height[0]},  {15, height[1]}, {25, height[2]}, {35, height[3]},
            {45, height[4]}, {55, height[5]}, {65, height[6]}};
}

inline std::vector<EnemyRec> world1_enemies() {
    return {{3, 8, 1},  {18, 7, -1}, {25, 7, 1}, {28, 8, -1}, {33, 8, -1},
            {39, 8, 1}, {68, 3, 1},  {66, 8, -1}, {81, 6, 1}, {92, 6, 1}};
}

inline std::vector<EnemyRec> world2_enemies() {
    return {{18, 8, 1}, {24, 5, 1}, {28, 5, -1}, {44, 9, 1}, {97, 6, 1}, {99, 4, 1}};
}

inline std::vector<EnemyRec> world3_enemies() {
    return {{24, 4, -1}, {34, 6, 1}, {37, 3, -1}, {43, 5, -1},
            {63, 7, 1},  {67, 8, -1}, {86, 5, 1}};
}

inline const char* tile_name(int id) {
    switch (id) {
    case 1:
        return "grass_top";
    case 2:
        return "cloud";
    case 3:
        return "dirt";
    case 4:
        return "snow_fill";
    case 5:
        return "snow_top";
    case 6:
        return "sticky_or_pipe";
    case 7:
        return "pipe_shaft";
    case 8:
        return "pipe_mouth_down";
    case 9:
        return "pipe_unused";
    case 10:
        return "pipe_mouth_up";
    case 11:
        return "decor_grass";
    case 12:
        return "goal_sign";
    case 13:
        return "water";
    case 14:
        return "tree";
    default:
        return "unknown";
    }
}

inline bool collides(int id) { return id > 0 && id < 11; }

} // namespace maoliao
