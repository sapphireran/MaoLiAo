#include "kinematics.hpp"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

namespace {

int fail(const char* msg) {
    std::cerr << "FAIL: " << msg << "\n";
    return 1;
}

bool near(double a, double b, double eps) { return std::fabs(a - b) <= eps; }

}  // namespace

int main() {
    using namespace maoliao;

    std::cout << "MaoLiAo inertia_sim\n";
    std::cout << "G=" << kG << " REAL_HEIGHT=" << kRealHeight
              << " UNREAL_HEIGHT=" << kUnrealHeight << " scale=" << pixelScale() << "\n";
    std::cout << "vY0=" << jumpSpeed() << " m/s\n\n";

    const double v0 = jumpSpeed();
    const double expect = -std::sqrt(210.0);
    if (!near(v0, expect, 1e-9)) {
        return fail("jumpSpeed is not -sqrt(2*G*REAL_HEIGHT)");
    }

    const double launchY = 9.0 * kHeight;  // typical grass row
    double apex = launchY;
    int apexFrame = 0;
    std::vector<JumpSample> samples;
    samples.reserve(80);

    std::cout << std::setw(6) << "frame" << std::setw(8) << "t" << std::setw(12) << "vY"
              << std::setw(12) << "y_px" << std::setw(12) << "rise_px"
              << "\n";

    const int frames = simulateJump(launchY, 200, [&](const JumpSample& s) {
        samples.push_back(s);
        const double rise = launchY - s.yPixels;
        if (s.yPixels < apex) {
            apex = s.yPixels;
            apexFrame = s.frame;
        }
        if (s.frame < 8 || s.frame % 10 == 0 || s.vY > 0.0) {
            std::cout << std::setw(6) << s.frame << std::setw(8) << std::fixed
                      << std::setprecision(3) << s.t << std::setw(12) << std::setprecision(4)
                      << s.vY << std::setw(12) << std::setprecision(2) << s.yPixels
                      << std::setw(12) << rise << "\n";
        }
    });

    const double risePx = launchY - apex;
    std::cout << "\nlanded-or-stopped after " << frames << " frames, apex frame " << apexFrame
              << " rise " << risePx << " px (authored UNREAL_HEIGHT=" << kUnrealHeight << ")\n";

    // Discrete Euler does not hit the analytic 101 px exactly; it should be close.
    if (risePx < 95.0 || risePx > 105.0) {
        return fail("jump apex is not near UNREAL_HEIGHT pixels");
    }
    if (apexFrame < 40 || apexFrame > 55) {
        return fail("apex frame is not near v/g = 0.483 s");
    }

    // Grass friction: coasting rightward from V_MAX should brake at ~2*G*u.
    const double uGrass = frictionU(1, false);
    const double expectU = (kVMax / kT2) / kG;
    if (!near(uGrass, expectU, 1e-9)) {
        return fail("grass u mismatch");
    }
    if (frictionSignK(4.0) != -2 || frictionSignK(-4.0) != 2) {
        return fail("friction sign k is not ±2");
    }

    double v = kVMax;
    HorizStep coast = stepHorizontal(v, 0.0, uGrass, true);
    if (coast.vX >= kVMax) {
        return fail("grass coast should slow the runner");
    }
    const double analyticBrake = 2.0 * kG * uGrass;  // 2 * V_MAX / T2
    if (!near(analyticBrake, 2.0 * kVMax / kT2, 1e-9)) {
        return fail("brake identity");
    }

    // Ice (id 6) is grippier than grass.
    double vIce = kVMax;
    double vGrass = kVMax;
    const HorizStep ice = stepHorizontal(vIce, 0.0, frictionU(6, false), true);
    const HorizStep grass = stepHorizontal(vGrass, 0.0, uGrass, true);
    if (ice.vX >= grass.vX) {
        return fail("ice should brake harder than grass");
    }

    // Air: no friction.
    double vAir = kVMax;
    const HorizStep air = stepHorizontal(vAir, 0.0, uGrass, false);
    if (!near(air.vX, kVMax, 1e-9)) {
        return fail("air coast should keep V_MAX");
    }

    // Drive into V_MAX clamp.
    double vRun = 7.9;
    HorizStep run = stepHorizontal(vRun, kARole, uGrass, true);
    if (run.vX > kVMax + 1e-9) {
        return fail("speed exceeded V_MAX");
    }

    std::cout << "friction u grass=" << uGrass << " ice=" << frictionU(6, false)
              << " world3-pipe=" << frictionU(7, true) << "\n";
    std::cout << "coast one tick: grass vX " << grass.vX << " ice vX " << ice.vX << " air vX "
              << air.vX << "\n";
    std::cout << "OK\n";
    return 0;
}
