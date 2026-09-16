#pragma once

// Numbers copied from MaoLiAo/define.h, without the trailing-semicolon macros.

#include <cmath>

namespace maoliao {

constexpr int kScreenWidth = 512;
constexpr int kScreenHeight = 384;
constexpr int kTileWidth = 32;
constexpr int kTileHeight = 32;

constexpr int kSpawnX = 2 * kTileWidth;   // X0
constexpr int kSpawnYDefined = 3 * kTileHeight; // Y0 in define.h
constexpr int kSpawnYActual = kSpawnX;    // Role::Role assigns y = X0

constexpr double kTick = 0.01;            // TIME
constexpr int kWalkFrameStep = 10;        // STEP
constexpr int kEnemyStep = 1;             // ENEMY_STEP
constexpr int kMapBgRatio = 5;            // K_MAP_BG

constexpr int kLeftLimit = 0;             // XLEFT
constexpr int kRightLimit = kTileWidth * 6; // XRIGHT = 192

constexpr double kRealJumpMeters = 3.5;   // REAL_HEIGHT
constexpr double kPixelJumpHeight = 3 * kTileHeight + 5; // UNREAL_HEIGHT = 101
constexpr double kMetersToPixels = kPixelJumpHeight / kRealJumpMeters;

constexpr double kGravity = 30.0;         // G
constexpr double kMaxSpeed = 8.0;         // V_MAX
constexpr double kWalkAccel = 20.0;       // A_ROLE

constexpr double kFrictionTimeHigh = 0.5;   // T1 pipe
constexpr double kFrictionTimeMid = 1.2;    // T2 grass / snow / cloud
constexpr double kFrictionTimeLow = 1.5;    // T3 default

constexpr double kSceneryFrameRate = kTick * 0.3; // F, without the ';'
constexpr int kLifeCount = 5;                     // LIFE, without the ';'

constexpr int kBulletStep = 4;            // LEHGTH_INTERVAL_BULLET
constexpr double kBulletInterval = 0.2;   // TIME_INTERVAL_BULLET
constexpr int kBulletMaxDistance = 480;   // MAX_DISTANCE

constexpr int kMapCapacity = 30;
constexpr int kCoinCapacity = 70;
constexpr int kScoreFxCapacity = 5;
constexpr int kFoodCapacity = 5;
constexpr int kEnemyCapacity = 30;
constexpr int kBombCapacity = 5;
constexpr int kBulletCapacity = 30;

constexpr int kCmdLeft = 1;
constexpr int kCmdRight = 2;
constexpr int kCmdUp = 4;
constexpr int kCmdDown = 8;
constexpr int kCmdShoot = 16;
constexpr int kCmdEsc = 32;
constexpr int kVirReturn = 64;
constexpr int kVirRestart = 128;
constexpr int kVirHome = 256;

inline double pixel_scale() { return kMetersToPixels; }

inline double jump_launch_speed() {
    return -std::sqrt(2.0 * kGravity * kRealJumpMeters);
}

inline double friction_u(double time_to_vmax) {
    return (kMaxSpeed / time_to_vmax) / kGravity;
}

} // namespace maoliao
