#pragma once

#include <cmath>

// Numeric values from MaoLiAo/define.h. Not the header itself: that file
// #define-s max(), F, and LIFE with trailing semicolons.

namespace maoliao {

constexpr int kXSize = 512;
constexpr int kYSize = 384;
constexpr int kWidth = 32;
constexpr int kHeight = 32;
constexpr int kX0 = 2 * kWidth;
constexpr int kY0 = 3 * kHeight;
constexpr double kTime = 0.01;
constexpr double kG = 30.0;
constexpr double kRealHeight = 3.5;
constexpr int kUnrealHeight = 3 * kHeight + 5;
constexpr double kVMax = 8.0;
constexpr double kARole = 20.0;
constexpr double kT1 = 0.5;
constexpr double kT2 = 1.2;
constexpr double kT3 = 1.5;
constexpr int kXLeft = 0;
constexpr int kXRight = kWidth * 6;
constexpr int kMapBg = 5;

inline double metres_to_pixels(double metres)
{
    return metres * static_cast<double>(kUnrealHeight) / kRealHeight;
}

inline double jump_takeoff()
{
    return -std::sqrt(2.0 * kG * kRealHeight);
}

inline double friction_u(int id, bool pipes)
{
    double t = kT3;
    if (pipes) {
        if (id == 1 || id == 3 || id == 4 || id == 5 || id == 6)
            t = kT1;
        else if (id == 2)
            t = kT2;
    } else {
        if (id == 1 || id == 2 || id == 3 || id == 4 || id == 5)
            t = kT2;
        else if (id == 6)
            t = kT1;
    }
    return (kVMax / t) / kG;
}

// Role::action: k = Xabs/vX ± 3  →  -2 when vX>0, +4 when vX<0.
inline double friction_k(double vx)
{
    return (vx < 0.0) ? 4.0 : -2.0;
}

inline double friction_accel(int id, bool pipes, double vx)
{
    return friction_k(vx) * kG * friction_u(id, pipes);
}

}  // namespace maoliao
