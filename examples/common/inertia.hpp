#pragma once

#include <cmath>

#include "maoliao_const.hpp"

namespace maoliao {

// Byte-for-byte the same integrator as MaoLiAo/inertia.cpp.
inline double move(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

// Friction sign used in Role::action. |k| is 2, not 1 — a 2020 tuning leftover.
inline int frictionK(double vX) {
    const double xabs = (vX < 0.0) ? -vX : vX;
    if (vX < 0.0) {
        return static_cast<int>(xabs / vX + 3.0);
    }
    return static_cast<int>(xabs / vX - 3.0);
}

struct JumpTick {
    int frame;
    double vY;
    double realY;    // meters, up positive after we flip the integrator output
    double pixelY;   // screen space, down positive, starting at 0
};

// Integrate an unobstructed jump until vY >= 0 (apex) or maxFrames.
inline int simulateJump(JumpTick* out, int maxFrames, double& apexMeters, double& apexPixels) {
    double vY = jumpSpeed();
    double realUp = 0.0;
    double pixelY = 0.0;
    apexMeters = 0.0;
    apexPixels = 0.0;
    int n = 0;
    for (int i = 0; i < maxFrames; ++i) {
        const double disp = move(vY, kTime, kG);
        realUp += -disp;
        pixelY += disp * pixelScale();
        if (out != nullptr && i < maxFrames) {
            out[i] = JumpTick{i, vY, realUp, pixelY};
        }
        ++n;
        if (realUp > apexMeters) {
            apexMeters = realUp;
        }
        if (pixelY < apexPixels) {
            apexPixels = pixelY;
        }
        if (vY >= 0.0) {
            break;
        }
    }
    return n;
}

}  // namespace maoliao
