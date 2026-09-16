#include <iostream>

#include "maoliao/aabb.hpp"
#include "maoliao/check.hpp"
#include "maoliao/constants.hpp"

int main() {
    using namespace maoliao;

    // Hero at screen (64, 64), camera 0: world box [65,65]–[95,95].
    const Vertex4 r = hero_corners(kX0, kSpawnY, 0);
    MLA_CHECK(r.x[0] == 65 && r.y[0] == 65);
    MLA_CHECK(r.x[3] == 95 && r.y[3] == 95);

    Tile grass{0, 2, 1, 4, 1, u_t2()};  // world [0,64]–[128,96]
    MLA_CHECK(hero_hits_tile(kX0, kSpawnY, 0, grass));

    Tile far{20, 9, 1, 2, 1, u_t2()};
    MLA_CHECK(!hero_hits_tile(kX0, kSpawnY, 0, far));

    Tile tuft{2, 2, 11, 1, 1, 0};  // scenery: no collide
    MLA_CHECK(!hero_hits_tile(kX0, kSpawnY, 0, tuft));

    // Pipe mouth id 10 is 2×: one tile at (0,0) covers [0,0]–[64,64].
    Tile mouth{0, 0, 10, 1, 1, u_t3()};
    const Rect box = tile_box(mouth);
    MLA_CHECK(box.x1 - box.x0 == 64);
    MLA_CHECK(box.y1 - box.y0 == 64);
    MLA_CHECK(hero_hits_tile(16, 16, 0, mouth));

    // Shaft id 7 stays 32×32.
    Tile shaft{0, 0, 7, 1, 1, u_t3()};
    const Rect s = tile_box(shaft);
    MLA_CHECK(s.x1 - s.x0 == 32);

    MLA_CHECK(world3_instadeath(mouth, false));
    MLA_CHECK(!world3_instadeath(mouth, true));
    Tile cloud{0, 0, 2, 1, 1, u_t2()};
    MLA_CHECK(!world3_instadeath(cloud, false));

    // Coin on (2,2) is [64,64]–[96,96] and overlaps the inset hero.
    MLA_CHECK(is_hit(r, coin_box(2, 2)));
    MLA_CHECK(!is_hit(r, coin_box(10, 5)));

    const Rect food = food_box(14 * kWidth, 5 * kHeight);
    MLA_CHECK(food.x1 - food.x0 == 52);
    MLA_CHECK(food.y1 - food.y0 == 25);

    std::cout << "hero inset     [" << r.x[0] << "," << r.y[0] << "]-["
              << r.x[3] << "," << r.y[3] << "]\n";
    std::cout << "pipe 2x box    " << box.x1 - box.x0 << "x" << box.y1 - box.y0 << "\n";
    std::cout << "food box       " << food.x1 - food.x0 << "x" << food.y1 - food.y0 << "\n";

    return done("aabb_lab");
}
