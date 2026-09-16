#pragma once

// Tunables copied from MaoLiAo/define.h. The homework macros stay in the
// EasyX tree; this header is the portable spelling used by the labs.

namespace maoliao {

constexpr int kXSize = 512;
constexpr int kYSize = 384;
constexpr int kWidth = 32;
constexpr int kHeight = 32;
constexpr int kX0 = 2 * kWidth;          // 64
constexpr int kY0 = 3 * kHeight;         // 96 — documented, not what Role uses
constexpr int kSpawnY = kX0;             // Role::Role sets y = X0
constexpr double kTime = 0.01;
constexpr int kStep = 10;
constexpr int kEnemyStep = 1;
constexpr int kMapBg = 5;
constexpr int kXLeft = 0;
constexpr int kXRight = kWidth * 6;      // 192
constexpr double kRealHeight = 3.5;
constexpr double kUnrealHeight = 3 * kHeight + 5;  // 101
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
constexpr int kBulletStep = 4;           // LEHGTH_INTERVAL_BULLET
constexpr double kBulletInterval = 0.2;
constexpr int kMaxBulletDistance = 480;

constexpr int kScoreCoin = 10;
constexpr int kScoreEnemy = 5;

constexpr double kPxPerMeter = kUnrealHeight / kRealHeight;

inline double friction_u(double time_to_vmax) {
    return (kVMax / time_to_vmax) / kG;
}

inline double u_t1() { return friction_u(kT1); }
inline double u_t2() { return friction_u(kT2); }
inline double u_t3() { return friction_u(kT3); }

}  // namespace maoliao
