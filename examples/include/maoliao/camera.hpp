#pragma once

#include "maoliao/constants.hpp"
#include "maoliao/kinematics.hpp"

namespace maoliao {

struct Camera {
    double x0 = 0;   // hero origin (negative as you walk right)
    int x = kX0;     // screen x
    double xx = kX0;
    double vX = 0;
    bool ending = false;
    bool passed = false;
};

inline bool is_ending(int world, int world_x) {
    const int tiles = (world == 2) ? 104 : 94;
    return world_x > tiles * kWidth;
}

// Apply the XLEFT / XRIGHT rail after a horizontal step of `dx` pixels.
inline void apply_rail(Camera& c, double dx) {
    c.xx += dx;
    c.x = static_cast<int>(c.xx);

    const int world_x = -static_cast<int>(c.x0) + c.x;
    if (is_ending(1, world_x)) {
        c.ending = true;
    }
    if (c.x > kXSize) {
        c.passed = true;
    }

    if (c.x < kXLeft) {
        c.x = kXLeft;
        c.xx = c.x;
        c.vX = 0;
    } else if (c.x > kXRight && !c.ending) {
        c.x0 -= (c.x - kXRight);
        c.x = kXRight;
        c.xx = c.x;
    }
}

inline double sky_step(double vX, bool pinned_right, bool map_moved) {
    if (!(pinned_right && vX > 0 && map_moved)) {
        return 0;
    }
    double xabs = vX < 0 ? -vX : vX;
    return xabs * kTime * kPxPerMeter / kMapBg;
}

}  // namespace maoliao
