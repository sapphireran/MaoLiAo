#pragma once

#include "maoliao_const.hpp"

namespace maoliao {

struct CameraHero {
    double x = 0.0;
    double xx = 0.0;
    double x0 = 0.0;
    double vX = 0.0;
    bool ending = false;
};

// Apply the right-edge scroll used in Role::action after the inertia step.
inline void applyCameraClamp(CameraHero& h) {
    if (h.x < kXLeft) {
        h.x = kXLeft;
        h.xx = h.x;
        h.vX = 0.0;
        return;
    }
    if (h.x > kXRight && !h.ending) {
        h.x0 -= (h.x - kXRight);
        h.x = kXRight;
        h.xx = h.x;
    }
}

inline double backgroundStep(double vX) {
    const double xabs = (vX < 0.0) ? -vX : vX;
    return xabs * kTime * pixelScale() / kMapBg;
}

inline bool isEnding(int world, int worldX) {
    if (world == 1 || world == 3) {
        return worldX > kEndTilesWorld1 * kWidth;
    }
    if (world == 2) {
        return worldX > kEndTilesWorld2 * kWidth;
    }
    return false;
}

inline int worldX(const CameraHero& h) { return -static_cast<int>(h.x0) + static_cast<int>(h.x); }

}  // namespace maoliao
