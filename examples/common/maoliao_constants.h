#pragma once

#include <cmath>

// Snapshot of MaoLiAo/define.h and the Role/Scene capacities.
// Keep numeric literals aligned with the game headers.

namespace maoliao {

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
constexpr double kUnrealHeight = 3 * kHeight + 5;
constexpr double kG = 30.0;
constexpr double kVMax = 8.0;
constexpr double kARole = 20.0;
constexpr double kT1 = 0.5;
constexpr double kT2 = 1.2;
constexpr double kT3 = 1.5;
constexpr int kLife = 5;

constexpr int kCmdLeft = 1;
constexpr int kCmdRight = 2;
constexpr int kCmdUp = 4;
constexpr int kCmdDown = 8;
constexpr int kCmdShoot = 16;
constexpr int kCmdEsc = 32;
constexpr int kVirReturn = 64;
constexpr int kVirRestart = 128;
constexpr int kVirHome = 256;

constexpr int kMapNumber = 30;
constexpr int kCoinsNumber = 70;
constexpr int kFoodNumber = 5;
constexpr int kEnemyTote = 30;
constexpr int kBombNumber = 5;
constexpr int kBulletNumber = 30;
constexpr int kBulletPixelStep = 4;
constexpr double kBulletInterval = 0.2;
constexpr int kBulletMaxDistance = 480;

constexpr double kPixelScale = kUnrealHeight / kRealHeight;

inline double frictionU(int tileId, int world) {
    // Scene::createMap switch, worlds 1–2 vs 3.
    if (world == 3) {
        switch (tileId) {
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
    switch (tileId) {
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

inline double jumpSpeed() {
    return -sqrt(2.0 * kG * kRealHeight);
}

}  // namespace maoliao
