#include "map_data.hpp"

#include <algorithm>
#include <string>

namespace mla {
namespace {

const MapRec kWorld1[] = {
    {0, 9, 1, 15, 1},   {18, 8, 1, 9, 1},  {27, 9, 5, 37, 1}, {67, 9, 6, 4, 1},
    {74, 9, 5, 1, 1},   {78, 9, 1, 28, 1}, {0, 10, 3, 15, 2}, {18, 9, 3, 9, 3},
    {27, 10, 4, 37, 2}, {67, 10, 3, 4, 2}, {74, 10, 4, 1, 2}, {78, 10, 3, 28, 2},
    {10, 6, 2, 4, 1},   {40, 4, 2, 3, 1},  {62, 6, 2, 1, 1},  {65, 4, 2, 5, 1},
    {36, 7, 10, 1, 1},  {45, 7, 10, 1, 1}, {80, 7, 10, 1, 1}, {90, 7, 10, 1, 1},
    {19, 6, 11, 1, 1},  {23, 6, 11, 1, 1}, {32, 7, 11, 1, 1}, {49, 7, 11, 1, 1},
    {53, 7, 11, 1, 1},  {57, 7, 11, 1, 1}, {93, 7, 11, 1, 1}, {101, 7, 12, 1, 1},
    {15, 10, 13, 1, 1}, {64, 10, 13, 1, 1}, {71, 10, 13, 1, 1}, {75, 10, 13, 1, 1},
};

const MapRec kWorld2[] = {
    {18, 9, 2, 4, 1},  {23, 6, 2, 6, 1},  {25, 3, 2, 3, 1},  {31, 10, 2, 2, 1},
    {39, 7, 2, 1, 1},  {39, 4, 2, 1, 1},  {96, 7, 2, 2, 1},  {99, 5, 2, 2, 1},
    {102, 3, 2, 12, 1},{0, 10, 5, 15, 1}, {38, 10, 5, 10, 1}, {50, 10, 5, 1, 1},
    {53, 10, 5, 1, 1}, {56, 10, 5, 1, 1}, {61, 10, 5, 10, 1}, {75, 10, 5, 1, 1},
    {82, 10, 5, 11, 1},{0, 11, 4, 15, 2}, {38, 11, 4, 10, 2}, {50, 11, 4, 1, 2},
    {53, 11, 4, 1, 2}, {56, 11, 4, 1, 2}, {61, 11, 4, 10, 2}, {75, 11, 4, 1, 2},
    {82, 11, 4, 11, 2},{67, 9, 2, 1, 1},  {68, 8, 2, 1, 2},   {69, 7, 2, 1, 3},
    {70, 6, 2, 1, 4},  {75, 6, 2, 1, 4},  {42, 8, 11, 1, 1},  {111, 1, 12, 1, 1},
    {4, 8, 14, 1, 1},  {9, 8, 14, 1, 1},  {83, 8, 14, 1, 1},  {89, 8, 14, 1, 1},
};

const PointRec kWorld1Coins[] = {
    {10, 5}, {11, 5}, {12, 5}, {13, 5}, {66, 3}, {67, 3}, {68, 3}, {69, 3},
    {74, 5}, {74, 6}, {74, 7}, {74, 8}, {82, 7}, {83, 8}, {84, 7}, {85, 8},
    {86, 7}, {87, 8}, {88, 7}, {89, 8},
};

const PointRec kWorld2Coins[] = {
    {25, 2}, {26, 2}, {27, 2}, {32, 4}, {32, 5}, {32, 6},
    {32, 7}, {32, 8}, {50, 6}, {53, 6}, {56, 6},
};

const EnemyRec kWorld1Enemies[] = {
    {3, 8, 1},  {18, 7, -1}, {25, 7, 1}, {28, 8, -1}, {33, 8, -1},
    {39, 8, 1}, {68, 3, 1},  {66, 8, -1}, {81, 6, 1}, {92, 6, 1},
};

const EnemyRec kWorld2Enemies[] = {
    {18, 8, 1}, {24, 5, 1}, {28, 5, -1}, {44, 9, 1}, {97, 6, 1}, {99, 4, 1},
};

const MapRec kWorld3Tail[] = {
    {80, 6, 2, 25, 1},
    {101, 4, 12, 1, 1},
};

}  // namespace

const MapRec* world1Map(std::size_t& count) {
    count = sizeof(kWorld1) / sizeof(kWorld1[0]);
    return kWorld1;
}
const MapRec* world2Map(std::size_t& count) {
    count = sizeof(kWorld2) / sizeof(kWorld2[0]);
    return kWorld2;
}
const PointRec* world1Coins(std::size_t& count) {
    count = sizeof(kWorld1Coins) / sizeof(kWorld1Coins[0]);
    return kWorld1Coins;
}
const PointRec* world2Coins(std::size_t& count) {
    count = sizeof(kWorld2Coins) / sizeof(kWorld2Coins[0]);
    return kWorld2Coins;
}
const EnemyRec* world1Enemies(std::size_t& count) {
    count = sizeof(kWorld1Enemies) / sizeof(kWorld1Enemies[0]);
    return kWorld1Enemies;
}
const EnemyRec* world2Enemies(std::size_t& count) {
    count = sizeof(kWorld2Enemies) / sizeof(kWorld2Enemies[0]);
    return kWorld2Enemies;
}
const MapRec* world3FixedTail(std::size_t& count) {
    count = sizeof(kWorld3Tail) / sizeof(kWorld3Tail[0]);
    return kWorld3Tail;
}

std::size_t loadedCount(std::size_t authored, std::size_t cap) {
    return std::min(authored, cap);
}

std::vector<std::string> rasterize(const MapRec* recs, std::size_t count,
                                   int tilesW, int tilesH, std::size_t loadCap) {
    std::vector<std::string> rows(static_cast<std::size_t>(tilesH),
                                  std::string(static_cast<std::size_t>(tilesW), ' '));
    const std::size_t n = loadedCount(count, loadCap);
    for (std::size_t i = 0; i < n; ++i) {
        const MapRec& m = recs[i];
        const char ch = (m.id >= 1 && m.id <= 10) ? '#' : '.';
        const int cellW = (m.id == 8 || m.id == 10) ? 2 : 1;
        const int cellH = (m.id == 8 || m.id == 10) ? 2 : 1;
        for (int j = 0; j < m.xAmount * cellW; ++j) {
            for (int k = 0; k < m.yAmount * cellH; ++k) {
                const int x = m.x + j;
                const int y = m.y + k;
                if (x >= 0 && y >= 0 && x < tilesW && y < tilesH) {
                    rows[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = ch;
                }
            }
        }
    }
    return rows;
}

}  // namespace mla
