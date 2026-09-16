#include "check.hpp"
#include "collision.hpp"
#include "constants.hpp"
#include "map_data.hpp"

#include <cstdio>

// Role::isHit: any of four inset vertices inside the map rectangle.

int main() {
    std::printf("=== 03 collision AABB ===\n");

    const mla::Rect grass = mla::mapBox(0, 9, 1, 15, 1);
    std::printf("world1 grass [0,9] 15x1  box=(%d,%d)-(%d,%d)\n", grass.left,
                grass.top, grass.right, grass.bottom);

    const mla::Rect pipe = mla::mapBox(36, 7, 10, 1, 1);
    std::printf("pipe mouth id10 [36,7]  box=(%d,%d)-(%d,%d)  (2x tiles)\n",
                pipe.left, pipe.top, pipe.right, pipe.bottom);

    mla::Checks c;
    c.expect(grass.left == 0 && grass.top == 9 * 32 && grass.right == 15 * 32 &&
                 grass.bottom == 10 * 32,
             "grass AABB is 480x32 at y=288");
    c.expect(pipe.right - pipe.left == 64 && pipe.bottom - pipe.top == 64,
             "id 10 uses 64x64");

    // A sprite whose feet sit exactly on the tile top (y = 256, bottom = 288)
    // does NOT hit: vertices are inset by 1 px, so y=287 is still above 288.
    // Role therefore probes hitMap(x, y+1) to detect "standing on".
    const mla::Point flushFeet{64, 8 * 32};
    c.expect(!mla::vertexHitsRect(flushFeet, grass),
             "flush feet on tile top miss (1px inset)");
    const mla::Point probeDown{64, 8 * 32 + 1};
    c.expect(mla::vertexHitsRect(probeDown, grass),
             "Role ground probe uses y+1 and hits");

    const mla::Point above{64, 7 * 32};
    c.expect(!mla::vertexHitsRect(above, grass),
             "one full tile above: no hit");

    // Inset of 1 px: a sprite whose right edge is exactly on the left wall
    // does not count as a hit (vertex x = left-1 is outside).
    const mla::Point justLeft{-32, 9 * 32};
    c.expect(!mla::vertexHitsRect(justLeft, grass),
             "flush left of the box does not overlap (inset)");

    const mla::Point overlap{-16, 9 * 32};
    c.expect(mla::vertexHitsRect(overlap, grass),
             "16 px overlap: right vertices inside");

    // Coin cell (10,5) is 32x32 at (320, 160).
    const mla::Rect coin{10 * 32, 5 * 32, 11 * 32, 6 * 32};
    const mla::Point grab{10 * 32 + 4, 5 * 32 + 4};
    c.expect(mla::vertexHitsRect(grab, coin), "coin pickup AABB");

    std::size_t n = 0;
    const mla::MapRec* w1 = mla::world1Map(n);
    int solids = 0;
    int deco = 0;
    for (std::size_t i = 0; i < n; ++i) {
        if (w1[i].id >= 1 && w1[i].id <= 10) {
            ++solids;
        } else {
            ++deco;
        }
    }
    std::printf("world1 authored tiles: %zu  solids=%d  deco=%d  loaded=%zu\n",
                n, solids, deco, mla::loadedCount(n));
    c.expect(n == 32, "world1 authored 32 Map records");
    c.expect(mla::loadedCount(n) == 30, "MAP_NUMBER drops the last 2 waters");
    return c.finish("03_collision_aabb");
}
