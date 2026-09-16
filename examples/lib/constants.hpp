#pragma once

// Constants copied from MaoLiAo/define.h so examples compile without EasyX.
// Values must stay in lockstep with the game header.

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
constexpr int kLife = 5;

constexpr int kMapNumber = 30;
constexpr int kCoinsNumber = 70;
constexpr int kFoodNumber = 5;
constexpr int kEnemyTote = 30;

constexpr int kCmdLeft = 1;
constexpr int kCmdRight = 2;
constexpr int kCmdUp = 4;
constexpr int kCmdDown = 8;
constexpr int kCmdShoot = 16;
constexpr int kCmdEsc = 32;
constexpr int kVirReturn = 64;
constexpr int kVirRestart = 128;
constexpr int kVirHome = 256;

constexpr int kBulletMaxDistance = 480;
constexpr int kBulletStep = 4;
constexpr double kBulletInterval = 0.2;

inline constexpr double pixelScale() {
    return kUnrealHeight / kRealHeight;
}

}  // namespace maoliao
