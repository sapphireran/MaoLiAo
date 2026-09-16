#include "aabb.hpp"
#include "worlds.hpp"

#include <cstdlib>
#include <iostream>

namespace {

int gFails = 0;

void expect(bool cond, const char* msg) {
    if (!cond) {
        std::cerr << "FAIL: " << msg << "\n";
        ++gFails;
    }
}

}  // namespace

int main() {
    using namespace maoliao;

    std::cout << "MaoLiAo collision_probe\n";

    // A 32×32 hero at (64, 288) with x0=0 sits on grass row y=9 (288).
    Point r[4];
    heroCorners(64, 288, 0, r);
    expect(r[0].x == 65 && r[0].y == 289, "inset top-left corner");
    expect(r[3].x == 64 + 31 && r[3].y == 288 + 31, "inset bottom-right corner");

    MapStamp grass{0, 9, 1, 15, 1};
    const Aabb box = stampBox(grass);
    expect(box.left == 0 && box.top == 288 && box.right == 15 * 32 && box.bottom == 320,
           "grass slab AABB");

    Point mn{box.left, box.top};
    Point mx{box.right, box.bottom};
    expect(cornersHitBox(r, mn, mx), "standing on grass should hit");

    Point high[4];
    heroCorners(64, 64, 0, high);
    expect(!cornersHitBox(high, mn, mx), "high above grass should miss");

    MapStamp pipeMouth{36, 7, 10, 1, 1};
    const Aabb pipe = stampBox(pipeMouth);
    expect(pipe.right - pipe.left == 64 && pipe.bottom - pipe.top == 64,
           "id 10 collision is 2×2 tiles");

    MapStamp tree{4, 8, 14, 1, 1};
    expect(!stampCollides(tree), "id 14 is scenery");

    bool died = false;
    const MapStamp* hit =
        hitMap(64, 288, 0, kWorld1Maps, storedMapCount(kWorld1), 1, false, &died);
    expect(hit != nullptr && hit->id == 1, "world 1 spawn overlaps grass");
    expect(!died, "world 1 must not use touch-death");

    // World 3: a pipe mouth kills unless isShoot.
    MapStamp w3[] = {{10, 2, 8, 1, 1}, {80, 6, 2, 25, 1}};
    died = false;
    const MapStamp* pipeHit = hitMap(10 * 32, 2 * 32, 0, w3, 2, 3, false, &died);
    expect(pipeHit != nullptr && died, "world 3 pipe kills without star");

    died = false;
    const MapStamp* pipeSafe = hitMap(10 * 32, 2 * 32, 0, w3, 2, 3, true, &died);
    expect(pipeSafe != nullptr && !died, "world 3 pipe is survivable with isShoot");

    died = false;
    const MapStamp* cloud = hitMap(80 * 32, 6 * 32, 0, w3, 2, 3, false, &died);
    expect(cloud != nullptr && cloud->id == 2 && !died, "world 3 clouds are safe");

    expect(endingDistance(1, 94 * kWidth) == false, "ending is strictly greater than 94 tiles");
    expect(endingDistance(1, 94 * kWidth + 1), "one pixel past 94 tiles starts auto-walk");
    expect(endingDistance(2, 104 * kWidth + 1), "world 2 ending");
    expect(!endingDistance(2, 94 * kWidth + 1), "world 2 is longer than world 1");

    int x0 = 0;
    int clamped = clampCameraX(200, x0, false);
    expect(clamped == kXRight, "camera rail at 192");
    expect(x0 == -(200 - kXRight), "overflow dumped into x0");

    x0 = 0;
    clamped = clampCameraX(200, x0, true);
    expect(clamped == 200 && x0 == 0, "ending releases the rail");

    x0 = 0;
    clamped = clampCameraX(-4, x0, false);
    expect(clamped == 0, "left wall");

    if (gFails) {
        std::cerr << gFails << " assertion(s) failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
