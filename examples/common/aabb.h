#pragma once

namespace maoliao {

struct Point {
    int x;
    int y;
};

// Role::isHit: any of the four hero corners inside [box[0], box[1]] inclusive.
bool isHit(const Point heroCorners[4], const Point box[2]);

// Role::hitMap corner inset: +1 / -1 so the box is 30x30 inside a 32x32 sprite.
void heroCorners(int screenX, int screenY, int x0, Point out[4]);

// Inclusive AABB from a Map record. Pipe mouths 8 and 10 use 2x tile size.
void tileBox(int tileX, int tileY, int id, int xAmount, int yAmount, Point out[2]);

// World 3: overlap with a solid that is not id 2 kills unless the hero has the weapon.
bool world3Lethal(int tileId, bool hasWeapon);

}  // namespace maoliao
