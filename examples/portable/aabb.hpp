#pragma once

// Four-corner inset test copied from Role::isHit / hitMap / hitCoins / hitEnemy.

#include "maoliao_math.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace mla {

struct Point {
    int x = 0;
    int y = 0;
};

struct Rect {
    int x0 = 0;
    int y0 = 0;
    int x1 = 0;
    int y1 = 0;
};

struct Tile {
    int x = 0;
    int y = 0;
    int id = 0;
    int xAmount = 0;
    int yAmount = 0;
};

inline std::array<Point, 4> actorCorners(int screenX, int screenY, int xMap) {
    std::array<Point, 4> r{};
    r[0] = {-xMap + screenX + 1, screenY + 1};
    r[1] = {-xMap + screenX + kWidth - 1, screenY + 1};
    r[2] = {-xMap + screenX + 1, screenY + kHeight - 1};
    r[3] = {-xMap + screenX + kWidth - 1, screenY + kHeight - 1};
    return r;
}

inline bool pointInRect(const Point& p, const Rect& box) {
    return p.x >= box.x0 && p.y >= box.y0 && p.x <= box.x1 && p.y <= box.y1;
}

inline bool isHit(const std::array<Point, 4>& corners, const Rect& box) {
    for (const Point& p : corners) {
        if (pointInRect(p, box)) {
            return true;
        }
    }
    return false;
}

inline Rect tileRect(const Tile& tile) {
    Rect m;
    m.x0 = tile.x * kWidth;
    m.y0 = tile.y * kHeight;
    if (tile.id == 10 || tile.id == 8) {
        m.x1 = tile.x * kWidth + tile.xAmount * 2 * kWidth;
        m.y1 = tile.y * kHeight + tile.yAmount * 2 * kHeight;
    } else {
        m.x1 = tile.x * kWidth + tile.xAmount * kWidth;
        m.y1 = tile.y * kHeight + tile.yAmount * kHeight;
    }
    return m;
}

inline bool tileCollides(int id) {
    return id > 0 && id < 11;
}

// Returns the index of the first colliding tile, or -1.
inline int hitMapIndex(int screenX, int screenY, int xMap, const Tile* tiles,
                       std::size_t count) {
    const auto corners = actorCorners(screenX, screenY, xMap);
    for (std::size_t i = 0; i < count; ++i) {
        if (!tileCollides(tiles[i].id)) {
            continue;
        }
        if (isHit(corners, tileRect(tiles[i]))) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

inline Rect coinRect(int cellX, int cellY) {
    const int x = cellX * kWidth;
    const int y = cellY * kHeight;
    return {x, y, x + kWidth, y + kHeight};
}

inline Rect enemyRect(int worldX, int worldY) {
    return {worldX, worldY, worldX + kWidth, worldY + kHeight};
}

inline Rect foodRect(int pixelX, int pixelY) {
    return {pixelX, pixelY, pixelX + 3 * kWidth / 2 + 4, pixelY + 4 * kHeight / 5};
}

// World-3 lethal tile: any collider that is not a cloud, unless powered.
inline bool world3Lethal(const Tile& tile, bool isShoot) {
    return tileCollides(tile.id) && tile.id != 2 && !isShoot;
}

} // namespace mla
