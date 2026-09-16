#pragma once

#include <cstddef>
#include <vector>

namespace maoliao {

struct MapTile {
    int x = 0;
    int y = 0;
    int id = 0;
    int x_amount = 1;
    int y_amount = 1;
    double u = 0;
};

struct TileCoord {
    int x = 0;
    int y = 0;
};

struct EnemySpawn {
    int tile_x = 0;
    int tile_y = 0;
    int turn = 1;
};

struct FoodSpawn {
    int pixel_x = 0;
    int pixel_y = 0;
};

struct Level {
    int world = 1;
    int ending_tiles = 94;
    std::vector<MapTile> tiles;
    std::vector<TileCoord> coins;
    std::vector<FoodSpawn> food;
    std::vector<EnemySpawn> enemies;
};

// Authored tables (may be longer than kMapNumber).
Level authored_level(int world);

// What Scene::createMap actually keeps (first kMapNumber tiles).
Level loaded_level(int world);

// World 3 with a deterministic RNG (the game uses unseeded rand()).
Level authored_world3(unsigned seed);

int ending_distance_px(int world);
double friction_for_id(int world, int id);

}  // namespace maoliao
