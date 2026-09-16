#pragma once

#include "constants.hpp"
#include "tiles.hpp"

#include <array>
#include <cstddef>
#include <optional>

namespace maoliao {

struct Vec2i {
    int x = 0;
    int y = 0;
};

struct Aabb {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;

    int width() const { return right - left; }
    int height() const { return bottom - top; }
};

inline bool point_in_aabb(Vec2i p, Aabb box) {
    return p.x >= box.left && p.x <= box.right && p.y >= box.top &&
           p.y <= box.bottom;
}

// Role::isHit: any of four hero vertices inside [p2[0], p2[1]].
inline bool vertices_hit_box(const std::array<Vec2i, 4>& vertices, Aabb box) {
    for (const auto& p : vertices) {
        if (point_in_aabb(p, box)) {
            return true;
        }
    }
    return false;
}

// 1 px inset, converted into world space with camera origin x0.
inline std::array<Vec2i, 4> inset_vertices(int screen_x, int screen_y, int x0) {
    std::array<Vec2i, 4> r{};
    r[0] = {-x0 + screen_x + 1, screen_y + 1};
    r[1] = {-x0 + screen_x + kTileWidth - 1, screen_y + 1};
    r[2] = {-x0 + screen_x + 1, screen_y + kTileHeight - 1};
    r[3] = {-x0 + screen_x + kTileWidth - 1, screen_y + kTileHeight - 1};
    return r;
}

inline Aabb tile_aabb(const Tile& tile) {
    Aabb box;
    box.left = tile.x * kTileWidth;
    box.top = tile.y * kTileHeight;
    box.right = box.left + tile.x_amount * cell_width_px(tile.id);
    box.bottom = box.top + tile.y_amount * cell_height_px(tile.id);
    return box;
}

inline Aabb coin_aabb(int tile_x, int tile_y) {
    return Aabb{tile_x * kTileWidth, tile_y * kTileHeight,
                tile_x * kTileWidth + kTileWidth,
                tile_y * kTileHeight + kTileHeight};
}

inline Aabb food_aabb(int pixel_x, int pixel_y) {
    return Aabb{pixel_x, pixel_y, pixel_x + 3 * kTileWidth / 2 + 4,
                pixel_y + 4 * kTileHeight / 5};
}

inline Aabb enemy_aabb(int pixel_x, int pixel_y) {
    return Aabb{pixel_x, pixel_y, pixel_x + kTileWidth, pixel_y + kTileHeight};
}

struct HitResult {
    std::size_t index = 0;
    Tile tile{};
};

inline std::optional<HitResult> hit_map(int screen_x, int screen_y, int x0,
                                        const Tile* tiles, std::size_t count,
                                        int world, bool* died = nullptr) {
    const auto vertices = inset_vertices(screen_x, screen_y, x0);
    for (std::size_t i = 0; i < count; ++i) {
        if (!is_solid(tiles[i].id)) {
            continue;
        }
        if (vertices_hit_box(vertices, tile_aabb(tiles[i]))) {
            if (died != nullptr && world == 3 && tiles[i].id != 2) {
                *died = true;
            }
            return HitResult{i, tiles[i]};
        }
    }
    return std::nullopt;
}

inline bool overlaps_box(int screen_x, int screen_y, int x0, Aabb box) {
    return vertices_hit_box(inset_vertices(screen_x, screen_y, x0), box);
}

} // namespace maoliao
