#pragma once

// Mirror of MaoLiAo/define.h for headless demos. Keep in sync with
// examples/data/constants.json.

constexpr int kXSize = 512;
constexpr int kYSize = 384;
constexpr int kWidth = 32;
constexpr int kHeight = 32;
constexpr double kTime = 0.01;
constexpr double kRealHeight = 3.5;
constexpr int kUnrealHeight = 101;
constexpr double kG = 30.0;
constexpr double kVMax = 8.0;
constexpr double kARole = 20.0;
constexpr double kT1 = 0.5;
constexpr double kT2 = 1.2;
constexpr double kT3 = 1.5;

constexpr int kCmdLeft = 1;
constexpr int kCmdRight = 2;
constexpr int kCmdUp = 4;
constexpr int kCmdDown = 8;
constexpr int kCmdShoot = 16;
constexpr int kCmdEsc = 32;
constexpr int kVirReturn = 64;
constexpr int kVirRestart = 128;
constexpr int kVirHome = 256;

inline double pixelsPerMetre() {
    return static_cast<double>(kUnrealHeight) / kRealHeight;
}

inline double frictionU(double t) {
    return (kVMax / t) / kG;
}
