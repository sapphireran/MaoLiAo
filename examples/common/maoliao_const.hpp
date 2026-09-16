#pragma once

#include <cmath>

// Portable copy of MaoLiAo/define.h constants.
// LIFE in the original is written `#define LIFE 5;` — the trailing semicolon
// is absorbed by `int life = LIFE;`. We store the intended numeric value.

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
constexpr double kSceneryF = kTime * 0.3;
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

constexpr int kEnemyTote = 30;
constexpr int kBombNumber = 5;
constexpr int kBulletNumber = 30;
constexpr int kBulletInterval = kWidth;
constexpr int kBulletFlight = 4;
constexpr double kBulletHoldGap = 0.2;
constexpr int kMaxBulletDistance = 480;

constexpr int kMapNumber = 30;
constexpr int kCoinsNumber = 70;
constexpr int kScorePops = 5;
constexpr int kFoodNumber = 5;

constexpr int kCoinScore = 10;
constexpr int kEnemyScore = 5;

constexpr int kEndTilesWorld1 = 94;
constexpr int kEndTilesWorld2 = 104;
constexpr int kEndTilesWorld3 = 94;

inline double pixelScale() { return kUnrealHeight / kRealHeight; }

inline double frictionU(double timeToVMax) { return (kVMax / timeToVMax) / kG; }

inline double jumpSpeed() { return -std::sqrt(2.0 * kG * kRealHeight); }

}  // namespace maoliao
