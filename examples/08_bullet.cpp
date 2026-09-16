// 08_bullet — screen-space flight, 4 px/tick, MAX_DISTANCE expire.
//
// Matches Role::bullteFlying motion (not the hit resolution). Hold-to
// fire spacing is TIME_INTERVAL_BULLET = 0.2 s → 20 ticks at TIME.

#include "maoliao_core.hpp"

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

void expect_eq(int got, int want, const char* name)
{
    const bool ok = got == want;
    std::printf("  [%s] %s  got=%d want=%d\n", ok ? "PASS" : "FAIL", name, got,
                want);
    if (!ok) {
        ++g_failed;
    }
}

}  // namespace

int main()
{
    std::printf("08_bullet\n");

    expect_eq(maoliao::kLengthIntervalBullet, 4, "LEHGTH_INTERVAL_BULLET");
    expect_eq(maoliao::kMaxDistance, 480, "MAX_DISTANCE");
    expect(maoliao::kTimeIntervalBullet == 0.2, "TIME_INTERVAL_BULLET");

    maoliao::Bullet b;
    b.x = 80;
    b.y = 200;
    b.turn = 1;
    maoliao::bullet_step(b);
    expect_eq(b.x, 84, "right +4");

    b.turn = -1;
    maoliao::bullet_step(b);
    expect_eq(b.x, 80, "left -4");

    expect(!maoliao::bullet_expired_screen(b), "on-screen lives");

    // Off the right of the 512 window.
    b.x = 520;
    b.y = 200;
    b.turn = 1;
    expect(maoliao::bullet_expired_screen(b), "x > XSIZE expires");

    // Off the left (including a full sprite width).
    b.x = -33;
    expect(maoliao::bullet_expired_screen(b), "x < -WIDTH expires");

    // MAX_DISTANCE is 480, inside the 512 window — shots die before
    // the right edge if they fly far enough from a left-side spawn.
    b.x = 481;
    b.y = 100;
    expect(maoliao::bullet_expired_screen(b), "x > 480 expires");

    b.x = 480;
    expect(!maoliao::bullet_expired_screen(b), "x == 480 still lives");

    // Empty slot (0,0) is treated as unused, same as bullteFlying.
    b.x = 0;
    b.y = 0;
    b.turn = 1;
    expect(maoliao::bullet_expired_screen(b), "(0,0) is an empty slot");
    maoliao::bullet_step(b);
    expect_eq(b.x, 0, "empty slot does not move");

    // How many ticks from a typical muzzle (hero.x + WIDTH/2 = 64+16)
    // until MAX_DISTANCE.
    {
        maoliao::Bullet shot;
        shot.x = maoliao::kX0 + maoliao::kWidth / 2;
        shot.y = 200;
        shot.turn = 1;
        int ticks = 0;
        while (!maoliao::bullet_expired_screen(shot) && ticks < 200) {
            maoliao::bullet_step(shot);
            ++ticks;
        }
        const int travel = (maoliao::kMaxDistance - (maoliao::kX0 + 16) +
                            maoliao::kLengthIntervalBullet - 1) /
                           maoliao::kLengthIntervalBullet;
        std::printf("  muzzle 80 → expire in %d ticks (ceil band %d)\n", ticks,
                    travel);
        expect(ticks >= 90 && ticks <= 110, "≈100 ticks to max range");
    }

    // Hold-to-fire: first shot immediate, then every 0.2 / 0.01 = 20 ticks.
    {
        double interval = 0.0;
        bool button_down = false;
        int shots = 0;
        const int hold_ticks = 61;  // 0.61 s
        for (int i = 0; i < hold_ticks; ++i) {
            const bool key_shoot = true;
            if (key_shoot) {
                if (!button_down) {
                    ++shots;
                    button_down = true;
                    interval = 0.0;
                } else {
                    if (interval == 0.0) {
                        ++shots;
                    }
                    interval += maoliao::kTime;
                    if (interval > maoliao::kTimeIntervalBullet) {
                        interval = 0.0;
                    }
                }
            }
        }
        std::printf("  hold-fire 0.61s → %d shots\n", shots);
        // t=0 fire, then when interval hits 0 after exceeding 0.2:
        // fires at 0, 0.20, 0.40, 0.60 → 4 shots.
        expect(shots == 4, "four shots in 0.61 s of hold");
    }

    if (g_failed != 0) {
        std::printf("08_bullet: %d failed\n", g_failed);
        return 1;
    }
    std::printf("08_bullet: all passed\n");
    return 0;
}
