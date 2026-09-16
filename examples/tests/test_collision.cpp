#include "check.hpp"
#include "maoliao.hpp"

int main() {
    using namespace maoliao;

    const Tile grass = make_tile(0, 9, 1, 15, 1, 1);
    const Aabb box = tile_aabb(grass);
    CHECK(box.left == 0);
    CHECK(box.top == 288);
    CHECK(box.right == 480);
    CHECK(box.bottom == 320);
    CHECK(is_solid(1));
    CHECK(!is_solid(12));
    CHECK(!is_solid(0));

    const Tile mouth = make_tile(10, 4, 10, 1, 1, 3);
    CHECK(tile_aabb(mouth).width() == 64);
    CHECK(tile_aabb(mouth).height() == 64);
    CHECK(is_pipe_mouth(8));
    CHECK(is_pipe_mouth(10));
    CHECK(!is_pipe_mouth(7));

    CHECK(overlaps_box(64, 257, 0, box));
    CHECK(!overlaps_box(64, 200, 0, box));

    const Aabb coin = coin_aabb(10, 5);
    CHECK(coin.left == 320);
    CHECK(coin.top == 160);
    CHECK(overlaps_box(320, 160, 0, coin));
    CHECK(!overlaps_box(0, 0, 0, coin));

    const Aabb food = food_aabb(14 * 32, 5 * 32);
    CHECK(food.width() == 52);
    CHECK(food.height() == 25);

    const Tile cloud = make_tile(0, 9, 2, 15, 1, 3);
    const Tile world3[] = {cloud, mouth};
    bool died_cloud = false;
    CHECK(hit_map(64, 257, 0, world3, 2, 3, &died_cloud).has_value());
    CHECK(!died_cloud);

    bool died_grass = false;
    const Tile grass_only[] = {grass};
    CHECK(hit_map(64, 257, 0, grass_only, 1, 3, &died_grass).has_value());
    CHECK(died_grass);

    bool died_pipe = false;
    CHECK(hit_map(320, 128, 0, world3, 2, 3, &died_pipe).has_value());
    CHECK(died_pipe);

    const auto verts = inset_vertices(64, 256, 0);
    CHECK(verts[0].x == 65);
    CHECK(verts[0].y == 257);
    CHECK(verts[1].x == 95);
    CHECK(verts[3].y == 287);
    return report("test_collision");
}
