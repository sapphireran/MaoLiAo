#include "inertia_portable.h"
#include "physics_constants.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace {

int g_failed = 0;

void expect(bool ok, const char* name)
{
    if (ok) {
        std::cout << "OK    " << name << "\n";
        return;
    }
    std::cout << "FAIL  " << name << "\n";
    ++g_failed;
}

void expect_near(double got, double want, double eps, const char* name)
{
    const bool ok = std::fabs(got - want) <= eps;
    if (ok) {
        std::cout << "OK    " << name << "  got=" << got << "\n";
        return;
    }
    std::cout << "FAIL  " << name << "  got=" << got << " want=" << want
              << " eps=" << eps << "\n";
    ++g_failed;
}

}  // namespace

int main()
{
    using namespace maoliao;

    expect(kXSize == 512 && kYSize == 384, "screen 512x384");
    expect(kWidth == 32 && kUnrealHeight == 101, "tile 32 / unreal 101");
    expect(kXRight == 192, "camera rail at 192");

    double v = 4.0;
    const double step = move(v, 0.01, -10.0);
    expect_near(step, 4.0 * 0.01 + 0.5 * -10.0 * 0.01 * 0.01, 1e-12, "Inertia::move displacement");
    expect_near(v, 4.0 + -10.0 * 0.01, 1e-12, "Inertia::move mutates v");

    expect_near(jump_takeoff(), -std::sqrt(2.0 * 30.0 * 3.5), 1e-12, "takeoff -sqrt(2gH)");
    expect_near(metres_to_pixels(kRealHeight), 101.0, 1e-12, "3.5m maps to 101px");

    double vY = jump_takeoff();
    double metres = 0.0;
    double apex = 0.0;
    for (int i = 0; i < 2000; ++i) {
        metres += -move(vY, kTime, kG);
        if (metres > apex)
            apex = metres;
        if (vY >= 0.0)
            break;
    }
    expect_near(metres_to_pixels(apex), 101.0, 2.0, "discrete jump apex within 2px of 101");
    expect(apex > 3.4 && apex < 3.6, "discrete jump apex near 3.5m");

    expect_near(friction_u(1, false), (8.0 / 1.2) / 30.0, 1e-12, "u grass T2");
    expect_near(friction_u(6, false), (8.0 / 0.5) / 30.0, 1e-12, "u slick T1");
    expect_near(friction_u(10, false), (8.0 / 1.5) / 30.0, 1e-12, "u pipe T3");
    expect_near(friction_u(6, true), (8.0 / 0.5) / 30.0, 1e-12, "u pipe-profile id6 T1");
    expect_near(friction_k(+3.0), -2.0, 0.0, "k facing right");
    expect_near(friction_k(-3.0), 4.0, 0.0, "k facing left");
    expect_near(std::fabs(friction_accel(1, false, -1.0)) / std::fabs(friction_accel(1, false, +1.0)),
                2.0, 1e-12, "left stop is 2x right stop");

    // Hold right from x=64 with no walls: must pin to 192 and slide x0.
    double xx = static_cast<double>(kX0);
    double x0 = 0.0;
    double vx = 0.0;
    bool railed = false;
    for (int i = 0; i < 400; ++i) {
        const double tmp = vx;
        double H = move(vx, kTime, kARole) * static_cast<double>(kUnrealHeight) / kRealHeight;
        if (tmp * vx < 0.0)
            vx = 0.0;
        xx += H;
        double x = xx;
        const double ax = std::fabs(vx);
        if (ax > kVMax)
            vx = vx / ax * kVMax;
        if (x > kXRight) {
            railed = true;
            x0 -= (x - kXRight);
            x = kXRight;
            xx = x;
        }
    }
    expect(railed, "camera rail engaged");
    expect_near(xx, static_cast<double>(kXRight), 1e-6, "sprite pinned at XRIGHT");
    expect(x0 < 0.0, "origin x0 slides left");
    expect((-x0 + xx) > kXRight, "map-x keeps growing past the rail");
    expect_near(std::fabs(vx), kVMax, 1e-6, "vx clamped at V_MAX");

    if (g_failed) {
        std::cout << g_failed << " checks failed\n";
        return 1;
    }
    std::cout << "all physics checks passed\n";
    return 0;
}
