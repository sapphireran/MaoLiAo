#include "collision_portable.h"

#include "define_portable.h"

namespace maoliao {

std::array<Point, 4> spriteCorners(int worldX, int y) {
    return {{
        {worldX + 1, y + 1},
        {worldX + kWidth - 1, y + 1},
        {worldX + 1, y + kHeight - 1},
        {worldX + kWidth - 1, y + kHeight - 1},
    }};
}

bool pointInRect(const Point& p, const Rect& r) {
    return p.x >= r.x0 && p.y >= r.y0 && p.x <= r.x1 && p.y <= r.y1;
}

bool cornersHit(const std::array<Point, 4>& corners, const Rect& r) {
    for (const auto& c : corners) {
        if (pointInRect(c, r)) {
            return true;
        }
    }
    return false;
}

Rect tileWorldRect(const MapCell& cell) {
    Rect r;
    r.x0 = cell.x * kWidth;
    r.y0 = cell.y * kHeight;
    const bool pipeMouth = (cell.id == 8 || cell.id == 10);
    const int cellW = pipeMouth ? 2 * kWidth : kWidth;
    const int cellH = pipeMouth ? 2 * kHeight : kHeight;
    r.x1 = r.x0 + cell.xAmount * cellW;
    r.y1 = r.y0 + cell.yAmount * cellH;
    return r;
}

const MapCell* hitMap(int worldX,
                      int y,
                      const MapCell* cells,
                      std::size_t count,
                      int world,
                      bool hasStar,
                      bool* lethal) {
    if (lethal) {
        *lethal = false;
    }
    const auto corners = spriteCorners(worldX, y);
    for (std::size_t i = 0; i < count; ++i) {
        const MapCell& cell = cells[i];
        if (cell.id <= 0 || cell.id >= 11) {
            continue;
        }
        if (!cornersHit(corners, tileWorldRect(cell))) {
            continue;
        }
        if (world == 3 && cell.id != 2 && !hasStar) {
            if (lethal) {
                *lethal = true;
            }
        }
        return &cell;
    }
    return nullptr;
}

const Enemy* hitEnemy(int worldX, int y, const Enemy* enemies, std::size_t count) {
    const auto corners = spriteCorners(worldX, y);
    for (std::size_t i = 0; i < count; ++i) {
        if (enemies[i].turn == 0) {
            continue;
        }
        const Rect box{enemies[i].x, enemies[i].y, enemies[i].x + kWidth,
                       enemies[i].y + kHeight};
        if (cornersHit(corners, box)) {
            return &enemies[i];
        }
    }
    return nullptr;
}

EnemyContact classifyEnemyContact(int worldX, int y, double vY,
                                  const Enemy* enemies, std::size_t count) {
    if (hitEnemy(worldX, y, enemies, count) == nullptr) {
        return EnemyContact::None;
    }
    return (vY > 0.0) ? EnemyContact::Stomp : EnemyContact::Lethal;
}

}  // namespace maoliao
