#include "maoliao_kit.hpp"

#include <iostream>

int main() {
    using namespace maoliao;
    Tile grass{0, 9, 1, 15, 1};
    double x0, y0, x1, y1;
    tile_aabb(grass, x0, y0, x1, y1);
    // Hero standing on the grass: world x=64, y=256. Foot sensor y+1.
    const bool stand = is_hit(64, 257, x0, y0, x1, y1);
    const bool miss = is_hit(64, 200, x0, y0, x1, y1);
    Tile pipe{36, 7, 10, 1, 1};
    tile_aabb(pipe, x0, y0, x1, y1);
    const bool wide = (x1 - x0 == 64) && (y1 - y0 == 64);
    std::cout << "stand=" << stand << " miss=" << miss << " pipe2x2=" << wide << "\n";
    return (stand && !miss && wide) ? 0 : 1;
}
