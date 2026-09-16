// 07_friction — stop distance on T1 vs T2 tiles.
//
// u = (V_MAX / T) / G. When coasting, a1 = ±2 G u (the k=±2 trick in
// Role::action). T1 (id 6) stops sooner than T2 (grass / snow).

#include "maoliao_core.hpp"

#include <cmath>
#include <cstdio>

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
    std::printf("  [%s] %s  got=%.6f want=%.6f\n", ok ? "PASS" : "FAIL", name,
                got, want);
    if (!ok) {
        ++g_failed;
    }
}

int coast_ticks(double u)
{
    maoliao::HeroState h = maoliao::make_hero();
    h.is_fly = false;
    h.vx = maoliao::kVMax;
    h.x = 64;
    h.xx = 64;
    int ticks = 0;
    while (h.vx > 1e-9 && ticks < 500) {
        // No hold: a = 0, so friction applies.
        maoliao::tick_horizontal(h, 0, true, u);
        ++ticks;
    }
    return ticks;
}

}  // namespace

int main()
{
    std::printf("07_friction\n");

    const double u_t1 = maoliao::friction_u(6, 1);
    const double u_t2 = maoliao::friction_u(1, 1);
    const double u_t3 = maoliao::friction_u(11, 1);

    expect_near(u_t1, (maoliao::kVMax / maoliao::kT1) / maoliao::kG, 1e-12,
                "id 6 → T1");
    expect_near(u_t2, (maoliao::kVMax / maoliao::kT2) / maoliao::kG, 1e-12,
                "id 1 → T2");
    expect_near(u_t3, (maoliao::kVMax / maoliao::kT3) / maoliao::kG, 1e-12,
                "decor → T3");
    expect(u_t1 > u_t2 && u_t2 > u_t3, "T1 > T2 > T3  (larger u, grippier)");

    // Coasting right: k = 1 - 3 = -2, a1 = -2 G u.
    const double a1 = maoliao::friction_accel(maoliao::kVMax, 0.0, u_t2, true);
    expect_near(a1, -2.0 * maoliao::kG * u_t2, 1e-12, "coasting a1 = -2Gu");

    // Holding right: v and a same sign → no friction.
    const double a1_hold =
        maoliao::friction_accel(maoliao::kVMax, maoliao::kARole, u_t2, true);
    expect_near(a1_hold, 0.0, 1e-12, "accel with v → no friction");

    // Airborne: no friction even when coasting.
    const double a1_air =
        maoliao::friction_accel(maoliao::kVMax, 0.0, u_t2, false);
    expect_near(a1_air, 0.0, 1e-12, "airborne coast → no friction");

    // Leftward coast uses k = 2.
    const double a1_left =
        maoliao::friction_accel(-maoliao::kVMax, 0.0, u_t2, true);
    expect_near(a1_left, 2.0 * maoliao::kG * u_t2, 1e-12, "left coast a1 = +2Gu");

    const int t1_ticks = coast_ticks(u_t1);
    const int t2_ticks = coast_ticks(u_t2);
    const int t3_ticks = coast_ticks(u_t3);
    std::printf("  coast-to-stop ticks  T1=%d  T2=%d  T3=%d\n", t1_ticks,
                t2_ticks, t3_ticks);

    expect(t1_ticks < t2_ticks && t2_ticks < t3_ticks,
           "grippier tiles stop in fewer ticks");

    // Analytic coast time v / |a1| = V_MAX / (2 G u) = T/2.
    // T1=0.5 → 0.25s → 25 ticks; T2=1.2 → 0.6s → 60; T3=1.5 → 0.75s → 75.
    // Discrete zero-cross snap can land on or just after that count.
    expect(t1_ticks >= 24 && t1_ticks <= 28, "T1 coast ≈ T1/2 seconds");
    expect(t2_ticks >= 58 && t2_ticks <= 64, "T2 coast ≈ T2/2 seconds");
    expect(t3_ticks >= 73 && t3_ticks <= 80, "T3 coast ≈ T3/2 seconds");

    // Hold right from rest: speed approaches V_MAX, camera pins at 192.
    {
        maoliao::HeroState h = maoliao::make_hero();
        h.is_fly = false;
        h.x = 64;
        h.xx = 64;
        const int hold = maoliao::pack_command(false, true, false, false, false,
                                               false);
        for (int i = 0; i < 400; ++i) {
            maoliao::tick_horizontal(h, hold, true, u_t2);
        }
        const double xabs = (h.vx < 0.0) ? -h.vx : h.vx;
        std::printf("  sprint: vx=%.4f  screen_x=%d  x0=%.2f  world_x=%d\n",
                    h.vx, h.x, h.x0, maoliao::world_x(h));
        expect(xabs >= maoliao::kVMax - 1e-6, "hits V_MAX");
        expect(h.x == maoliao::kXRight, "screen x pinned at 192");
        expect(h.x0 < 0.0, "camera x0 scrolled negative");
        expect(maoliao::world_x(h) > 192, "world x keeps growing");
    }

    if (g_failed != 0) {
        std::printf("07_friction: %d failed\n", g_failed);
        return 1;
    }
    std::printf("07_friction: all passed\n");
    return 0;
}
