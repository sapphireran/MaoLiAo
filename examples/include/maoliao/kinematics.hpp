#pragma once

#include <cmath>

#include "maoliao/constants.hpp"

namespace maoliao {

// Inertia::move — updates v in place, returns displacement in meters.
inline double move(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

inline double jump_speed() {
    return -std::sqrt(2.0 * kG * kRealHeight);
}

inline double meters_to_px(double meters) {
    return meters * kPxPerMeter;
}

// One airborne frame matching Role::action:
//   yy = yy - (-move(vY, TIME, G) * UNREAL / REAL)
inline void air_step(double& yy, double& vY) {
    const double meters = move(vY, kTime, kG);
    yy += meters_to_px(meters);
}

struct JumpApex {
    double meters = 0;
    double pixels = 0;
    int frames = 0;
    double vY = 0;
};

// Integrate a jump from rest until vY crosses through zero (discrete apex).
inline JumpApex jump_apex() {
    double vY = jump_speed();
    double y_m = 0;  // upward-positive meters for the report
    JumpApex best;
    best.vY = vY;
    for (int i = 0; i < 2000; ++i) {
        const double before = vY;
        const double step = move(vY, kTime, kG);
        y_m += -step;  // step is signed with +down; negate for height
        ++best.frames;
        if (y_m > best.meters) {
            best.meters = y_m;
            best.pixels = meters_to_px(y_m);
        }
        if (before < 0.0 && vY >= 0.0) {
            break;
        }
    }
    best.vY = vY;
    return best;
}

// Homework k for friction: |k| = 2, sign opposite vX.
inline int friction_k(double vX) {
    const double xabs = vX < 0 ? -vX : vX;
    if (vX < 0) {
        return static_cast<int>(xabs / vX + 3);  // -1 + 3 = 2
    }
    return static_cast<int>(xabs / vX - 3);  // 1 - 3 = -2
}

// One grounded horizontal step. a_walk is ±A_ROLE or 0.
// If a floor is present and v opposes a_walk, apply k*G*u.
inline double walk_step(double& vX, double a_walk, double u, bool on_floor) {
    double a1 = 0;
    if (vX * a_walk <= 0.0 && vX != 0.0) {
        if (on_floor) {
            a1 = friction_k(vX) * kG * u;
        }
    }
    const double tmp = vX;
    double meters = move(vX, kTime, a_walk + a1);
    if (tmp * vX < 0.0) {
        vX = 0.0;
    }
    const double xabs = vX < 0 ? -vX : vX;
    if (xabs > kVMax) {
        vX = vX / xabs * kVMax;
    }
    return meters_to_px(meters);
}

}  // namespace maoliao
