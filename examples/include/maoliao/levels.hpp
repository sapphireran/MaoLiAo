#pragma once

#include "constants.hpp"

#include <array>

namespace maoliao {

constexpr int kWorld1EndTiles = 94;
constexpr int kWorld2EndTiles = 104;
constexpr int kWorld3EndTiles = 94;

inline int ending_distance_px(int world) {
    switch (world) {
    case 1:
        return kWorld1EndTiles * kTileWidth;
    case 2:
        return kWorld2EndTiles * kTileWidth;
    case 3:
        return kWorld3EndTiles * kTileWidth;
    default:
        return 0;
    }
}

inline bool is_ending(int world, int distance_px) {
    return distance_px > ending_distance_px(world);
}

inline bool is_passed(int screen_x) { return screen_x > kScreenWidth; }

struct CoinSpot {
    int x;
    int y;
};

constexpr std::array<CoinSpot, 20> kWorld1Coins = {{
    {10, 5}, {11, 5}, {12, 5}, {13, 5}, {66, 3}, {67, 3}, {68, 3}, {69, 3},
    {74, 5}, {74, 6}, {74, 7}, {74, 8}, {82, 7}, {83, 8}, {84, 7}, {85, 8},
    {86, 7}, {87, 8}, {88, 7}, {89, 8},
}};

constexpr std::array<CoinSpot, 11> kWorld2Coins = {{
    {25, 2},
    {26, 2},
    {27, 2},
    {32, 4},
    {32, 5},
    {32, 6},
    {32, 7},
    {32, 8},
    {50, 6},
    {53, 6},
    {56, 6},
}};

constexpr int kWorld1CoinTotal = static_cast<int>(kWorld1Coins.size()) * 10;
constexpr int kWorld2CoinTotal = static_cast<int>(kWorld2Coins.size()) * 10;

struct EnemySpot {
    int x;
    int y;
    int turn;
};

constexpr std::array<EnemySpot, 10> kWorld1Enemies = {{
    {3, 8, 1},
    {18, 7, -1},
    {25, 7, 1},
    {28, 8, -1},
    {33, 8, -1},
    {39, 8, 1},
    {68, 3, 1},
    {66, 8, -1},
    {81, 6, 1},
    {92, 6, 1},
}};

constexpr std::array<EnemySpot, 6> kWorld2Enemies = {{
    {18, 8, 1},
    {24, 5, 1},
    {28, 5, -1},
    {44, 9, 1},
    {97, 6, 1},
    {99, 4, 1},
}};

constexpr std::array<EnemySpot, 7> kWorld3Enemies = {{
    {24, 4, -1},
    {34, 6, 1},
    {37, 3, -1},
    {43, 5, -1},
    {63, 7, 1},
    {67, 8, -1},
    {86, 5, 1},
}};

inline int perfect_stomp_score(int world) {
    switch (world) {
    case 1:
        return static_cast<int>(kWorld1Enemies.size()) * 5;
    case 2:
        return static_cast<int>(kWorld2Enemies.size()) * 5;
    case 3:
        return static_cast<int>(kWorld3Enemies.size()) * 5;
    default:
        return 0;
    }
}

} // namespace maoliao
