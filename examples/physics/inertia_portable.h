#pragma once

// Portable copy of MaoLiAo/inertia.cpp. The original header pulls in
// graphics.h / scene.h even though move() is pure math.

namespace maoliao {

inline double move(double& v, double t, double a)
{
    double X = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return X;
}

}  // namespace maoliao
