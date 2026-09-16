#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace mla {

struct MapRec {
    int x;
    int y;
    int id;
    int xAmount;
    int yAmount;
};

struct EnemyRec {
    int x;
    int y;
    int turn;
};

struct PointRec {
    int x;
    int y;
};

// Authored tables transcribed from Scene::createMap / createCoin and
// Role::createEnemy. World 3 pipes are omitted here because they are
// generated with rand(); see world3FixedTail() for the runway + goal.
const MapRec* world1Map(std::size_t& count);
const MapRec* world2Map(std::size_t& count);
const PointRec* world1Coins(std::size_t& count);
const PointRec* world2Coins(std::size_t& count);
const EnemyRec* world1Enemies(std::size_t& count);
const EnemyRec* world2Enemies(std::size_t& count);
const MapRec* world3FixedTail(std::size_t& count);

// How many records the game actually keeps (min(authored, MAP_NUMBER)).
std::size_t loadedCount(std::size_t authored, std::size_t cap = 30);

// Occupancy grid: '#' solid (id 1-10), '.' scenery (11-14), ' ' empty.
// width/height are in tiles. Cells covered by xAmount/yAmount are filled.
std::vector<std::string> rasterize(const MapRec* recs, std::size_t count,
                                   int tilesW, int tilesH, std::size_t loadCap);

}  // namespace mla
