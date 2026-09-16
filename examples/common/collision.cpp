#include "collision.hpp"

namespace mla {

bool vertexHitsRect(const Point spriteTopLeft, const Rect box, int spriteW,
                    int spriteH) {
    const Point verts[4] = {
        {spriteTopLeft.x + 1, spriteTopLeft.y + 1},
        {spriteTopLeft.x + spriteW - 1, spriteTopLeft.y + 1},
        {spriteTopLeft.x + 1, spriteTopLeft.y + spriteH - 1},
        {spriteTopLeft.x + spriteW - 1, spriteTopLeft.y + spriteH - 1},
    };
    for (const Point& p : verts) {
        if (p.x >= box.left && p.y >= box.top && p.x <= box.right &&
            p.y <= box.bottom) {
            return true;
        }
    }
    return false;
}

Rect mapBox(int tileX, int tileY, int id, int xAmount, int yAmount, int tileW,
            int tileH) {
    Rect r;
    r.left = tileX * tileW;
    r.top = tileY * tileH;
    if (id == 10 || id == 8) {
        r.right = r.left + xAmount * 2 * tileW;
        r.bottom = r.top + yAmount * 2 * tileH;
    } else {
        r.right = r.left + xAmount * tileW;
        r.bottom = r.top + yAmount * tileH;
    }
    return r;
}

}  // namespace mla
