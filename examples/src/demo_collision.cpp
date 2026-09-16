#include "maoliao.hpp"

#include <iostream>

int main() {
    using namespace maoliao;

    const Tile floor = make_tile(0, 9, 1, 15, 1, 1);
    const Aabb box = tile_aabb(floor);
    std::cout << "Opening grass AABB: [" << box.left << "," << box.top << "]-["
              << box.right << "," << box.bottom << "]\n";

    const auto standing = inset_vertices(64, 256, 0);
    std::cout << "Hero at (64,256) inset vertices:\n";
    for (const auto& p : standing) {
        std::cout << "  (" << p.x << "," << p.y << ")\n";
    }

    const bool on_floor = overlaps_box(64, 256, 0, box);          // y+1 would be 257
    const bool feet_on = overlaps_box(64, 256 + 1, 0, box);
    const bool in_air = overlaps_box(64, 200, 0, box);
    std::cout << "body on floor: " << on_floor << "  probe y+1: " << feet_on
              << "  in air: " << in_air << '\n';

    const Tile mouth = make_tile(10, 4, 10, 1, 1, 3);
    const Aabb mouth_box = tile_aabb(mouth);
    std::cout << "Pipe mouth id 10 cell: " << mouth_box.width() << "x"
              << mouth_box.height() << " (expected 64x64)\n";

    const Aabb coin = coin_aabb(10, 5);
    std::cout << "Coin (10,5) overlaps hero (320,160): "
              << overlaps_box(320, 160, 0, coin) << '\n';

    const Tile cloud = make_tile(0, 9, 2, 15, 1, 3);
    const Tile world3[] = {cloud, mouth};
    bool died_cloud = false;
    const auto hit_cloud = hit_map(64, 257, 0, world3, 2, 3, &died_cloud);
    std::cout << "World 3 probe onto cloud: hit=" << hit_cloud.has_value()
              << " died=" << died_cloud << '\n';

    bool died_grass = false;
    const Tile grass_only[] = {floor};
    const auto hit_grass = hit_map(64, 257, 0, grass_only, 1, 3, &died_grass);
    std::cout << "World 3 probe onto grass: hit=" << hit_grass.has_value()
              << " died=" << died_grass << '\n';

    bool died_pipe = false;
    const auto hit_pipe = hit_map(320, 128, 0, world3, 2, 3, &died_pipe);
    std::cout << "World 3 probe onto pipe mouth: hit=" << hit_pipe.has_value()
              << " died=" << died_pipe << '\n';
    return 0;
}
