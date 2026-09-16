#pragma once

namespace mla {

struct Point {
    int x = 0;
    int y = 0;
};

struct Rect {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
};

// Role::isHit: any of the sprite's four vertices inside [p2[0], p2[1]].
// Vertices are inset by 1 px, matching hitMap / hitCoins / hitEnemy.
bool vertexHitsRect(const Point spriteTopLeft, const Rect box, int spriteW = 32,
                    int spriteH = 32);

// Build the world-space AABB for a Map record.
// id 8 and 10 use 2x tile size per cell (pipe mouths).
Rect mapBox(int tileX, int tileY, int id, int xAmount, int yAmount,
            int tileW = 32, int tileH = 32);

}  // namespace mla
