#pragma once

// Twin of the Map struct and Scene::createMap friction switch.

namespace maoliao {

struct MapCell {
    int x = 0;
    int y = 0;
    int id = 0;
    int xAmount = 0;
    int yAmount = 0;
    double u = 0.0;
};

// Worlds 1–2 table from Scene::createMap.
double frictionForIdOverworld(int id);

// World 3 table (ice on 1,3–6; medium on 2; slip default).
double frictionForIdWorld3(int id);

// Write u onto each cell. Returns how many cells were assigned.
int assignFriction(MapCell* cells, int count, int world);

// True when world-space pixels pass Scene::isEnding.
bool isEnding(int world, int distancePx);

}  // namespace maoliao
