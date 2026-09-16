#include "maoliao/hero.h"

#include "maoliao/constants.h"
#include "maoliao/inertia.h"

namespace maoliao {

void step_hero_free(HeroState& h, const HeroInput& in, double ground_u) {
    double a = 0;
    if (in.left && !h.ending) {
        a -= kARole;
    }
    if (in.right || h.ending) {
        a += kARole;
    }

    const bool can_jump =
        (in.jump && !h.airborne && !h.ending) || (in.jump && h.world == 3);
    if (can_jump) {
        h.airborne = true;
        h.v_y = jump_speed();
    }

    if (h.airborne) {
        const double dy = inertia_move_pixels(h.v_y, kTime, kG);
        h.y += dy;
    }

    double a1 = 0;
    if (!h.airborne && h.v_x != 0 && h.v_x * a <= 0) {
        const double k = (h.v_x < 0) ? 2.0 : -2.0;
        a1 = k * kG * ground_u;
    }

    const double tmp = h.v_x;
    const double dx = inertia_move_pixels(h.v_x, kTime, a + a1);
    if (tmp * h.v_x < 0) {
        h.v_x = 0;
    }
    h.x += dx;

    const double ax = (h.v_x < 0) ? -h.v_x : h.v_x;
    if (ax > kVMax) {
        h.v_x = (h.v_x / ax) * kVMax;
    }
}

}  // namespace maoliao
