#pragma once

// World 1 placement tables transcribed from Scene::createMap / createCoin
// and Role::createEnemy. Trailing decorative tiles that overflow MAP_NUMBER
// are still listed here so level_dump can show the author's full sketch.

#include "aabb.hpp"

#include <cstddef>

namespace mla {
namespace world1 {

constexpr Tile kMap[] = {
    {0, 9, 1, 15, 1},   {18, 8, 1, 9, 1},  {27, 9, 5, 37, 1}, {67, 9, 6, 4, 1},
    {74, 9, 5, 1, 1},   {78, 9, 1, 28, 1}, {0, 10, 3, 15, 2}, {18, 9, 3, 9, 3},
    {27, 10, 4, 37, 2}, {67, 10, 3, 4, 2}, {74, 10, 4, 1, 2}, {78, 10, 3, 28, 2},
    {10, 6, 2, 4, 1},   {40, 4, 2, 3, 1},  {62, 6, 2, 1, 1},  {65, 4, 2, 5, 1},
    {36, 7, 10, 1, 1},  {45, 7, 10, 1, 1}, {80, 7, 10, 1, 1}, {90, 7, 10, 1, 1},
    {19, 6, 11, 1, 1},  {23, 6, 11, 1, 1}, {32, 7, 11, 1, 1}, {49, 7, 11, 1, 1},
    {53, 7, 11, 1, 1},  {57, 7, 11, 1, 1}, {93, 7, 11, 1, 1}, {101, 7, 12, 1, 1},
    {15, 10, 13, 1, 1}, {64, 10, 13, 1, 1}, {71, 10, 13, 1, 1}, {75, 10, 13, 1, 1},
};

constexpr int kMapCount = static_cast<int>(sizeof(kMap) / sizeof(kMap[0]));
constexpr int kMapCapacity = 30; // Scene.h MAP_NUMBER

struct Cell {
    int x;
    int y;
};

constexpr Cell kCoins[] = {
    {10, 5}, {11, 5}, {12, 5}, {13, 5}, {66, 3}, {67, 3}, {68, 3}, {69, 3},
    {74, 5}, {74, 6}, {74, 7}, {74, 8}, {82, 7}, {83, 8}, {84, 7}, {85, 8},
    {86, 7}, {87, 8}, {88, 7}, {89, 8},
};

constexpr int kCoinCount = static_cast<int>(sizeof(kCoins) / sizeof(kCoins[0]));

struct EnemyCell {
    int x;
    int y;
    int turn;
};

constexpr EnemyCell kEnemies[] = {
    {3, 8, 1},  {18, 7, -1}, {25, 7, 1}, {28, 8, -1}, {33, 8, -1},
    {39, 8, 1}, {68, 3, 1},  {66, 8, -1}, {81, 6, 1}, {92, 6, 1},
};

constexpr int kEnemyCount =
    static_cast<int>(sizeof(kEnemies) / sizeof(kEnemies[0]));

// Food is stored in pixels in the game.
constexpr Point kFood{14 * kWidth, 5 * kHeight};

inline const Tile* collidingPrefix(int* outCount) {
    // First 20 rows are ids 1–10 (the solid set in the initializer).
    // Decorative ids start at index 20.
    int n = 0;
    for (int i = 0; i < kMapCount; ++i) {
        if (tileCollides(kMap[i].id)) {
            n = i + 1;
        }
    }
    if (outCount) {
        *outCount = n;
    }
    return kMap;
}

inline char glyphForId(int id) {
    switch (id) {
    case 1:
        return '='; // grass
    case 2:
        return 'C'; // cloud
    case 3:
        return '#'; // dirt
    case 4:
        return ':'; // snow fill
    case 5:
        return '-'; // snow / alt top
    case 6:
        return 'I'; // high-friction strip
    case 7:
        return '|';
    case 8:
        return 'v';
    case 10:
        return 'P'; // pipe mouth
    case 11:
        return ','; // grass tuft
    case 12:
        return 'F'; // flag
    case 13:
        return '~'; // water
    case 14:
        return 'T';
    default:
        return '?';
    }
}

} // namespace world1
} // namespace mla
