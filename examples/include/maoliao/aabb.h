#pragma once

namespace maoliao {

struct Vec2 {
    double x = 0;
    double y = 0;
};

struct Aabb {
    double min_x = 0;
    double min_y = 0;
    double max_x = 0;
    double max_y = 0;
};

struct SpriteBox {
    double x = 0;
    double y = 0;
    double w = 32;
    double h = 32;
};

// Four inset corners, matching Role::hitMap's r[4] (inset 1 px).
void inset_corners(const SpriteBox& s, Vec2 out[4]);

// Vertex-in-AABB, matching Role::isHit.
bool vertex_in_aabb(const Vec2 corners[4], const Aabb& box);

// True AABB overlap (not what the game uses; provided for comparison).
bool aabb_overlap(const Aabb& a, const Aabb& b);

Aabb sprite_aabb(const SpriteBox& s);
Aabb tile_aabb(int tile_x, int tile_y, int x_amount, int y_amount, int id);

}  // namespace maoliao
