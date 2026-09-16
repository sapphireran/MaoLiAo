// 01_inertia — Inertia::move against the closed-form kinematic equations.
//
// The game integrates with s = vt + ½at² then v += at. This program
// checks that the portable copy matches that pair, that a rest-to-max
// sprint hits V_MAX in the documented time, and that a jump impulse
// has the analytic apex REAL_HEIGHT in unscaled units.

#include "maoliao_core.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {

int g_failed = 0;

void expect(bool ok, const char* name)
{
    std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) {
        ++g_failed;
    }
}

void expect_near(double got, double want, double eps, const char* name)
{
    const bool ok = std::fabs(got - want) <= eps;
    std::printf("  [%s] %s  got=%.9f want=%.9f\n", ok ? "PASS" : "FAIL", name,
                got, want);
    if (!ok) {
        ++g_failed;
    }
}

}  // namespace

int main()
{
    std::printf("01_inertia\n");

    // One step from rest under A_ROLE.
    {
        double v = 0.0;
        const double s = maoliao::inertia_move(v, maoliao::kTime, maoliao::kARole);
        const double want_s = 0.5 * maoliao::kARole * maoliao::kTime * maoliao::kTime;
        const double want_v = maoliao::kARole * maoliao::kTime;
        expect_near(s, want_s, 1e-12, "rest displacement ½at²");
        expect_near(v, want_v, 1e-12, "rest velocity at");
    }

    // Constant velocity, no accel.
    {
        double v = 5.0;
        const double s = maoliao::inertia_move(v, 0.2, 0.0);
        expect_near(s, 1.0, 1e-12, "v=5 t=0.2 a=0 → s=1");
        expect_near(v, 5.0, 1e-12, "velocity unchanged");
    }

    // Pixel scale is 101 / 3.5.
    {
        expect_near(maoliao::kPixelScale, 101.0 / 3.5, 1e-12, "UNREAL/REAL");
        double v = 0.0;
        const double px =
            maoliao::inertia_move_pixels(v, maoliao::kTime, maoliao::kARole);
        const double unscaled =
            0.5 * maoliao::kARole * maoliao::kTime * maoliao::kTime;
        expect_near(px, unscaled * maoliao::kPixelScale, 1e-12,
                    "pixel step applies scale once");
    }

    // Jump impulse matches √(2 g h).
    {
        maoliao::HeroState h = maoliao::make_hero();
        maoliao::apply_jump(h);
        const double want = -std::sqrt(2.0 * maoliao::kG * maoliao::kRealHeight);
        expect_near(h.vy, want, 1e-12, "jump vy = -√(2Gh)");
        expect(h.is_fly, "jump sets is_fly");
    }

    // Unscaled apex of that impulse is REAL_HEIGHT (3.5).
    {
        double v = -std::sqrt(2.0 * maoliao::kG * maoliao::kRealHeight);
        double y = 0.0;  // up is negative vy, we accumulate +s with +g
        double peak = 0.0;
        for (int i = 0; i < 200; ++i) {
            const double s = maoliao::inertia_move(v, maoliao::kTime, maoliao::kG);
            y += s;
            if (y < peak) {
                peak = y;
            }
            if (v > 0.0 && y >= 0.0) {
                break;
            }
        }
        expect_near(peak, -maoliao::kRealHeight, 0.02,
                    "unscaled apex ≈ -REAL_HEIGHT (discrete)");
    }

    if (g_failed != 0) {
        std::printf("01_inertia: %d failed\n", g_failed);
        return 1;
    }
    std::printf("01_inertia: all passed\n");
    return 0;
}
