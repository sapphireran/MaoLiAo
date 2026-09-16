// 06_jump_sim — discrete jump vs the 101-pixel design height.
//
// apply_jump sets vY = -√(2 G REAL_HEIGHT). Each air tick adds
// inertia_move(vY, TIME, G) * (UNREAL/REAL) to y (down positive).
// This program records the peak and the hang time.

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

}  // namespace

int main()
{
    std::printf("06_jump_sim\n");

    maoliao::HeroState h = maoliao::make_hero();
    const int start_y = 9 * maoliao::kHeight;  // standing on world-1 grass
    h.y = start_y;
    h.yy = start_y;
    h.is_fly = false;
    h.vy = 0.0;

    maoliao::apply_jump(h);

    int min_y = h.y;
    int ticks_up = 0;
    int ticks = 0;
    const int kMaxTicks = 200;

    for (; ticks < kMaxTicks; ++ticks) {
        // No ceiling; land when we fall back to start_y.
        const bool probe = (h.vy > 0.0 && h.y + 1 >= start_y);
        const bool landed = maoliao::tick_vertical(h, probe);
        if (h.y < min_y) {
            min_y = h.y;
            ticks_up = ticks + 1;
        }
        if (landed) {
            ++ticks;
            break;
        }
    }

    const int rise = start_y - min_y;
    std::printf("  start_y=%d  peak_y=%d  rise=%d px  ticks_up=%d  ticks_total=%d\n",
                start_y, min_y, rise, ticks_up, ticks);

    // Design height is UNREAL_HEIGHT = 101. Discrete integration with
    // TIME=0.01 undershoots by a few pixels; lock a tight band so a
    // future constant tweak is obvious.
    expect(rise >= 96 && rise <= 101, "rise in [96, 101] px");
    expect_near(static_cast<double>(rise), maoliao::kUnrealHeight, 6.0,
                "rise ≈ UNREAL_HEIGHT");
    expect(ticks_up >= 40 && ticks_up <= 55, "apex around 0.48 s");
    expect(ticks >= 90 && ticks <= 110, "round trip ≈ 1.0 s");
    expect(!h.is_fly, "landed");
    expect(h.vy == 0.0, "vY cleared on land");
    expect(h.y == start_y, "snapped back to the floor row");
    expect(!h.died, "did not fall out of the world");

    // Fall death: jump from below the floor with no probe.
    {
        maoliao::HeroState fall = maoliao::make_hero();
        fall.y = 350;
        fall.yy = 350;
        fall.is_fly = true;
        fall.vy = 5.0;
        bool died = false;
        for (int i = 0; i < 80; ++i) {
            maoliao::tick_vertical(fall, false);
            if (fall.died) {
                died = true;
                break;
            }
        }
        expect(died, "falling past YSIZE sets died");
    }

    // World-3 style air jump: a second impulse mid-air raises the peak.
    {
        maoliao::HeroState bird = maoliao::make_hero();
        bird.y = start_y;
        bird.yy = start_y;
        maoliao::apply_jump(bird);
        int mid_peak = bird.y;
        for (int i = 0; i < 20; ++i) {
            maoliao::tick_vertical(bird, false);
            if (bird.y < mid_peak) {
                mid_peak = bird.y;
            }
        }
        maoliao::apply_jump(bird);
        int second_peak = bird.y;
        for (int i = 0; i < 80; ++i) {
            maoliao::tick_vertical(bird, false);
            if (bird.y < second_peak) {
                second_peak = bird.y;
            }
        }
        std::printf("  flappy: first-leg peak=%d  after second tap peak=%d\n",
                    mid_peak, second_peak);
        expect(second_peak < mid_peak, "air jump climbs higher than one tap");
    }

    if (g_failed != 0) {
        std::printf("06_jump_sim: %d failed\n", g_failed);
        return 1;
    }
    std::printf("06_jump_sim: all passed\n");
    return 0;
}
