#pragma once

#include <cstddef>
#include <vector>

#include "maoliao/aabb.hpp"
#include "maoliao/constants.hpp"

namespace maoliao {

struct Actor {
    int x = 0;
    int y = 0;
    int turn = 0;
};

inline double u_for_id(int id, int world) {
    if (world == 3) {
        switch (id) {
            case 1:
            case 3:
            case 4:
            case 5:
            case 6:
                return u_t1();
            case 2:
                return u_t2();
            default:
                return u_t3();
        }
    }
    switch (id) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            return u_t2();
        case 6:
            return u_t1();
        default:
            return u_t3();
    }
}

inline std::vector<Tile> world1_authored() {
    const Tile raw[] = {
        {0, 9, 1, 15, 1},   {18, 8, 1, 9, 1},  {27, 9, 5, 37, 1}, {67, 9, 6, 4, 1},
        {74, 9, 5, 1, 1},   {78, 9, 1, 28, 1}, {0, 10, 3, 15, 2}, {18, 9, 3, 9, 3},
        {27, 10, 4, 37, 2}, {67, 10, 3, 4, 2}, {74, 10, 4, 1, 2}, {78, 10, 3, 28, 2},
        {10, 6, 2, 4, 1},   {40, 4, 2, 3, 1},  {62, 6, 2, 1, 1},  {65, 4, 2, 5, 1},
        {36, 7, 10, 1, 1},  {45, 7, 10, 1, 1}, {80, 7, 10, 1, 1}, {90, 7, 10, 1, 1},
        {19, 6, 11, 1, 1},  {23, 6, 11, 1, 1}, {32, 7, 11, 1, 1}, {49, 7, 11, 1, 1},
        {53, 7, 11, 1, 1},  {57, 7, 11, 1, 1}, {93, 7, 11, 1, 1}, {101, 7, 12, 1, 1},
        {15, 10, 13, 1, 1}, {64, 10, 13, 1, 1}, {71, 10, 13, 1, 1}, {75, 10, 13, 1, 1},
    };
    std::vector<Tile> out;
    for (const Tile& t : raw) {
        Tile c = t;
        c.u = u_for_id(t.id, 1);
        out.push_back(c);
    }
    return out;
}

inline std::vector<Tile> world2_authored() {
    const Tile raw[] = {
        {18, 9, 2, 4, 1},  {23, 6, 2, 6, 1},  {25, 3, 2, 3, 1},  {31, 10, 2, 2, 1},
        {39, 7, 2, 1, 1},  {39, 4, 2, 1, 1},  {96, 7, 2, 2, 1},  {99, 5, 2, 2, 1},
        {102, 3, 2, 12, 1},
        {0, 10, 5, 15, 1}, {38, 10, 5, 10, 1}, {50, 10, 5, 1, 1}, {53, 10, 5, 1, 1},
        {56, 10, 5, 1, 1}, {61, 10, 5, 10, 1}, {75, 10, 5, 1, 1}, {82, 10, 5, 11, 1},
        {0, 11, 4, 15, 2}, {38, 11, 4, 10, 2}, {50, 11, 4, 1, 2}, {53, 11, 4, 1, 2},
        {56, 11, 4, 1, 2}, {61, 11, 4, 10, 2}, {75, 11, 4, 1, 2}, {82, 11, 4, 11, 2},
        {67, 9, 2, 1, 1},  {68, 8, 2, 1, 2},   {69, 7, 2, 1, 3},  {70, 6, 2, 1, 4},
        {75, 6, 2, 1, 4},
        {42, 8, 11, 1, 1}, {111, 1, 12, 1, 1},
        {4, 8, 14, 1, 1},  {9, 8, 14, 1, 1},   {83, 8, 14, 1, 1}, {89, 8, 14, 1, 1},
    };
    std::vector<Tile> out;
    for (const Tile& t : raw) {
        Tile c = t;
        c.u = u_for_id(t.id, 2);
        out.push_back(c);
    }
    return out;
}

// Homework copy: while (id>0 && id<15 && i < MAP_NUMBER). Safe version.
inline std::vector<Tile> stored_map(const std::vector<Tile>& authored) {
    std::vector<Tile> out;
    for (std::size_t i = 0; i < authored.size() && static_cast<int>(i) < kMapNumber; ++i) {
        if (authored[i].id <= 0 || authored[i].id >= 15) {
            break;
        }
        out.push_back(authored[i]);
    }
    return out;
}

inline std::vector<Actor> world1_coins() {
    const Actor raw[] = {
        {10, 5}, {11, 5}, {12, 5}, {13, 5}, {66, 3}, {67, 3}, {68, 3}, {69, 3},
        {74, 5}, {74, 6}, {74, 7}, {74, 8}, {82, 7}, {83, 8}, {84, 7}, {85, 8},
        {86, 7}, {87, 8}, {88, 7}, {89, 8},
    };
    return std::vector<Actor>(raw, raw + sizeof(raw) / sizeof(raw[0]));
}

inline std::vector<Actor> world2_coins() {
    const Actor raw[] = {
        {25, 2}, {26, 2}, {27, 2}, {32, 4}, {32, 5}, {32, 6},
        {32, 7}, {32, 8}, {50, 6}, {53, 6}, {56, 6},
    };
    return std::vector<Actor>(raw, raw + sizeof(raw) / sizeof(raw[0]));
}

inline std::vector<Actor> world1_enemies() {
    const Actor raw[] = {
        {3, 8, 1},  {18, 7, -1}, {25, 7, 1}, {28, 8, -1}, {33, 8, -1},
        {39, 8, 1}, {68, 3, 1},  {66, 8, -1}, {81, 6, 1}, {92, 6, 1},
    };
    return std::vector<Actor>(raw, raw + sizeof(raw) / sizeof(raw[0]));
}

inline std::vector<Actor> world2_enemies() {
    const Actor raw[] = {
        {18, 8, 1}, {24, 5, 1}, {28, 5, -1}, {44, 9, 1}, {97, 6, 1}, {99, 4, 1},
    };
    return std::vector<Actor>(raw, raw + sizeof(raw) / sizeof(raw[0]));
}

inline std::vector<Actor> world3_enemies() {
    const Actor raw[] = {
        {24, 4, -1}, {34, 6, 1}, {37, 3, -1}, {43, 5, -1},
        {63, 7, 1},  {67, 8, -1}, {86, 5, 1},
    };
    return std::vector<Actor>(raw, raw + sizeof(raw) / sizeof(raw[0]));
}

inline int ending_tiles(int world) { return world == 2 ? 104 : 94; }

inline int world1_food_x() { return 14 * kWidth; }
inline int world1_food_y() { return 5 * kHeight; }
inline int world2_food_x() { return 39 * kWidth - kWidth / 3; }
inline int world2_food_y() { return 3 * kHeight + kHeight / 5; }

// World-3 pipe group, matching createMap(3) with a supplied height in 1..6.
inline std::vector<Tile> pipe_group(int x, int height) {
    const int h = height;
    const Tile raw[] = {
        {x, h - 2, 8, 1, 1},
        {x, 0, 7, 1, h - 2 > 0 ? h - 2 : 0},
        {x, 4 + h, 10, 1, 1},
        {x, h + 6, 7, 1, 6 - h > 0 ? 6 - h : 0},
    };
    std::vector<Tile> out;
    for (const Tile& t : raw) {
        Tile c = t;
        c.u = u_for_id(t.id, 3);
        out.push_back(c);
    }
    return out;
}

inline std::vector<Tile> world3_from_heights(const int height[7]) {
    std::vector<Tile> out;
    for (int i = 0; i < 7; ++i) {
        auto g = pipe_group(i * 10 + 10, height[i]);
        out.insert(out.end(), g.begin(), g.end());
    }
    Tile runway{80, 6, 2, 25, 1, u_for_id(2, 3)};
    Tile flag{101, 4, 12, 1, 1, u_for_id(12, 3)};
    out.push_back(runway);
    out.push_back(flag);
    return out;
}

}  // namespace maoliao
