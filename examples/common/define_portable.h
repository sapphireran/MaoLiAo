#pragma once

// Portable twin of MaoLiAo/define.h — same numbers, no Windows macros,
// and no trailing semicolons on F / LIFE (see docs/known-quirks.md).

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
constexpr double kUnrealHeight = 3 * kHeight + 5;  // 101
constexpr double kG = 30.0;
constexpr double kVMax = 8.0;
constexpr double kARole = 20.0;
constexpr double kT1 = 0.5;
constexpr double kT2 = 1.2;
constexpr double kT3 = 1.5;
constexpr double kF = kTime * 0.3;
constexpr int kLife = 5;

constexpr double kPixelScale = kUnrealHeight / kRealHeight;

constexpr int kCmdLeft = 1;
constexpr int kCmdRight = 2;
constexpr int kCmdUp = 4;
constexpr int kCmdDown = 8;
constexpr int kCmdShoot = 16;
constexpr int kCmdEsc = 32;
constexpr int kVirReturn = 64;
constexpr int kVirRestart = 128;
constexpr int kVirHome = 256;

constexpr int kEnemyTote = 30;
constexpr int kBombNumber = 5;
constexpr int kBulletNumber = 30;
constexpr int kLengthIntervalBullet = 4;
constexpr double kTimeIntervalBullet = 0.2;
constexpr int kMaxDistance = 480;

constexpr int kMapNumber = 30;
constexpr int kCoinsNumber = 70;
constexpr int kFoodNumber = 5;
constexpr int kScoreNumber = 5;

inline double frictionU(double timeToVmax) {
    return (kVMax / timeToVmax) / kG;
}

}  // namespace maoliao
