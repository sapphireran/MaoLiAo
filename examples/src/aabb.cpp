#include "maoliao/aabb.h"

#include "maoliao/constants.h"

namespace maoliao {

void inset_corners(const SpriteBox& s, Vec2 out[4]) {
    out[0] = {s.x + 1.0, s.y + 1.0};
    out[1] = {s.x + s.w - 1.0, s.y + 1.0};
    out[2] = {s.x + 1.0, s.y + s.h - 1.0};
    out[3] = {s.x + s.w - 1.0, s.y + s.h - 1.0};
}

bool vertex_in_aabb(const Vec2 corners[4], const Aabb& box) {
    for (int i = 0; i < 4; ++i) {
        if (corners[i].x >= box.min_x && corners[i].y >= box.min_y &&
            corners[i].x <= box.max_x && corners[i].y <= box.max_y) {
            return true;
        }
    }
    return false;
}

bool aabb_overlap(const Aabb& a, const Aabb& b) {
    return a.min_x <= b.max_x && a.max_x >= b.min_x && a.min_y <= b.max_y &&
           a.max_y >= b.min_y;
}

Aabb sprite_aabb(const SpriteBox& s) {
    return {s.x, s.y, s.x + s.w, s.y + s.h};
}

Aabb tile_aabb(int tile_x, int tile_y, int x_amount, int y_amount, int id) {
    const double x0 = static_cast<double>(tile_x * kWidth);
    const double y0 = static_cast<double>(tile_y * kHeight);
    const bool pipe_mouth = (id == 8 || id == 10);
    const double cell_w = pipe_mouth ? 2.0 * kWidth : kWidth;
    const double cell_h = pipe_mouth ? 2.0 * kHeight : kHeight;
    return {x0, y0, x0 + x_amount * cell_w, y0 + y_amount * cell_h};
}

}  // namespace maoliao
