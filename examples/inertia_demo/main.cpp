#include "../common/inertia_portable.h"
#include "../common/maoliao_constants.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {

int g_failures = 0;

void expectNear(const char* name, double got, double want, double eps) {
    if (std::fabs(got - want) > eps) {
        std::fprintf(stderr, "FAIL %s: got %.10f want %.10f\n", name, got, want);
        ++g_failures;
    } else {
        std::printf("ok   %s = %.10f\n", name, got);
    }
}

void expectTrue(const char* name, bool cond) {
    if (!cond) {
        std::fprintf(stderr, "FAIL %s\n", name);
        ++g_failures;
    } else {
        std::printf("ok   %s\n", name);
    }
}

}  // namespace

int main() {
    using namespace maoliao;

    std::printf("=== MaoLiAo inertia (portable) ===\n");
    std::printf("TIME=%.2f  G=%.1f  REAL_HEIGHT=%.1f  UNREAL_HEIGHT=%.1f\n",
                kTime, kG, kRealHeight, kUnrealHeight);
    std::printf("pixel scale UNREAL/REAL = %.10f\n", kPixelScale);
    std::printf("V_MAX=%.1f  A_ROLE=%.1f  T1=%.1f T2=%.1f T3=%.1f\n\n",
                kVMax, kARole, kT1, kT2, kT3);

    expectNear("pixel_scale", kPixelScale, 101.0 / 3.5, 1e-12);
    expectNear("jump_vY", jumpSpeed(), -std::sqrt(2.0 * 30.0 * 3.5), 1e-12);

    // One integrator step must match the closed form in inertia.cpp.
    {
        double v = 8.0;
        const double t = kTime;
        const double a = -kARole;
        const double wantX = 8.0 * t + 0.5 * a * t * t;
        const double wantV = 8.0 + a * t;
        const double x = integrate(v, t, a);
        expectNear("step_x", x, wantX, 1e-12);
        expectNear("step_v", v, wantV, 1e-12);
    }

    std::printf("\n-- friction u by tile (world 1) --\n");
    const double uT2 = (kVMax / kT2) / kG;
    const double uT1 = (kVMax / kT1) / kG;
    const double uT3 = (kVMax / kT3) / kG;
    expectNear("u grass/snow/cloud", frictionU(1, 1), uT2, 1e-12);
    expectNear("u ice id6", frictionU(6, 1), uT1, 1e-12);
    expectNear("u default", frictionU(10, 1), uT3, 1e-12);
    expectNear("u world3 pipe default", frictionU(8, 3), uT3, 1e-12);
    expectNear("u world3 snow uses T1", frictionU(5, 3), uT1, 1e-12);

    std::printf("\n-- discrete jump, no ceiling --\n");
    double vY = jumpSpeed();
    double y = 0.0;  // displacement upward in pixels (positive = up)
    double peak = 0.0;
    int peakFrame = 0;
    int landFrame = -1;
    const int maxFrames = 200;
    for (int n = 1; n <= maxFrames; ++n) {
        const double meters = integrate(vY, kTime, kG);
        // Role::action: yy -= -meters * scale  with y increasing downward.
        // Upward pixels this step = -(-meters * scale) when we track height.
        y += -meters * kPixelScale;
        if (y > peak) {
            peak = y;
            peakFrame = n;
        }
        if (landFrame < 0 && y <= 0.0 && n > 1) {
            landFrame = n;
        }
        if (n == 1 || n == 10 || n == 48 || n == 49 || n == 96 || n == 97) {
            std::printf("  frame %3d  vY=%8.4f  height_px=%8.4f\n", n, vY, y);
        }
    }
    std::printf("  peak %.4f px at frame %d (ideal continuous peak = %.1f)\n",
                peak, peakFrame, kUnrealHeight);
    expectTrue("peak_near_101", std::fabs(peak - kUnrealHeight) < 2.0);
    expectTrue("apex_around_48", peakFrame >= 46 && peakFrame <= 50);
    expectTrue("returns_near_start", landFrame > 90 && landFrame < 110);

    std::printf("\n-- ending distances --\n");
    expectNear("world1 end", static_cast<double>(endingDistance(1)), 94.0 * 32.0, 0);
    expectNear("world2 end", static_cast<double>(endingDistance(2)), 104.0 * 32.0, 0);
    expectNear("world3 end", static_cast<double>(endingDistance(3)), 94.0 * 32.0, 0);

    if (g_failures) {
        std::fprintf(stderr, "\n%d check(s) failed\n", g_failures);
        return 1;
    }
    std::printf("\nall inertia checks passed\n");
    return 0;
}
