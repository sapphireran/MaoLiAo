#include "maoliao/aabb.h"
#include "maoliao/constants.h"

#include <iostream>

namespace {

void report(const char* name, bool vertex, bool full) {
    std::cout << name << "\n  vertex-in-AABB (game): " << (vertex ? "hit" : "miss")
              << "\n  full AABB overlap:      " << (full ? "hit" : "miss") << "\n";
}

}  // namespace

int main() {
    using namespace maoliao;

    const SpriteBox hero{64, 64, 32, 32};
    Vec2 corners[4];
    inset_corners(hero, corners);

    const Aabb floor = tile_aabb(2, 3, 4, 1, 1);
    const Aabb pipe = tile_aabb(4, 2, 1, 1, 10);
    const Aabb far{400, 400, 432, 432};

    report("hero vs grass run at tile (2,3) 4x1",
           vertex_in_aabb(corners, floor),
           aabb_overlap(sprite_aabb(hero), floor));

    report("hero vs pipe mouth id 10 at tile (4,2)",
           vertex_in_aabb(corners, pipe),
           aabb_overlap(sprite_aabb(hero), pipe));

    // A box entirely inside the hero but missing the inset corners.
    Aabb interior{hero.x + 8, hero.y + 8, hero.x + 24, hero.y + 24};
    report("32px hero vs 16px box sitting between corners",
           vertex_in_aabb(corners, interior),
           aabb_overlap(sprite_aabb(hero), interior));

    report("hero vs distant tile", vertex_in_aabb(corners, far),
           aabb_overlap(sprite_aabb(hero), far));

    std::cout << "\nPipe mouth pixel size (id 8/10): "
              << (pipe.max_x - pipe.min_x) << " x " << (pipe.max_y - pipe.min_y)
              << " (game uses 64x64 per amount)\n";
    return 0;
}
