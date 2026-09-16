#pragma once

// Twin of Role::isHit / hitMap / hitEnemy corner tests.

#include "map_portable.h"

#include <array>
#include <cstddef>
#include <optional>

namespace maoliao {

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

struct Enemy {
    int x = 0;
    int y = 0;
    int turn = 0;
};

// Four inset corners in world pixels (Role::hitMap).
std::array<Point, 4> spriteCorners(int worldX, int y);

bool pointInRect(const Point& p, const Rect& r);

// True if any of the four corners sits inside r.
bool cornersHit(const std::array<Point, 4>& corners, const Rect& r);

// Tile AABB, including the 2× size for pipe mouths (id 8 and 10).
Rect tileWorldRect(const MapCell& cell);

// First solid tile (id 1..10) whose AABB contains a sprite corner.
// If world == 3, lethal is set when the tile is not id 2 and !hasStar.
const MapCell* hitMap(int worldX,
                      int y,
                      const MapCell* cells,
                      std::size_t count,
                      int world,
                      bool hasStar,
                      bool* lethal = nullptr);

const Enemy* hitEnemy(int worldX, int y, const Enemy* enemies, std::size_t count);

// Falling onto an enemy (vY > 0) is a stomp; vY <= 0 is death.
enum class EnemyContact { None, Stomp, Lethal };

EnemyContact classifyEnemyContact(int worldX, int y, double vY,
                                  const Enemy* enemies, std::size_t count);

}  // namespace maoliao
