#pragma once

namespace maoliao {

struct HeroState {
    double x = 0;
    double y = 0;
    double v_x = 0;
    double v_y = 0;
    bool airborne = true;
    bool ending = false;
    int world = 1;
};

struct HeroInput {
    bool left = false;
    bool right = false;
    bool jump = false;
};

// One TIME-step of the jump / run model (no collision). Used by demos.
void step_hero_free(HeroState& h, const HeroInput& in, double ground_u);

}  // namespace maoliao
