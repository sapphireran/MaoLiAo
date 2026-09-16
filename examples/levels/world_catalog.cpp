#include <algorithm>
#include <iostream>
#include <vector>

#include "check.hpp"
#include "inertia.hpp"
#include "maoliao_const.hpp"

using namespace maoliao;
using maoliao::test::check;
using maoliao::test::checkNear;

struct Tile {
    int x, y, id, xAmount, yAmount;
};

struct Enemy {
    int x, y, turn;
};

struct Coin {
    int x, y;
};

static double uForWorld12(int id) {
    switch (id) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            return frictionU(kT2);
        case 6:
            return frictionU(kT1);
        default:
            return frictionU(kT3);
    }
}

static double uForWorld3(int id) {
    switch (id) {
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
            return frictionU(kT1);
        case 2:
            return frictionU(kT2);
        default:
            return frictionU(kT3);
    }
}

static const std::vector<Tile> kWorld1 = {
    {0, 9, 1, 15, 1},   {18, 8, 1, 9, 1},  {27, 9, 5, 37, 1}, {67, 9, 6, 4, 1},
    {74, 9, 5, 1, 1},   {78, 9, 1, 28, 1}, {0, 10, 3, 15, 2}, {18, 9, 3, 9, 3},
    {27, 10, 4, 37, 2}, {67, 10, 3, 4, 2}, {74, 10, 4, 1, 2}, {78, 10, 3, 28, 2},
    {10, 6, 2, 4, 1},   {40, 4, 2, 3, 1},  {62, 6, 2, 1, 1},  {65, 4, 2, 5, 1},
    {36, 7, 10, 1, 1},  {45, 7, 10, 1, 1}, {80, 7, 10, 1, 1}, {90, 7, 10, 1, 1},
    {19, 6, 11, 1, 1},  {23, 6, 11, 1, 1}, {32, 7, 11, 1, 1}, {49, 7, 11, 1, 1},
    {53, 7, 11, 1, 1},  {57, 7, 11, 1, 1}, {93, 7, 11, 1, 1}, {101, 7, 12, 1, 1},
    {15, 10, 13, 1, 1}, {64, 10, 13, 1, 1}, {71, 10, 13, 1, 1}, {75, 10, 13, 1, 1},
};

static const std::vector<Tile> kWorld2 = {
    {18, 9, 2, 4, 1},  {23, 6, 2, 6, 1},  {25, 3, 2, 3, 1},  {31, 10, 2, 2, 1},
    {39, 7, 2, 1, 1},  {39, 4, 2, 1, 1},  {96, 7, 2, 2, 1},  {99, 5, 2, 2, 1},
    {102, 3, 2, 12, 1}, {0, 10, 5, 15, 1}, {38, 10, 5, 10, 1}, {50, 10, 5, 1, 1},
    {53, 10, 5, 1, 1}, {56, 10, 5, 1, 1}, {61, 10, 5, 10, 1}, {75, 10, 5, 1, 1},
    {82, 10, 5, 11, 1}, {0, 11, 4, 15, 2}, {38, 11, 4, 10, 2}, {50, 11, 4, 1, 2},
    {53, 11, 4, 1, 2}, {56, 11, 4, 1, 2}, {61, 11, 4, 10, 2}, {75, 11, 4, 1, 2},
    {82, 11, 4, 11, 2}, {67, 9, 2, 1, 1}, {68, 8, 2, 1, 2}, {69, 7, 2, 1, 3},
    {70, 6, 2, 1, 4},  {75, 6, 2, 1, 4},  {42, 8, 11, 1, 1}, {111, 1, 12, 1, 1},
    {4, 8, 14, 1, 1},  {9, 8, 14, 1, 1},  {83, 8, 14, 1, 1}, {89, 8, 14, 1, 1},
};

static const std::vector<Enemy> kEmy1 = {
    {3, 8, 1},  {18, 7, -1}, {25, 7, 1}, {28, 8, -1}, {33, 8, -1},
    {39, 8, 1}, {68, 3, 1},  {66, 8, -1}, {81, 6, 1}, {92, 6, 1},
};

static const std::vector<Enemy> kEmy2 = {{18, 8, 1}, {24, 5, 1}, {28, 5, -1},
                                         {44, 9, 1}, {97, 6, 1}, {99, 4, 1}};

static const std::vector<Enemy> kEmy3 = {{24, 4, -1}, {34, 6, 1}, {37, 3, -1}, {43, 5, -1},
                                         {63, 7, 1},  {67, 8, -1}, {86, 5, 1}};

static const std::vector<Coin> kCoins1 = {
    {10, 5}, {11, 5}, {12, 5}, {13, 5}, {66, 3}, {67, 3}, {68, 3}, {69, 3}, {74, 5}, {74, 6},
    {74, 7}, {74, 8}, {82, 7}, {83, 8}, {84, 7}, {85, 8}, {86, 7}, {87, 8}, {88, 7}, {89, 8},
};

static const std::vector<Coin> kCoins2 = {{25, 2}, {26, 2}, {27, 2}, {32, 4}, {32, 5}, {32, 6},
                                         {32, 7}, {32, 8}, {50, 6}, {53, 6}, {56, 6}};

static void testCaps() {
    check(static_cast<int>(kWorld1.size()) == 32, "world 1 authors 32 tiles");
    check(static_cast<int>(kWorld2.size()) == 36, "world 2 authors 36 tiles");
    check(static_cast<int>(kWorld1.size()) > kMapNumber, "world 1 exceeds MAP_NUMBER");
    check(static_cast<int>(kWorld2.size()) > kMapNumber, "world 2 exceeds MAP_NUMBER");
    const int loaded1 = std::min(static_cast<int>(kWorld1.size()), kMapNumber);
    const int dropped1 = static_cast<int>(kWorld1.size()) - loaded1;
    check(dropped1 == 2, "world 1 drops two water tiles");
    check(kWorld1[30].id == 13 && kWorld1[31].id == 13, "dropped ids are scenery water");
    check(kWorld2.back().id == 14, "world 2 tail is trees");
    check(kWorld2[31].id == 12, "goal sign sits at index 31 and is not loaded");
}

static void testFrictionTables() {
    checkNear(uForWorld12(1), frictionU(kT2), 1e-12, "grass T2");
    checkNear(uForWorld12(6), frictionU(kT1), 1e-12, "pipe shaft T1");
    checkNear(uForWorld12(10), frictionU(kT3), 1e-12, "mouth T3");
    checkNear(uForWorld3(6), frictionU(kT1), 1e-12, "world 3 shaft T1");
    checkNear(uForWorld3(2), frictionU(kT2), 1e-12, "world 3 cloud T2");
    check(uForWorld12(1) == uForWorld12(5), "snow top shares grass µ");
}

static void testCounts() {
    check(static_cast<int>(kEmy1.size()) == 10, "world 1 enemies");
    check(static_cast<int>(kEmy2.size()) == 6, "world 2 enemies");
    check(static_cast<int>(kEmy3.size()) == 7, "world 3 enemies");
    check(static_cast<int>(kCoins1.size()) == 20, "world 1 coins");
    check(static_cast<int>(kCoins2.size()) == 11, "world 2 coins");
    check(kEmy1.size() <= static_cast<std::size_t>(kEnemyTote), "enemy pool");
    check(kCoins1.size() <= static_cast<std::size_t>(kCoinsNumber), "coin pool");
}

static void testEnding() {
    check(kEndTilesWorld1 * kWidth == 3008, "world 1/3 end px");
    check(kEndTilesWorld2 * kWidth == 3328, "world 2 end px");
    check(3008 > 94 * 31, "sanity");
}

static void testSolidVsScenery() {
    int solid = 0;
    int scenery = 0;
    for (int i = 0; i < kMapNumber && i < static_cast<int>(kWorld1.size()); ++i) {
        if (kWorld1[static_cast<std::size_t>(i)].id > 0 &&
            kWorld1[static_cast<std::size_t>(i)].id < 11) {
            ++solid;
        } else {
            ++scenery;
        }
    }
    check(solid > 0 && scenery > 0, "loaded world 1 mix of solid and scenery");
    std::cout << "  world 1 loaded solids=" << solid << " scenery=" << scenery << '\n';
}

static void testWorld3PipeGroups() {
    // 7 groups × 4 tiles + cloud runway + sign = 30.
    const int groups = 7;
    const int extras = 2;
    check(groups * 4 + extras == kMapNumber, "world 3 fills the pool exactly");
}

static void testScoreBudget() {
    const int w1Max = static_cast<int>(kCoins1.size()) * kCoinScore +
                      static_cast<int>(kEmy1.size()) * kEnemyScore;
    const int w2Max = static_cast<int>(kCoins2.size()) * kCoinScore +
                      static_cast<int>(kEmy2.size()) * kEnemyScore;
    check(w1Max == 20 * 10 + 10 * 5, "world 1 perfect score 250");
    check(w2Max == 11 * 10 + 6 * 5, "world 2 perfect score 140");
    std::cout << "  perfect scores W1=" << w1Max << " W2=" << w2Max
              << " W3=70+35 (7 coins + 7 enemies, random y)\n";
}

int main() {
    std::cout << "levels / world_catalog\n";
    testCaps();
    testFrictionTables();
    testCounts();
    testEnding();
    testSolidVsScenery();
    testWorld3PipeGroups();
    testScoreBudget();
    return test::summary("world_catalog");
}
