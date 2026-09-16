#include <iostream>
#include <string>

#include "maoliao/check.hpp"
#include "maoliao/csv.hpp"
#include "maoliao/worlds.hpp"

static bool same_tile(const maoliao::Tile& a, const maoliao::Tile& b) {
    return a.x == b.x && a.y == b.y && a.id == b.id && a.x_amount == b.x_amount &&
           a.y_amount == b.y_amount;
}

static bool same_actor(const maoliao::Actor& a, const maoliao::Actor& b) {
    return a.x == b.x && a.y == b.y && a.turn == b.turn;
}

int main() {
    using namespace maoliao;

    const auto w1 = world1_authored();
    const auto w2 = world2_authored();
    const auto s1 = stored_map(w1);
    const auto s2 = stored_map(w2);

    std::cout << "world1 authored " << w1.size() << "  stored " << s1.size() << "\n";
    std::cout << "world2 authored " << w2.size() << "  stored " << s2.size() << "\n";

    MLA_CHECK(w1.size() == 32);
    MLA_CHECK(s1.size() == 30);
    MLA_CHECK(w2.size() == 36);
    MLA_CHECK(s2.size() == 30);

    // Last stored world-1 tile is water at x=64; 71 and 75 are clipped.
    MLA_CHECK(s1.back().x == 64 && s1.back().id == 13);
    MLA_CHECK(w1[30].x == 71 && w1[31].x == 75);

    // World-2 clipped tail includes the goal sign at (111,1).
    bool flag_stored = false;
    for (const Tile& t : s2) {
        if (t.id == 12) {
            flag_stored = true;
        }
    }
    MLA_CHECK(!flag_stored);
    MLA_CHECK(w2[31].id == 12 && w2[31].x == 111);

    const auto csv1 = tiles_from_csv("testdata/world1_map.csv");
    const auto csv2 = tiles_from_csv("testdata/world2_map.csv");
    MLA_CHECK(csv1.size() == w1.size());
    MLA_CHECK(csv2.size() == w2.size());
    for (std::size_t i = 0; i < w1.size(); ++i) {
        MLA_CHECK(same_tile(csv1[i], w1[i]));
    }
    for (std::size_t i = 0; i < w2.size(); ++i) {
        MLA_CHECK(same_tile(csv2[i], w2[i]));
    }

    const auto c1 = world1_coins();
    const auto e1 = world1_enemies();
    MLA_CHECK(c1.size() == 20);
    MLA_CHECK(e1.size() == 10);
    const auto csv_c1 = actors_from_csv("testdata/world1_coins.csv");
    const auto csv_e1 = actors_from_csv("testdata/world1_enemies.csv");
    MLA_CHECK(csv_c1.size() == c1.size());
    MLA_CHECK(csv_e1.size() == e1.size());
    for (std::size_t i = 0; i < c1.size(); ++i) {
        MLA_CHECK(same_actor(csv_c1[i], c1[i]));
    }
    for (std::size_t i = 0; i < e1.size(); ++i) {
        MLA_CHECK(same_actor(csv_e1[i], e1[i]));
    }

    const auto c2 = world2_coins();
    const auto e2 = world2_enemies();
    MLA_CHECK(c2.size() == 11);
    MLA_CHECK(e2.size() == 6);
    const auto csv_c2 = actors_from_csv("testdata/world2_coins.csv");
    const auto csv_e2 = actors_from_csv("testdata/world2_enemies.csv");
    MLA_CHECK(csv_c2.size() == 11);
    MLA_CHECK(csv_e2.size() == 6);

    MLA_CHECK(world1_food_x() == 448 && world1_food_y() == 160);
    MLA_CHECK(world2_food_x() == 1238 && world2_food_y() == 102);
    MLA_CHECK(ending_tiles(1) == 94 && ending_tiles(2) == 104);

    int solids = 0, scenery = 0;
    for (const Tile& t : s1) {
        if (t.id >= 1 && t.id <= 10) {
            ++solids;
        } else {
            ++scenery;
        }
    }
    std::cout << "world1 stored  solids " << solids << "  scenery " << scenery << "\n";
    MLA_CHECK(solids + scenery == 30);

    return done("world_atlas");
}
