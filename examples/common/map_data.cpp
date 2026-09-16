#include "map_data.h"
#include "maoliao_constants.h"

namespace maoliao {

LevelTables world1() {
    LevelTables t;
    t.world = 1;
    t.authoredMaps = {
        {0, 9, 1, 15, 1},  {18, 8, 1, 9, 1},  {27, 9, 5, 37, 1}, {67, 9, 6, 4, 1},
        {74, 9, 5, 1, 1},  {78, 9, 1, 28, 1}, {0, 10, 3, 15, 2}, {18, 9, 3, 9, 3},
        {27, 10, 4, 37, 2},{67, 10, 3, 4, 2}, {74, 10, 4, 1, 2}, {78, 10, 3, 28, 2},
        {10, 6, 2, 4, 1},  {40, 4, 2, 3, 1},  {62, 6, 2, 1, 1},  {65, 4, 2, 5, 1},
        {36, 7, 10, 1, 1}, {45, 7, 10, 1, 1}, {80, 7, 10, 1, 1}, {90, 7, 10, 1, 1},
        {19, 6, 11, 1, 1}, {23, 6, 11, 1, 1}, {32, 7, 11, 1, 1}, {49, 7, 11, 1, 1},
        {53, 7, 11, 1, 1}, {57, 7, 11, 1, 1}, {93, 7, 11, 1, 1}, {101, 7, 12, 1, 1},
        {15, 10, 13, 1, 1},{64, 10, 13, 1, 1},{71, 10, 13, 1, 1},{75, 10, 13, 1, 1},
    };
    t.coins = {
        {10, 5}, {11, 5}, {12, 5}, {13, 5}, {66, 3}, {67, 3}, {68, 3}, {69, 3},
        {74, 5}, {74, 6}, {74, 7}, {74, 8}, {82, 7}, {83, 8}, {84, 7}, {85, 8},
        {86, 7}, {87, 8}, {88, 7}, {89, 8},
    };
    t.enemies = {
        {3, 8, 1},  {18, 7, -1}, {25, 7, 1}, {28, 8, -1}, {33, 8, -1},
        {39, 8, 1}, {68, 3, 1},  {66, 8, -1}, {81, 6, 1}, {92, 6, 1},
    };
    return t;
}

LevelTables world2() {
    LevelTables t;
    t.world = 2;
    t.authoredMaps = {
        {18, 9, 2, 4, 1},  {23, 6, 2, 6, 1},  {25, 3, 2, 3, 1},  {31, 10, 2, 2, 1},
        {39, 7, 2, 1, 1},  {39, 4, 2, 1, 1},  {96, 7, 2, 2, 1},  {99, 5, 2, 2, 1},
        {102, 3, 2, 12, 1},{0, 10, 5, 15, 1}, {38, 10, 5, 10, 1},{50, 10, 5, 1, 1},
        {53, 10, 5, 1, 1}, {56, 10, 5, 1, 1}, {61, 10, 5, 10, 1},{75, 10, 5, 1, 1},
        {82, 10, 5, 11, 1},{0, 11, 4, 15, 2}, {38, 11, 4, 10, 2},{50, 11, 4, 1, 2},
        {53, 11, 4, 1, 2}, {56, 11, 4, 1, 2}, {61, 11, 4, 10, 2},{75, 11, 4, 1, 2},
        {82, 11, 4, 11, 2},{67, 9, 2, 1, 1},  {68, 8, 2, 1, 2},  {69, 7, 2, 1, 3},
        {70, 6, 2, 1, 4},  {75, 6, 2, 1, 4},  {42, 8, 11, 1, 1}, {111, 1, 12, 1, 1},
        {4, 8, 14, 1, 1},  {9, 8, 14, 1, 1},  {83, 8, 14, 1, 1}, {89, 8, 14, 1, 1},
    };
    t.coins = {
        {25, 2}, {26, 2}, {27, 2}, {32, 4}, {32, 5}, {32, 6},
        {32, 7}, {32, 8}, {50, 6}, {53, 6}, {56, 6},
    };
    t.enemies = {
        {18, 8, 1}, {24, 5, 1}, {28, 5, -1}, {44, 9, 1}, {97, 6, 1}, {99, 4, 1},
    };
    return t;
}

std::vector<MapRec> storedMaps(const std::vector<MapRec>& authored) {
    std::vector<MapRec> out;
    for (std::size_t i = 0; i < authored.size() && static_cast<int>(i) < kMapNumber; ++i) {
        if (authored[i].id > 0 && authored[i].id < 15) {
            out.push_back(authored[i]);
        }
    }
    return out;
}

const char* tileName(int id) {
    switch (id) {
    case 1:
        return "grass-top";
    case 2:
        return "cloud";
    case 3:
        return "dirt";
    case 4:
        return "snow-fill";
    case 5:
        return "snow-top";
    case 6:
        return "ice-or-pipe-shaft";
    case 7:
        return "pipe-extender";
    case 8:
        return "pipe-mouth-down";
    case 9:
        return "pipe-unused";
    case 10:
        return "pipe-mouth-up";
    case 11:
        return "bg-grass";
    case 12:
        return "goal-sign";
    case 13:
        return "water";
    case 14:
        return "tree";
    default:
        return "unknown";
    }
}

}  // namespace maoliao
