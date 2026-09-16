#include "enemy.hpp"

#include "constants.hpp"

namespace mla {

std::vector<Rect> solidBoxes(const MapRec* map, std::size_t mapCount) {
    std::vector<Rect> boxes;
    boxes.reserve(mapCount);
    for (std::size_t i = 0; i < mapCount; ++i) {
        if (map[i].id >= 1 && map[i].id <= 10) {
            boxes.push_back(mapBox(map[i].x, map[i].y, map[i].id, map[i].xAmount,
                                   map[i].yAmount));
        }
    }
    return boxes;
}

namespace {

bool hitsAny(int px, int py, const std::vector<Rect>& boxes) {
    const Point p{px, py};
    for (const Rect& b : boxes) {
        if (vertexHitsRect(p, b, kWidth, kHeight)) {
            return true;
        }
    }
    return false;
}

}  // namespace

bool shouldReverse(const Patrol& e, const MapRec* map, std::size_t mapCount,
                   int cameraX0) {
    const std::vector<Rect> boxes = solidBoxes(map, mapCount);
    // Role feeds window-space coords into hitMap, which subtracts x0 again.
    // For a world-space patrol we pass cameraX0 = 0 and world pixels.
    (void)cameraX0;
    const int x1 = e.x;
    const int y1 = e.y;
    const int x2 = e.x + e.turn * kWidth;
    const int y2 = e.y + 1;
    return hitsAny(x1, y1, boxes) || !hitsAny(x2, y2, boxes);
}

void stepPatrol(Patrol& e, const MapRec* map, std::size_t mapCount, int step) {
    e.x += e.turn * step;
    if (shouldReverse(e, map, mapCount, 0)) {
        e.turn *= -1;
    }
}

}  // namespace mla
