#pragma once

#include <cstddef>

namespace maoliao {

struct Point {
    int x;
    int y;
};

struct Rect {
    int left;
    int top;
    int right;
    int bottom;
};

// Same test as Role::isHit: any of four hero vertices inside the axis-aligned box
// defined by p2[0] (top-left) and p2[1] (bottom-right), inclusive.
inline bool isHit(const Point p1[4], const Point p2[2]) {
    for (int i = 0; i < 4; ++i) {
        if (p1[i].x >= p2[0].x && p1[i].y >= p2[0].y && p1[i].x <= p2[1].x && p1[i].y <= p2[1].y) {
            return true;
        }
    }
    return false;
}

// Hero collision inset used in hitMap / hitCoins / hitEnemy: shrink 1 px on each side.
inline void heroVertices(int screenX, int screenY, int x0, int width, int height, Point out[4]) {
    const int xmap = x0;
    out[0] = {-xmap + screenX + 1, screenY + 1};
    out[1] = {-xmap + screenX + width - 1, screenY + 1};
    out[2] = {-xmap + screenX + 1, screenY + height - 1};
    out[3] = {-xmap + screenX + width - 1, screenY + height - 1};
}

inline Rect tileRect(int tx, int ty, int xAmount, int yAmount, int id, int tileW, int tileH) {
    Rect r{};
    r.left = tx * tileW;
    r.top = ty * tileH;
    const int scale = (id == 8 || id == 10) ? 2 : 1;
    r.right = r.left + xAmount * scale * tileW;
    r.bottom = r.top + yAmount * scale * tileH;
    return r;
}

inline bool rectHit(const Point verts[4], const Rect& r) {
    const Point box[2] = {{r.left, r.top}, {r.right, r.bottom}};
    return isHit(verts, box);
}

// World 3: any colliding tile other than id 2 is lethal until the weapon is held.
inline bool world3Lethal(int tileId, bool hasWeapon) {
    return tileId != 2 && !hasWeapon;
}

}  // namespace maoliao
