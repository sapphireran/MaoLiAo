#pragma once

#include <cstddef>
#include <vector>

namespace maoliao {

struct MapRec {
    int x;
    int y;
    int id;
    int xAmount;
    int yAmount;
};

struct TilePoint {
    int x;
    int y;
};

struct EnemyRec {
    int x;  // tile column
    int y;  // tile row
    int turn;
};

struct LevelTables {
    std::vector<MapRec> authoredMaps;
    std::vector<TilePoint> coins;
    std::vector<EnemyRec> enemies;
    int world;
};

LevelTables world1();
LevelTables world2();

// Same cap as Scene::createMap: copy while id in (0,15) and index < MAP_NUMBER.
std::vector<MapRec> storedMaps(const std::vector<MapRec>& authored);

const char* tileName(int id);

}  // namespace maoliao
