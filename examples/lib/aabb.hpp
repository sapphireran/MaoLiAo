#pragma once

#include "constants.hpp"

#include <algorithm>
#include <cstddef>

namespace maoliao {

struct Point {
    int x = 0;
    int y = 0;
};

struct Aabb {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
};

// Role::isHit: any of the four actor corners inside the box [p2[0], p2[1]].
inline bool cornersHitBox(const Point actor[4], Point boxMin, Point boxMax) {
    for (int i = 0; i < 4; ++i) {
        if (actor[i].x >= boxMin.x && actor[i].y >= boxMin.y && actor[i].x <= boxMax.x &&
            actor[i].y <= boxMax.y) {
            return true;
        }
    }
    return false;
}

// Inset 32×32 sprite corners used by hitMap / hitCoins / hitEnemy.
inline void heroCorners(int screenX, int screenY, int x0, Point out[4]) {
    out[0].x = -x0 + screenX + 1;
    out[0].y = screenY + 1;
    out[1].x = -x0 + screenX + kWidth - 1;
    out[1].y = screenY + 1;
    out[2].x = -x0 + screenX + 1;
    out[2].y = screenY + kHeight - 1;
    out[3].x = -x0 + screenX + kWidth - 1;
    out[3].y = screenY + kHeight - 1;
}

struct MapStamp {
    int x = 0;
    int y = 0;
    int id = 0;
    int xAmount = 0;
    int yAmount = 0;
};

inline Aabb stampBox(const MapStamp& m) {
    Aabb b;
    b.left = m.x * kWidth;
    b.top = m.y * kHeight;
    if (m.id == 10 || m.id == 8) {
        b.right = m.x * kWidth + m.xAmount * 2 * kWidth;
        b.bottom = m.y * kHeight + m.yAmount * 2 * kHeight;
    } else {
        b.right = m.x * kWidth + m.xAmount * kWidth;
        b.bottom = m.y * kHeight + m.yAmount * kHeight;
    }
    return b;
}

inline bool stampCollides(const MapStamp& m) { return m.id > 0 && m.id < 11; }

// First colliding stamp, or nullptr. world==3 death is returned via diedOut.
inline const MapStamp* hitMap(int screenX, int screenY, int x0, const MapStamp* stamps,
                              std::size_t count, int world, bool isShoot, bool* diedOut) {
    if (diedOut) {
        *diedOut = false;
    }
    Point r[4];
    heroCorners(screenX, screenY, x0, r);
    for (std::size_t i = 0; i < count; ++i) {
        if (!stampCollides(stamps[i])) {
            continue;
        }
        const Aabb b = stampBox(stamps[i]);
        Point mn{b.left, b.top};
        Point mx{b.right, b.bottom};
        if (cornersHitBox(r, mn, mx)) {
            if (world == 3 && stamps[i].id != 2 && !isShoot) {
                if (diedOut) {
                    *diedOut = true;
                }
            }
            return &stamps[i];
        }
    }
    return nullptr;
}

inline bool endingDistance(int world, int distancePx) {
    switch (world) {
        case 1:
            return distancePx > 94 * kWidth;
        case 2:
            return distancePx > 104 * kWidth;
        case 3:
            return distancePx > 94 * kWidth;
        default:
            return false;
    }
}

inline int clampCameraX(int x, int& x0, bool ending) {
    if (x < kXLeft) {
        return kXLeft;
    }
    if (x > kXRight && !ending) {
        x0 -= (x - kXRight);
        return kXRight;
    }
    return x;
}

}  // namespace maoliao
