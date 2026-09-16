#include "map_portable.h"

#include "define_portable.h"

namespace maoliao {

double frictionForIdOverworld(int id) {
    switch (id) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            return frictionU(kT2);
        case 6:
            return frictionU(kT1);
        default:
            return frictionU(kT3);
    }
}

double frictionForIdWorld3(int id) {
    switch (id) {
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
            return frictionU(kT1);
        case 2:
            return frictionU(kT2);
        default:
            return frictionU(kT3);
    }
}

int assignFriction(MapCell* cells, int count, int world) {
    int n = 0;
    for (int i = 0; i < count; ++i) {
        if (cells[i].id <= 0) {
            continue;
        }
        cells[i].u = (world == 3) ? frictionForIdWorld3(cells[i].id)
                                  : frictionForIdOverworld(cells[i].id);
        ++n;
    }
    return n;
}

bool isEnding(int world, int distancePx) {
    switch (world) {
        case 1:
        case 3:
            return distancePx > 94 * kWidth;
        case 2:
            return distancePx > 104 * kWidth;
        default:
            return false;
    }
}

}  // namespace maoliao
