#include "maoliao/levels.h"

#include "maoliao/constants.h"

#include <algorithm>
#include <random>

namespace maoliao {
namespace {

double u_for(int world, int id) {
    if (world == 3) {
        if (id == 2) {
            return friction_u(kT2);
        }
        if (id >= 1 && id <= 6) {
            return friction_u(kT1);
        }
        return friction_u(kT3);
    }
    switch (id) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            return friction_u(kT2);
        case 6:
            return friction_u(kT1);
        default:
            return friction_u(kT3);
    }
}

void apply_friction(std::vector<MapTile>& tiles, int world) {
    for (MapTile& t : tiles) {
        t.u = u_for(world, t.id);
    }
}

Level world1() {
    Level L;
    L.world = 1;
    L.ending_tiles = 94;
    L.tiles = {
        {0, 9, 1, 15, 1},  {18, 8, 1, 9, 1}, {27, 9, 5, 37, 1}, {67, 9, 6, 4, 1},
        {74, 9, 5, 1, 1},  {78, 9, 1, 28, 1},
        {0, 10, 3, 15, 2}, {18, 9, 3, 9, 3}, {27, 10, 4, 37, 2}, {67, 10, 3, 4, 2},
        {74, 10, 4, 1, 2}, {78, 10, 3, 28, 2},
        {10, 6, 2, 4, 1},  {40, 4, 2, 3, 1}, {62, 6, 2, 1, 1}, {65, 4, 2, 5, 1},
        {36, 7, 10, 1, 1}, {45, 7, 10, 1, 1}, {80, 7, 10, 1, 1}, {90, 7, 10, 1, 1},
        {19, 6, 11, 1, 1}, {23, 6, 11, 1, 1}, {32, 7, 11, 1, 1}, {49, 7, 11, 1, 1},
        {53, 7, 11, 1, 1}, {57, 7, 11, 1, 1}, {93, 7, 11, 1, 1},
        {101, 7, 12, 1, 1},
        {15, 10, 13, 1, 1}, {64, 10, 13, 1, 1}, {71, 10, 13, 1, 1}, {75, 10, 13, 1, 1},
    };
    L.coins = {
        {10, 5}, {11, 5}, {12, 5}, {13, 5}, {66, 3}, {67, 3}, {68, 3}, {69, 3},
        {74, 5}, {74, 6}, {74, 7}, {74, 8}, {82, 7}, {83, 8}, {84, 7}, {85, 8},
        {86, 7}, {87, 8}, {88, 7}, {89, 8},
    };
    L.food = {{14 * kWidth, 5 * kHeight}};
    L.enemies = {
        {3, 8, 1},  {18, 7, -1}, {25, 7, 1}, {28, 8, -1}, {33, 8, -1},
        {39, 8, 1}, {68, 3, 1},  {66, 8, -1}, {81, 6, 1}, {92, 6, 1},
    };
    apply_friction(L.tiles, 1);
    return L;
}

Level world2() {
    Level L;
    L.world = 2;
    L.ending_tiles = 104;
    L.tiles = {
        {18, 9, 2, 4, 1},  {23, 6, 2, 6, 1},  {25, 3, 2, 3, 1},  {31, 10, 2, 2, 1},
        {39, 7, 2, 1, 1},  {39, 4, 2, 1, 1},  {96, 7, 2, 2, 1},  {99, 5, 2, 2, 1},
        {102, 3, 2, 12, 1},
        {0, 10, 5, 15, 1}, {38, 10, 5, 10, 1}, {50, 10, 5, 1, 1}, {53, 10, 5, 1, 1},
        {56, 10, 5, 1, 1}, {61, 10, 5, 10, 1}, {75, 10, 5, 1, 1}, {82, 10, 5, 11, 1},
        {0, 11, 4, 15, 2}, {38, 11, 4, 10, 2}, {50, 11, 4, 1, 2}, {53, 11, 4, 1, 2},
        {56, 11, 4, 1, 2}, {61, 11, 4, 10, 2}, {75, 11, 4, 1, 2}, {82, 11, 4, 11, 2},
        {67, 9, 2, 1, 1},  {68, 8, 2, 1, 2},  {69, 7, 2, 1, 3},  {70, 6, 2, 1, 4},
        {75, 6, 2, 1, 4},
        {42, 8, 11, 1, 1},
        {111, 1, 12, 1, 1},
        {4, 8, 14, 1, 1},  {9, 8, 14, 1, 1},  {83, 8, 14, 1, 1}, {89, 8, 14, 1, 1},
    };
    L.coins = {
        {25, 2}, {26, 2}, {27, 2}, {32, 4}, {32, 5}, {32, 6},
        {32, 7}, {32, 8}, {50, 6}, {53, 6}, {56, 6},
    };
    L.food = {{39 * kWidth - kWidth / 3, 3 * kHeight + kHeight / 5}};
    L.enemies = {
        {18, 8, 1}, {24, 5, 1}, {28, 5, -1}, {44, 9, 1}, {97, 6, 1}, {99, 4, 1},
    };
    apply_friction(L.tiles, 2);
    return L;
}

int game_random(std::mt19937& rng, int a, int b) {
    // Matches `#define random(a,b) (rand()%(b-a)+a)` on [a, b).
    std::uniform_int_distribution<int> dist(0, (b - a) - 1);
    return dist(rng) + a;
}

Level world3(unsigned seed) {
    Level L;
    L.world = 3;
    L.ending_tiles = 94;
    std::mt19937 rng(seed);
    int height[10];
    int xs[10];
    for (int i = 0; i < 10; ++i) {
        height[i] = game_random(rng, 1, 7);
        xs[i] = i * 10 + 10;
    }
    // Only the first seven pairs are compiled into the game.
    for (int i = 0; i < 7; ++i) {
        const int h = height[i];
        const int x = xs[i];
        const int top_h = std::max(0, h - 2);
        const int bot_h = std::max(0, 6 - h);
        L.tiles.push_back({x, h - 2, 8, 1, 1});
        L.tiles.push_back({x, 0, 7, 1, top_h});
        L.tiles.push_back({x, 4 + h, 10, 1, 1});
        L.tiles.push_back({x, h + 6, 7, 1, bot_h});
    }
    L.tiles.push_back({80, 6, 2, 25, 1});
    L.tiles.push_back({101, 4, 12, 1, 1});

    int coin_h[10];
    for (int i = 0; i < 10; ++i) {
        coin_h[i] = game_random(rng, 3, 7);
    }
    const int coin_x[] = {5, 15, 25, 35, 45, 55, 65};
    for (int i = 0; i < 7; ++i) {
        L.coins.push_back({coin_x[i], coin_h[i]});
    }
    L.food = {{10, 10}};
    L.enemies = {
        {24, 4, -1}, {34, 6, 1}, {37, 3, -1}, {43, 5, -1},
        {63, 7, 1},  {67, 8, -1}, {86, 5, 1},
    };
    apply_friction(L.tiles, 3);
    return L;
}

}  // namespace

Level authored_level(int world) {
    if (world == 2) {
        return world2();
    }
    if (world == 3) {
        return world3(1);
    }
    return world1();
}

Level loaded_level(int world) {
    Level L = authored_level(world);
    if (L.tiles.size() > static_cast<std::size_t>(kMapNumber)) {
        L.tiles.resize(static_cast<std::size_t>(kMapNumber));
    }
    return L;
}

Level authored_world3(unsigned seed) {
    return world3(seed);
}

int ending_distance_px(int world) {
    if (world == 2) {
        return 104 * kWidth;
    }
    return 94 * kWidth;
}

double friction_for_id(int world, int id) {
    return u_for(world, id);
}

}  // namespace maoliao
