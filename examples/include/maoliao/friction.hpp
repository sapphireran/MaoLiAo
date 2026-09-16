#pragma once

#include "constants.hpp"

namespace maoliao {

enum class Surface : int {
    High,   // T1, pipe tops / world-3 solids 1-6
    Mid,    // T2, grass / snow / clouds
    Low,    // T3, default
};

inline double time_for_surface(Surface surface) {
    switch (surface) {
    case Surface::High: return kFrictionTimeHigh;
    case Surface::Mid: return kFrictionTimeMid;
    case Surface::Low: return kFrictionTimeLow;
    }
    return kFrictionTimeLow;
}

inline double u_for_surface(Surface surface) {
    return friction_u(time_for_surface(surface));
}

// Mirrors the switch inside Scene::createMap.
inline Surface surface_for_tile(int id, int world) {
    if (world == 3) {
        if (id >= 1 && id <= 6) {
            if (id == 2) {
                return Surface::Mid;
            }
            return Surface::High;
        }
        if (id == 2) {
            return Surface::Mid;
        }
        return Surface::Low;
    }
    switch (id) {
    case 1:
    case 3:
    case 4:
    case 5:
        return Surface::Mid;
    case 6:
        return Surface::High;
    case 2:
        return Surface::Mid;
    default:
        return Surface::Low;
    }
}

inline double friction_for_tile(int id, int world) {
    return u_for_surface(surface_for_tile(id, world));
}

// Role::action friction extra-acceleration. k only picks a sign:
//   vX < 0 -> k = +2
//   vX > 0 -> k = -2
inline int friction_direction_k(double vx) {
    if (vx < 0.0) {
        const double xabs = -vx;
        return static_cast<int>(xabs / vx + 3.0); // -1 + 3
    }
    if (vx > 0.0) {
        const double xabs = vx;
        return static_cast<int>(xabs / vx - 3.0); // 1 - 3
    }
    return 0;
}

inline double friction_accel(double vx, double u, bool airborne) {
    if (airborne || vx == 0.0) {
        return 0.0;
    }
    return friction_direction_k(vx) * kGravity * u;
}

} // namespace maoliao
