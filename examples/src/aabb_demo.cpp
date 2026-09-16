// Four-corner AABB, same shrink-by-1 and pipe double-size as Role::hitMap.

#include "maoliao_model.h"

#include <cstdio>

using namespace maoliao;

int main() {
    Point body[4];

    // Hero at screen (64, 256), camera x0 = 0, standing on a 15-wide grass
    // strip at tile (0, 9) → pixel y = 288. Feet at y+31 = 287, so y=256
    // overlaps the row y=256..287? Grass top is 288. One pixel above:
    heroCorners(64, 256, 0, body);
    MLA_CHECK(body[0].x == 65 && body[0].y == 257);
    MLA_CHECK(body[3].x == 64 + kWidth - 1 && body[3].y == 256 + kHeight - 1);

    Point grass0{0, 9 * kHeight};
    Point grass1{15 * kWidth, 9 * kHeight + kHeight};
    heroCorners(64, 9 * kHeight - kHeight, 0, body);  // standing on top
    // Bottom vertices y = 288 - 32 + 31 = 287, grass top is 288 → miss.
    MLA_CHECK(!isHit(body, grass0, grass1));
    // The game probes y+1 when asking "am I on the floor?"
    heroCorners(64, 9 * kHeight - kHeight + 1, 0, body);
    MLA_CHECK(isHit(body, grass0, grass1));

    // Completely inside a 32x32 coin at (10, 5).
    heroCorners(10 * kWidth, 5 * kHeight, 0, body);
    Point c0{10 * kWidth, 5 * kHeight};
    Point c1{11 * kWidth, 6 * kHeight};
    MLA_CHECK(isHit(body, c0, c1));

    // Far to the right, camera scrolled: x0 = -200, screen x = 192.
    // World x of left corner = -(-200) + 192 + 1 = 393.
    heroCorners(kXRight, 100, -200, body);
    MLA_CHECK(body[0].x == 200 + kXRight + 1);

    // Pipe mouth id 8/10 uses 2x tile size.
    Point pipe0{36 * kWidth, 7 * kHeight};
    Point pipe1{36 * kWidth + 2 * kWidth, 7 * kHeight + 2 * kHeight};
    heroCorners(36 * kWidth + 10, 7 * kHeight + 10, 0, body);
    MLA_CHECK(isHit(body, pipe0, pipe1));
    // Same box as a 1x1 tile would miss a point 40px to the right.
    Point small1{36 * kWidth + kWidth, 7 * kHeight + kHeight};
    heroCorners(36 * kWidth + 40, 7 * kHeight + 10, 0, body);
    MLA_CHECK(!isHit(body, pipe0, small1));
    MLA_CHECK(isHit(body, pipe0, pipe1));

    // Thin bar through the torso: four corners miss (known limitation).
    heroCorners(0, 0, 0, body);
    Point bar0{10, 20};
    Point bar1{20, 22};
    MLA_CHECK(!isHit(body, bar0, bar1));

    std::printf("aabb_demo ok  corners=(%d,%d)..(%d,%d)\n",
                body[0].x, body[0].y, body[3].x, body[3].y);
    return 0;
}
