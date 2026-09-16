#include "aabb.h"
#include "maoliao_constants.h"

namespace maoliao {

bool isHit(const Point heroCorners[4], const Point box[2]) {
    for (int i = 0; i < 4; ++i) {
        if (heroCorners[i].x >= box[0].x && heroCorners[i].y >= box[0].y &&
            heroCorners[i].x <= box[1].x && heroCorners[i].y <= box[1].y) {
            return true;
        }
    }
    return false;
}

void heroCorners(int screenX, int screenY, int x0, Point out[4]) {
    const int xmap = x0;
    out[0].x = -xmap + screenX + 1;
    out[0].y = screenY + 1;
    out[1].x = -xmap + screenX + kWidth - 1;
    out[1].y = screenY + 1;
    out[2].x = -xmap + screenX + 1;
    out[2].y = screenY + kHeight - 1;
    out[3].x = -xmap + screenX + kWidth - 1;
    out[3].y = screenY + kHeight - 1;
}

void tileBox(int tileX, int tileY, int id, int xAmount, int yAmount, Point out[2]) {
    out[0].x = tileX * kWidth;
    out[0].y = tileY * kHeight;
    if (id == 10 || id == 8) {
        out[1].x = tileX * kWidth + xAmount * 2 * kWidth;
        out[1].y = tileY * kHeight + yAmount * 2 * kHeight;
    } else {
        out[1].x = tileX * kWidth + xAmount * kWidth;
        out[1].y = tileY * kHeight + yAmount * kHeight;
    }
}

bool world3Lethal(int tileId, bool hasWeapon) {
    if (hasWeapon) {
        return false;
    }
    return tileId != 2;
}

}  // namespace maoliao
