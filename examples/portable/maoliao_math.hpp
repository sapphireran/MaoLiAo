#pragma once

// Portable copy of MaoLiAo/define.h constants and Inertia::move.
// Keep in sync with the game headers. No EasyX, no Windows.

#include <cmath>

namespace mla {

constexpr int kXSize = 512;
constexpr int kYSize = 384;
constexpr int kWidth = 32;
constexpr int kHeight = 32;
constexpr int kX0 = 2 * kWidth;
constexpr int kY0 = 3 * kHeight;
constexpr double kTime = 0.01;
constexpr int kStep = 10;
constexpr int kEnemyStep = 1;
constexpr int kMapBg = 5;
constexpr int kXLeft = 0;
constexpr int kXRight = kWidth * 6;
constexpr double kRealHeight = 3.5;
constexpr double kUnrealHeight = 3 * kHeight + 5; // 101
constexpr double kGravity = 30.0;
constexpr double kVMax = 8.0;
constexpr double kAccelRole = 20.0;
constexpr double kT1 = 0.5;
constexpr double kT2 = 1.2;
constexpr double kT3 = 1.5;
constexpr int kLife = 5;
constexpr int kMaxBulletDistance = 480;
constexpr int kBulletStep = 4;

constexpr double kPixelScale = kUnrealHeight / kRealHeight;

inline double frictionU(int tileId, int world) {
    // Mirrors Scene::createMap switch bodies.
    if (world == 3) {
        switch (tileId) {
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
            return (kVMax / kT1) / kGravity;
        case 2:
            return (kVMax / kT2) / kGravity;
        default:
            return (kVMax / kT3) / kGravity;
        }
    }
    switch (tileId) {
    case 1:
    case 3:
    case 4:
    case 5:
        return (kVMax / kT2) / kGravity;
    case 6:
        return (kVMax / kT1) / kGravity;
    case 2:
        return (kVMax / kT2) / kGravity;
    default:
        return (kVMax / kT3) / kGravity;
    }
}

// Same integrator as Inertia::move(double& v, double t, double a).
inline double move(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

inline double jumpSpeed() {
    return -std::sqrt(2.0 * kGravity * kRealHeight);
}

inline double metersToPixels(double meters) {
    return meters * kPixelScale;
}

inline int endingDistance(int world) {
    switch (world) {
    case 1:
        return 94 * kWidth;
    case 2:
        return 104 * kWidth;
    case 3:
        return 94 * kWidth;
    default:
        return 0;
    }
}

inline bool isEnding(int world, int distance) {
    return distance > endingDistance(world);
}

} // namespace mla
