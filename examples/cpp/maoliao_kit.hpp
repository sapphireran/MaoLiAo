// Single-header personal kit for MaoLiAo V2.0 numbers and hero math.
// No EasyX. C++17. Include from one translation unit per demo, or
// keep everything inline as written here.
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace maoliao {

constexpr int XSIZE = 512;
constexpr int YSIZE = 384;
constexpr int WIDTH = 32;
constexpr int HEIGHT = 32;
constexpr int X0 = 2 * WIDTH;
constexpr int Y0 = 3 * HEIGHT;
constexpr double TIME = 0.01;
constexpr int XLEFT = 0;
constexpr int XRIGHT = WIDTH * 6;
constexpr double REAL_HEIGHT = 3.5;
constexpr double UNREAL_HEIGHT = 3 * HEIGHT + 5;
constexpr double G = 30.0;
constexpr double V_MAX = 8.0;
constexpr double A_ROLE = 20.0;
constexpr double T1 = 0.5;
constexpr double T2 = 1.2;
constexpr double T3 = 1.5;
constexpr int LIFE = 5;
constexpr int MAP_NUMBER = 30;
constexpr int MAX_DISTANCE = 480;
constexpr int ENDING1 = 94;
constexpr int ENDING2 = 104;
constexpr int ENDING3 = 94;

constexpr int CMD_LEFT = 1;
constexpr int CMD_RIGHT = 2;
constexpr int CMD_UP = 4;
constexpr int CMD_DOWN = 8;
constexpr int CMD_SHOOT = 16;
constexpr int CMD_ESC = 32;

inline double pixel_scale() { return UNREAL_HEIGHT / REAL_HEIGHT; }

inline double friction_u(double t) { return (V_MAX / t) / G; }

inline double launch_vy() { return -std::sqrt(2.0 * G * REAL_HEIGHT); }

inline std::pair<double, double> inertia_move(double v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    return {x, v + a * t};
}

struct JumpSample {
    int frame;
    double t;
    double vY;
    double rise_px;
};

inline std::vector<JumpSample> jump_profile(int max_frames = 200) {
    double v = launch_vy();
    double y = 0.0;
    std::vector<JumpSample> out;
    for (int frame = 1; frame <= max_frames; ++frame) {
        auto [dx, nv] = inertia_move(v, TIME, G);
        v = nv;
        y += dx * pixel_scale();
        out.push_back({frame, frame * TIME, v, -y});
        if (y > 0 && frame > 2) break;
    }
    return out;
}

inline JumpSample apex(const std::vector<JumpSample>& s) {
    return *std::max_element(s.begin(), s.end(),
                             [](auto& a, auto& b) { return a.rise_px < b.rise_px; });
}

struct Tile {
    int x, y, id, xAmount, yAmount;
};

inline bool solid(const Tile& t) { return t.id > 0 && t.id < 11; }

inline void tile_aabb(const Tile& t, double& x0, double& y0, double& x1, double& y1) {
    x0 = t.x * WIDTH;
    y0 = t.y * HEIGHT;
    if (t.id == 8 || t.id == 10) {
        x1 = x0 + t.xAmount * 2 * WIDTH;
        y1 = y0 + t.yAmount * 2 * HEIGHT;
    } else {
        x1 = x0 + t.xAmount * WIDTH;
        y1 = y0 + t.yAmount * HEIGHT;
    }
}

inline bool aabb_contains(double x0, double y0, double x1, double y1, double px, double py) {
    return px >= x0 && px <= x1 && py >= y0 && py <= y1;
}

inline bool is_hit(double world_x, double y, double x0, double y0, double x1, double y1) {
    const double xs[4] = {world_x + 1, world_x + WIDTH - 1, world_x + 1, world_x + WIDTH - 1};
    const double ys[4] = {y + 1, y + 1, y + HEIGHT - 1, y + HEIGHT - 1};
    for (int i = 0; i < 4; ++i)
        if (aabb_contains(x0, y0, x1, y1, xs[i], ys[i])) return true;
    return false;
}

inline double world_x(double x, double x0) { return -x0 + x; }

inline bool is_ending(double distance, int world) {
    const int tiles = world == 2 ? ENDING2 : ENDING1;
    return distance > tiles * WIDTH;
}

inline void apply_rail(double& x, double& x0, bool ending) {
    if (x < XLEFT) {
        x = XLEFT;
        return;
    }
    if (x > XRIGHT && !ending) {
        x0 -= (x - XRIGHT);
        x = XRIGHT;
    }
}

inline int ending_tiles(int world) { return world == 2 ? ENDING2 : ENDING1; }

struct SaveResult {
    int world;
    std::string status;
};

inline SaveResult read_record(const std::string& path) {
    std::ifstream in(path);
    if (!in) return {0, "missing"};
    std::string text;
    in >> text;
    try {
        size_t n = 0;
        int value = std::stoi(text, &n);
        if (n != text.size()) return {0, "garbage"};
        if (value < 1 || value > 3) return {value, "invalid"};
        return {value, "ok"};
    } catch (...) {
        return {0, "garbage"};
    }
}

inline void write_record(const std::string& path, int world) {
    if (world < 1 || world > 3) throw std::invalid_argument("world");
    std::ofstream out(path);
    out << world;
}

// MSVC LCG so world-3 catalog seeds match Python.
struct MsvcRand {
    std::int32_t hold;
    explicit MsvcRand(int seed = 1) : hold(seed) {}
    int rand() {
        std::int64_t next = static_cast<std::int64_t>(hold) * 214013 + 2531011;
        hold = static_cast<std::int32_t>(next);
        return (static_cast<std::uint32_t>(hold) >> 16) & 0x7fff;
    }
    int random_ab(int a, int b) { return rand() % (b - a) + a; }
};

inline int command_from_keys(const std::string& keys) {
    int bits = 0;
    for (char c : keys) {
        switch (c) {
            case 'A': case 'a': bits |= CMD_LEFT; break;
            case 'D': case 'd': bits |= CMD_RIGHT; break;
            case 'W': case 'w':
            case 'K': case 'k': bits |= CMD_UP; break;
            case 'S': case 's': bits |= CMD_DOWN; break;
            case 'J': case 'j': bits |= CMD_SHOOT; break;
            default: break;
        }
    }
    return bits;
}

}  // namespace maoliao
