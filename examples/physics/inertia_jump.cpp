#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "check.hpp"
#include "inertia.hpp"
#include "maoliao_const.hpp"

using namespace maoliao;
using maoliao::test::check;
using maoliao::test::checkNear;

static void testIntegratorRest() {
    double v = 0.0;
    const double x = move(v, kTime, kARole);
    // X = 0 + 0.5 * 20 * 0.01^2 = 0.001
    checkNear(x, 0.5 * kARole * kTime * kTime, 1e-12, "rest displacement");
    checkNear(v, kARole * kTime, 1e-12, "rest velocity");
}

static void testIntegratorConstantVelocity() {
    double v = 5.0;
    const double x = move(v, kTime, 0.0);
    checkNear(x, 5.0 * kTime, 1e-12, "constant-v displacement");
    checkNear(v, 5.0, 1e-12, "constant-v unchanged");
}

static void testJumpImpulse() {
    const double v0 = jumpSpeed();
    checkNear(v0, -std::sqrt(2.0 * kG * kRealHeight), 1e-12, "jump v0 formula");
    check(v0 < 0.0, "jump starts upward in physics space");
    const double continuousApexTime = std::fabs(v0) / kG;
    checkNear(continuousApexTime, kRealHeight * 2.0 / std::sqrt(2.0 * kG * kRealHeight), 1e-9,
              "apex time |v0|/G");
    checkNear(v0 * v0 / (2.0 * kG), kRealHeight, 1e-12, "continuous apex is REAL_HEIGHT");
}

static void testDiscreteJump() {
    std::vector<JumpTick> ticks(80);
    double apexM = 0.0;
    double apexPx = 0.0;
    const int n = simulateJump(ticks.data(), static_cast<int>(ticks.size()), apexM, apexPx);

    check(n > 40 && n < 55, "apex around 0.48s at 100 Hz");
    checkNear(apexM, kRealHeight, 0.05, "discrete apex near 3.5 m");
    check(apexPx < 0.0, "screen Y decreases while rising");
    checkNear(std::fabs(apexPx), kUnrealHeight, 2.0, "pixel apex near UNREAL_HEIGHT");

    // Last tick should have flipped or be at vY ~= 0.
    check(ticks[static_cast<std::size_t>(n - 1)].vY >= -0.5, "vY near zero at recorded apex");

    std::cout << "  jump ticks to apex: " << n << "  apex m=" << std::fixed << std::setprecision(4)
              << apexM << "  apex px=" << apexPx << '\n';
}

static void testAccelToVMax() {
    double v = 0.0;
    int frames = 0;
    while (v < kVMax && frames < 2000) {
        (void)move(v, kTime, kARole);
        ++frames;
        if (v > kVMax) {
            v = kVMax;
            break;
        }
    }
    // Continuous: v = a t => t = 8/20 = 0.4 s = 40 frames. Discrete overshoots one step.
    check(frames >= 40 && frames <= 42, "time to V_MAX under A_ROLE");
    checkNear(v, kVMax, 1e-9, "clamped at V_MAX");
}

static void testFrictionK() {
    check(frictionK(4.0) == -2, "rightward k");
    check(frictionK(-3.0) == 2, "leftward k");
    const double u2 = frictionU(kT2);
    const double a1Right = frictionK(4.0) * kG * u2;
    checkNear(a1Right, -2.0 * kVMax / kT2, 1e-9, "deceleration is 2 V_MAX / T");
    checkNear(frictionU(kT1), (kVMax / kT1) / kG, 1e-12, "T1 u");
    checkNear(frictionU(kT3), (kVMax / kT3) / kG, 1e-12, "T3 u");
    check(frictionU(kT1) > frictionU(kT2), "T1 is the grippiest");
    check(frictionU(kT2) > frictionU(kT3), "T3 is the iciest");
}

static void testStopFromVMax(double tConst, const char* label) {
    double v = kVMax;
    const double u = frictionU(tConst);
    const double a1 = frictionK(v) * kG * u;
    int frames = 0;
    while (v > 0.0 && frames < 5000) {
        const double prev = v;
        (void)move(v, kTime, a1);
        if (prev * v < 0.0) {
            v = 0.0;
            ++frames;
            break;
        }
        ++frames;
    }
    // Comment claims T seconds; implementation is twice that deceleration, so ~T/2.
    const double expected = tConst / 2.0;
    checkNear(frames * kTime, expected, 0.05, std::string("stop time ") + label);
    std::cout << "  stop from V_MAX (" << label << "): " << frames << " frames ("
              << frames * kTime << " s), comment T=" << tConst << " s\n";
}

static void printAsciiArc() {
    std::vector<JumpTick> ticks(80);
    double apexM = 0.0;
    double apexPx = 0.0;
    const int n = simulateJump(ticks.data(), static_cast<int>(ticks.size()), apexM, apexPx);
    std::cout << "  jump arc (each '@' is 2 frames, height in meters):\n";
    for (int i = 0; i < n; i += 2) {
        const int bars = static_cast<int>(ticks[static_cast<std::size_t>(i)].realY * 8.0);
        std::ostringstream line;
        line << "    " << std::setw(5) << std::fixed << std::setprecision(2)
             << ticks[static_cast<std::size_t>(i)].realY << " |";
        for (int b = 0; b < bars; ++b) {
            line << '#';
        }
        std::cout << line.str() << '\n';
    }
}

int main() {
    std::cout << "physics / inertia_jump\n";
    testIntegratorRest();
    testIntegratorConstantVelocity();
    testJumpImpulse();
    testDiscreteJump();
    testAccelToVMax();
    testFrictionK();
    testStopFromVMax(kT1, "T1 pipe");
    testStopFromVMax(kT2, "T2 grass");
    testStopFromVMax(kT3, "T3 default");
    printAsciiArc();
    checkNear(pixelScale(), kUnrealHeight / kRealHeight, 1e-12, "pixel scale");
    checkNear(kUnrealHeight, 101.0, 1e-12, "UNREAL_HEIGHT = 3*32+5");
    check(kXRight == 192, "right camera pin");
    return test::summary("inertia_jump");
}
