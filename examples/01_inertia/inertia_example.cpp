// Standalone walkthrough of Inertia::move and the jump / walk scales.
// Compile via: make -C examples

#include "maoliao_core.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>

using namespace maoliao;

static int gFailures = 0;

static void expectNear(const char* name, double got, double want, double eps) {
    if (std::fabs(got - want) > eps) {
        std::cerr << "FAIL " << name << " got " << got << " want " << want << "\n";
        ++gFailures;
    } else {
        std::cout << "ok   " << name << " = " << got << "\n";
    }
}

int main() {
    std::cout << "=== MaoLiAo inertia / jump extract ===\n";
    std::cout << std::fixed << std::setprecision(6);

    const double vLaunch = jumpVelocity();
    expectNear("jump vY", vLaunch, -std::sqrt(2.0 * kGravity * kRealHeight), 1e-12);
    expectNear("pixel scale", pixelScale(), 101.0 / 3.5, 1e-12);

    // One airborne tick, matching Role::action's vertical integrate.
    double vY = vLaunch;
    const double meters = integrate(vY, kTime, kGravity);
    const double pixels = meters * pixelScale();
    expectNear("tick0 meters", meters, vLaunch * kTime + 0.5 * kGravity * kTime * kTime, 1e-12);
    expectNear("tick0 vY", vY, vLaunch + kGravity * kTime, 1e-12);
    expectNear("tick0 pixels", pixels, meters * 101.0 / 3.5, 1e-12);

    // Time to apex: v = v0 + gt = 0 ⇒ t = -v0 / g.
    const double apexT = -vLaunch / kGravity;
    expectNear("apex seconds", apexT, 14.491376751881573 / 30.0, 1e-6);

    // Horizontal: hold right from rest for 20 ticks on a T2 tile, no friction
    // while accelerating with the velocity (vX * a > 0).
    double vX = 0.0;
    double xMeters = 0.0;
    for (int i = 0; i < 20; ++i) {
        xMeters += integrate(vX, kTime, kARole);
        if (std::fabs(vX) > kVMax) {
            vX = (vX / std::fabs(vX)) * kVMax;
        }
    }
    expectNear("20-tick vX capped?", vX, std::min(kARole * 20 * kTime, kVMax), 1e-9);
    std::cout << "     20-tick walk meters=" << xMeters
              << " pixels=" << xMeters * pixelScale() << "\n";

    // Friction k is ±2.
    expectNear("k when vX>0", static_cast<double>(frictionSignK(3.0)), -2.0, 1e-12);
    expectNear("k when vX<0", static_cast<double>(frictionSignK(-3.0)), 2.0, 1e-12);

    const double uGrass = frictionU(1, 1);
    expectNear("u grass T2", uGrass, (kVMax / kT2) / kGravity, 1e-12);
    const double a1 = frictionSignK(3.0) * kGravity * uGrass;
    expectNear("|a1| == 2*Vmax/T2", std::fabs(a1), 2.0 * kVMax / kT2, 1e-12);

    if (gFailures != 0) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "all inertia checks passed\n";
    return 0;
}
