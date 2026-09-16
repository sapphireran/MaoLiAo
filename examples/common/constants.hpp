#pragma once

#include <cmath>

// Numbers copied from MaoLiAo/define.h so examples can run without EasyX.
// Keep this file in lockstep when the game macros change.

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
constexpr int kBulletNumber = 30;
constexpr int kMaxDistance = 480;
constexpr int kBulletStep = 4;
constexpr double kBulletInterval = 0.2;

inline double pxPerMetre() { return kUnrealHeight / kRealHeight; }

inline double frictionMu(double timeToVmax) {
    return (kVMax / timeToVmax) / kG;
}

inline double muHigh() { return frictionMu(kT1); }  // ~0.5333
inline double muMid() { return frictionMu(kT2); }   // ~0.2222
inline double muLow() { return frictionMu(kT3); }   // ~0.1778

inline double jumpTakeoffVy() {
    // v = -sqrt(2 g H)  — same formula as Role::action
    return -std::sqrt(2.0 * kG * kRealHeight);
}

inline int endingDistance(int world) {
    if (world == 2) {
        return 104 * kWidth;
    }
    return 94 * kWidth;
}

}  // namespace mla
