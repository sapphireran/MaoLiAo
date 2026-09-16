// Replay Inertia::move for a jump and a grass sprint.
// Expected: apex ≈ 101 px, time-to-V_MAX = 0.40 s while accelerating.

#include "maoliao_core.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace maoliao;

static int fail = 0;

static void expect_near(const char* name, double got, double want, double eps) {
    if (std::fabs(got - want) > eps) {
        std::printf("FAIL %s: got %.6f want %.6f (eps %.6f)\n", name, got, want, eps);
        ++fail;
    } else {
        std::printf("ok   %s: %.6f\n", name, got);
    }
}

static void expect_eq(const char* name, int got, int want) {
    if (got != want) {
        std::printf("FAIL %s: got %d want %d\n", name, got, want);
        ++fail;
    } else {
        std::printf("ok   %s: %d\n", name, got);
    }
}

int main() {
    std::printf("== jump impulse ==\n");
    const double v0 = jump_impulse();
    expect_near("v0", v0, -std::sqrt(2.0 * kG * kRealHeight), 1e-12);
    expect_near("designed_real_apex_m", kRealHeight, 3.5, 1e-12);
    expect_near("px_per_meter", kPxPerMeter, 101.0 / 3.5, 1e-12);

    std::printf("\n== integrate jump until vY crosses 0 ==\n");
    double vy = v0;
    double yy = 0.0; // pixel y, up is negative after the game's double-neg
    int frames = 0;
    double min_yy = 0.0;
    while (vy < 0.0 && frames < 2000) {
        // Role::action: yy = yy - (-move(vY, TIME, G) * scale)
        const double meters = inertia_move(vy, kTime, kG);
        yy -= -meters * kPxPerMeter;
        if (yy < min_yy) {
            min_yy = yy;
        }
        ++frames;
    }
    const double apex_px = -min_yy;
    std::printf("frames_to_apex=%d  apex_px=%.4f  vY_after=%.6f\n", frames, apex_px, vy);
    expect_near("apex_px", apex_px, kUnrealHeight, 1.5); // one-step integration error
    expect_near("time_to_apex_s", frames * kTime, std::fabs(v0) / kG, 0.02);

    std::printf("\n== hold D from rest on grass (friction off while vX*a > 0) ==\n");
    double vx = 0.0;
    double xx = 0.0;
    int run_frames = 0;
    while (std::fabs(vx) < kVMax - 1e-9 && run_frames < 2000) {
        const double a = kARole;
        const double meters = inertia_move(vx, kTime, a);
        xx += meters * kPxPerMeter;
        if (std::fabs(vx) > kVMax) {
            vx = (vx / std::fabs(vx)) * kVMax;
        }
        ++run_frames;
    }
    std::printf("frames_to_vmax=%d  vx=%.6f  xx_px=%.4f\n", run_frames, vx, xx);
    expect_eq("frames_to_vmax", run_frames, 40); // 8/20 / 0.01
    expect_near("vx", vx, kVMax, 1e-9);

    std::printf("\n== coast with grass friction (k=-2, u=T2) ==\n");
    const double u = friction_u(1, 1);
    expect_near("u_grass", u, (kVMax / kT2) / kG, 1e-12);
    expect_eq("k_right", friction_k(1.0), -2);
    expect_eq("k_left", friction_k(-1.0), 2);

    vx = kVMax;
    int coast = 0;
    while (vx > 0.0 && coast < 2000) {
        const double a1 = friction_k(vx) * kG * u;
        const double prev = vx;
        inertia_move(vx, kTime, a1);
        if (prev * vx < 0.0) {
            vx = 0.0;
        }
        ++coast;
    }
    std::printf("frames_to_stop=%d  vx=%.6f\n", coast, vx);
    // a = 2*G*u = 2*(8/1.2) ≈ 13.333 m/s²; t = 8/13.333 ≈ 0.60 s → 60 frames
    expect_near("coast_time_s", coast * kTime, kVMax / (2.0 * kG * u), 0.02);

    std::printf("\n== sign flip clamp ==\n");
    vx = 0.05;
    const double before = vx;
    inertia_move(vx, kTime, -kARole);
    if (before * vx < 0.0) {
        vx = 0.0;
    }
    expect_near("clamped_vx", vx, 0.0, 1e-12);

    if (fail) {
        std::printf("\n%d check(s) failed\n", fail);
        return 1;
    }
    std::printf("\nall inertia checks passed\n");
    return 0;
}
