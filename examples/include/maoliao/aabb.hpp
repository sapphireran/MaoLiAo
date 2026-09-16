#pragma once

#include "maoliao/constants.hpp"

namespace maoliao {

struct Rect {
    int x0 = 0;
    int y0 = 0;
    int x1 = 0;
    int y1 = 0;
};

struct Vertex4 {
    int x[4]{};
    int y[4]{};
};

// Role::isHit — any of four vertices inside [m0, m1] inclusive.
inline bool is_hit(const Vertex4& r, const Rect& m) {
    for (int i = 0; i < 4; ++i) {
        if (r.x[i] >= m.x0 && r.y[i] >= m.y0 && r.x[i] <= m.x1 && r.y[i] <= m.y1) {
            return true;
        }
    }
    return false;
}

// Inset-by-1 hero corners in world pixels (Role::hitMap).
inline Vertex4 hero_corners(int screen_x, int screen_y, int xmap) {
    Vertex4 r;
    r.x[0] = -xmap + screen_x + 1;
    r.y[0] = screen_y + 1;
    r.x[1] = -xmap + screen_x + kWidth - 1;
    r.y[1] = screen_y + 1;
    r.x[2] = -xmap + screen_x + 1;
    r.y[2] = screen_y + kHeight - 1;
    r.x[3] = -xmap + screen_x + kWidth - 1;
    r.y[3] = screen_y + kHeight - 1;
    return r;
}

struct Tile {
    int x = 0;
    int y = 0;
    int id = 0;
    int x_amount = 0;
    int y_amount = 0;
    double u = 0;
};

inline Rect tile_box(const Tile& t) {
    Rect m;
    m.x0 = t.x * kWidth;
    m.y0 = t.y * kHeight;
    const int wide = (t.id == 8 || t.id == 10) ? 2 : 1;
    m.x1 = m.x0 + t.x_amount * wide * kWidth;
    m.y1 = m.y0 + t.y_amount * wide * kHeight;
    return m;
}

inline bool hero_hits_tile(int screen_x, int screen_y, int xmap, const Tile& t) {
    if (t.id < 1 || t.id > 10) {
        return false;  // scenery 11–14 does not collide
    }
    return is_hit(hero_corners(screen_x, screen_y, xmap), tile_box(t));
}

// World-3 terrain death: any colliding tile except id 2, unless armed.
inline bool world3_instadeath(const Tile& t, bool is_shoot) {
    return t.id != 2 && !is_shoot;
}

inline Rect coin_box(int tile_x, int tile_y) {
    Rect m;
    m.x0 = tile_x * kWidth;
    m.y0 = tile_y * kHeight;
    m.x1 = m.x0 + kWidth;
    m.y1 = m.y0 + kHeight;
    return m;
}

inline Rect food_box(int px, int py) {
    Rect m;
    m.x0 = px;
    m.y0 = py;
    m.x1 = px + 3 * kWidth / 2 + 4;  // 52
    m.y1 = py + 4 * kHeight / 5;     // 25
    return m;
}

inline Rect enemy_box(int world_x, int world_y) {
    return Rect{world_x, world_y, world_x + kWidth, world_y + kHeight};
}

}  // namespace maoliao
