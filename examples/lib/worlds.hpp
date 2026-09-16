#pragma once

#include "aabb.hpp"
#include "constants.hpp"

#include <array>
#include <cstddef>
#include <iterator>
#include <vector>

namespace maoliao {

struct EnemyStamp {
    int tx = 0;
    int ty = 0;
    int turn = 0;
};

struct CoinStamp {
    int tx = 0;
    int ty = 0;
};

struct FoodStamp {
    int x = 0;  // worlds 1–2: pixels; world 3: authored as {10,10}
    int y = 0;
    bool pixels = true;
};

struct WorldDef {
    int id = 0;
    const char* name = "";
    const MapStamp* maps = nullptr;
    std::size_t mapCount = 0;
    const CoinStamp* coins = nullptr;
    std::size_t coinCount = 0;
    const EnemyStamp* enemies = nullptr;
    std::size_t enemyCount = 0;
    FoodStamp food{};
    int endingTiles = 0;
};

// --- World 1 (32 authored stamps; runtime keeps 30) ---

inline constexpr MapStamp kWorld1Maps[] = {
    {0, 9, 1, 15, 1},   {18, 8, 1, 9, 1},  {27, 9, 5, 37, 1}, {67, 9, 6, 4, 1},
    {74, 9, 5, 1, 1},   {78, 9, 1, 28, 1}, {0, 10, 3, 15, 2}, {18, 9, 3, 9, 3},
    {27, 10, 4, 37, 2}, {67, 10, 3, 4, 2}, {74, 10, 4, 1, 2}, {78, 10, 3, 28, 2},
    {10, 6, 2, 4, 1},   {40, 4, 2, 3, 1},  {62, 6, 2, 1, 1},  {65, 4, 2, 5, 1},
    {36, 7, 10, 1, 1},  {45, 7, 10, 1, 1}, {80, 7, 10, 1, 1}, {90, 7, 10, 1, 1},
    {19, 6, 11, 1, 1},  {23, 6, 11, 1, 1}, {32, 7, 11, 1, 1}, {49, 7, 11, 1, 1},
    {53, 7, 11, 1, 1},  {57, 7, 11, 1, 1}, {93, 7, 11, 1, 1}, {101, 7, 12, 1, 1},
    {15, 10, 13, 1, 1}, {64, 10, 13, 1, 1}, {71, 10, 13, 1, 1}, {75, 10, 13, 1, 1},
};

inline constexpr CoinStamp kWorld1Coins[] = {
    {10, 5}, {11, 5}, {12, 5}, {13, 5}, {66, 3}, {67, 3}, {68, 3}, {69, 3},
    {74, 5}, {74, 6}, {74, 7}, {74, 8}, {82, 7}, {83, 8}, {84, 7}, {85, 8},
    {86, 7}, {87, 8}, {88, 7}, {89, 8},
};

inline constexpr EnemyStamp kWorld1Enemies[] = {
    {3, 8, 1},  {18, 7, -1}, {25, 7, 1}, {28, 8, -1}, {33, 8, -1},
    {39, 8, 1}, {68, 3, 1},  {66, 8, -1}, {81, 6, 1}, {92, 6, 1},
};

// --- World 2 (36 authored stamps; runtime keeps 30) ---

inline constexpr MapStamp kWorld2Maps[] = {
    {18, 9, 2, 4, 1},  {23, 6, 2, 6, 1},  {25, 3, 2, 3, 1},  {31, 10, 2, 2, 1},
    {39, 7, 2, 1, 1},  {39, 4, 2, 1, 1},  {96, 7, 2, 2, 1},  {99, 5, 2, 2, 1},
    {102, 3, 2, 12, 1}, {0, 10, 5, 15, 1}, {38, 10, 5, 10, 1}, {50, 10, 5, 1, 1},
    {53, 10, 5, 1, 1}, {56, 10, 5, 1, 1}, {61, 10, 5, 10, 1}, {75, 10, 5, 1, 1},
    {82, 10, 5, 11, 1}, {0, 11, 4, 15, 2}, {38, 11, 4, 10, 2}, {50, 11, 4, 1, 2},
    {53, 11, 4, 1, 2}, {56, 11, 4, 1, 2}, {61, 11, 4, 10, 2}, {75, 11, 4, 1, 2},
    {82, 11, 4, 11, 2}, {67, 9, 2, 1, 1}, {68, 8, 2, 1, 2},  {69, 7, 2, 1, 3},
    {70, 6, 2, 1, 4},  {75, 6, 2, 1, 4},  {42, 8, 11, 1, 1}, {111, 1, 12, 1, 1},
    {4, 8, 14, 1, 1},  {9, 8, 14, 1, 1},  {83, 8, 14, 1, 1}, {89, 8, 14, 1, 1},
};

inline constexpr CoinStamp kWorld2Coins[] = {
    {25, 2}, {26, 2}, {27, 2}, {32, 4}, {32, 5}, {32, 6},
    {32, 7}, {32, 8}, {50, 6}, {53, 6}, {56, 6},
};

inline constexpr EnemyStamp kWorld2Enemies[] = {
    {18, 8, 1}, {24, 5, 1}, {28, 5, -1}, {44, 9, 1}, {97, 6, 1}, {99, 4, 1},
};

inline constexpr EnemyStamp kWorld3Enemies[] = {
    {24, 4, -1}, {34, 6, 1}, {37, 3, -1}, {43, 5, -1},
    {63, 7, 1},  {67, 8, -1}, {86, 5, 1},
};

inline constexpr WorldDef kWorld1{
    1,
    "grassland",
    kWorld1Maps,
    std::size(kWorld1Maps),
    kWorld1Coins,
    std::size(kWorld1Coins),
    kWorld1Enemies,
    std::size(kWorld1Enemies),
    FoodStamp{14 * kWidth, 5 * kHeight, true},
    94,
};

inline constexpr WorldDef kWorld2{
    2,
    "snow",
    kWorld2Maps,
    std::size(kWorld2Maps),
    kWorld2Coins,
    std::size(kWorld2Coins),
    kWorld2Enemies,
    std::size(kWorld2Enemies),
    FoodStamp{39 * kWidth - kWidth / 3, 3 * kHeight + kHeight / 5, true},
    104,
};

inline constexpr WorldDef kWorld3Fixed{
    3,
    "pipes",
    nullptr,
    0,
    nullptr,
    0,
    kWorld3Enemies,
    std::size(kWorld3Enemies),
    FoodStamp{10, 10, false},
    94,
};

inline std::size_t storedMapCount(const WorldDef& w) {
    return std::min(w.mapCount, static_cast<std::size_t>(kMapNumber));
}

// World 3 generator. `rng` should return values in [0, RAND_MAX] like rand().
template <typename Rng>
inline std::vector<MapStamp> generateWorld3Pipes(Rng&& rng) {
    int height[10];
    for (int i = 0; i < 10; ++i) {
        height[i] = rng() % 6 + 1;  // random(1, 7)
    }
    int x[10];
    for (int i = 0; i < 10; ++i) {
        x[i] = i * 10 + 10;
    }

    std::vector<MapStamp> out;
    out.reserve(30);
    const int pairs = 7;  // pairs 7–9 are commented out in scene.cpp
    for (int i = 0; i < pairs; ++i) {
        const int h = height[i];
        const int mx = x[i];
        out.push_back({mx, h - 2, 8, 1, 1});
        out.push_back({mx, 0, 7, 1, std::max(0, h - 2)});
        out.push_back({mx, 4 + h, 10, 1, 1});
        out.push_back({mx, h + 6, 7, 1, std::max(0, 6 - h)});
    }
    out.push_back({80, 6, 2, 25, 1});
    out.push_back({101, 4, 12, 1, 1});
    return out;
}

template <typename Rng>
inline std::vector<CoinStamp> generateWorld3Coins(Rng&& rng) {
    int height[10];
    for (int i = 0; i < 10; ++i) {
        height[i] = rng() % 4 + 3;  // random(3, 7)
    }
    std::vector<CoinStamp> coins;
    for (int i = 0; i < 7; ++i) {
        coins.push_back({5 + 10 * i, height[i]});
    }
    return coins;
}

inline const WorldDef* worldById(int id) {
    switch (id) {
        case 1:
            return &kWorld1;
        case 2:
            return &kWorld2;
        case 3:
            return &kWorld3Fixed;
        default:
            return nullptr;
    }
}

inline char tileGlyph(int id) {
    switch (id) {
        case 1:
            return '#';
        case 2:
            return 'c';
        case 3:
            return '=';
        case 4:
            return 's';
        case 5:
            return 'S';
        case 6:
            return 'I';
        case 7:
            return '|';
        case 8:
            return 'n';
        case 9:
            return 'v';
        case 10:
            return '^';
        case 11:
            return 'g';
        case 12:
            return 'W';
        case 13:
            return '~';
        case 14:
            return 'T';
        default:
            return '?';
    }
}

}  // namespace maoliao
