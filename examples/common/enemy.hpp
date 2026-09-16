#pragma once

#include "collision.hpp"
#include "map_data.hpp"

#include <cstddef>
#include <vector>

namespace mla {

struct Patrol {
    int x;
    int y;
    int turn;
    Patrol() : x(0), y(0), turn(1) {}
};

// Reverse if the body overlaps a solid or the leading foot has no floor.
// Mirrors the two hitMap probes in Role::action's enemy loop.
bool shouldReverse(const Patrol& e, const MapRec* map, std::size_t mapCount,
                   int cameraX0 = 0);

void stepPatrol(Patrol& e, const MapRec* map, std::size_t mapCount, int step);

// Build pixel-space solids from tile records (id 1-10 only).
std::vector<Rect> solidBoxes(const MapRec* map, std::size_t mapCount);

}  // namespace mla
