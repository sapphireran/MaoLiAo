#pragma once

#include "constants.hpp"

#include <cmath>

namespace maoliao {

// Same integrator as MaoLiAo/inertia.cpp Inertia::move.
inline double integrate(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

inline double jumpSpeed() {
    return -std::sqrt(2.0 * kG * kRealHeight);
}

inline double metersToPixels(double meters) {
    return meters * pixelScale();
}

// Friction coefficient Scene::createMap assigns from tile id.
// world3 uses the grippy T1 branch for ids 1,3,4,5,6.
inline double frictionU(int id, bool world3) {
    if (world3) {
        switch (id) {
            case 1:
            case 3:
            case 4:
            case 5:
            case 6:
                return (kVMax / kT1) / kG;
            case 2:
                return (kVMax / kT2) / kG;
            default:
                return (kVMax / kT3) / kG;
        }
    }
    switch (id) {
        case 1:
        case 3:
        case 4:
        case 5:
            return (kVMax / kT2) / kG;
        case 6:
            return (kVMax / kT1) / kG;
        case 2:
            return (kVMax / kT2) / kG;
        default:
            return (kVMax / kT3) / kG;
    }
}

// Sign trick from Role::action: k is +2 when vX < 0, -2 when vX > 0.
inline int frictionSignK(double vX) {
    if (vX == 0.0) {
        return 0;
    }
    const double xabs = (vX < 0.0) ? -vX : vX;
    if (vX < 0.0) {
        return static_cast<int>(xabs / vX + 3.0);
    }
    return static_cast<int>(xabs / vX - 3.0);
}

struct HorizStep {
    double vX;
    double deltaMeters;
    double deltaPixels;
};

// One horizontal integration with optional ground friction, matching Role::action.
inline HorizStep stepHorizontal(double vX, double aDrive, double u, bool grounded) {
    double a1 = 0.0;
    if (vX * aDrive <= 0.0 && vX != 0.0) {
        if (grounded) {
            a1 = frictionSignK(vX) * kG * u;
        }
    }
    const double tmp = vX;
    const double hMeters = integrate(vX, kTime, aDrive + a1);
    if (tmp * vX < 0.0) {
        vX = 0.0;
    }
    const double xabs = (vX < 0.0) ? -vX : vX;
    if (xabs > kVMax) {
        vX = vX / xabs * kVMax;
    }
    HorizStep out;
    out.vX = vX;
    out.deltaMeters = hMeters;
    out.deltaPixels = metersToPixels(hMeters);
    return out;
}

struct JumpSample {
    int frame;
    double t;
    double vY;
    double yPixels;  // screen-y, 0 at launch, decreasing while rising
};

// Integrate a hop until we fall back through the launch y, or maxFrames.
template <typename Fn>
inline int simulateJump(double launchY, int maxFrames, Fn&& onFrame) {
    double vY = jumpSpeed();
    double y = launchY;
    int frame = 0;
    for (; frame < maxFrames; ++frame) {
        const double meters = integrate(vY, kTime, kG);
        y = y + metersToPixels(meters);
        JumpSample s;
        s.frame = frame;
        s.t = (frame + 1) * kTime;
        s.vY = vY;
        s.yPixels = y;
        onFrame(s);
        if (frame > 2 && y >= launchY && vY > 0.0) {
            ++frame;
            break;
        }
    }
    return frame;
}

}  // namespace maoliao
