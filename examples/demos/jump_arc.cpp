#include "maoliao/constants.h"
#include "maoliao/hero.h"

#include <cmath>
#include <iomanip>
#include <iostream>

int main() {
    using namespace maoliao;

    HeroState h;
    h.x = 64;
    h.y = 200;
    h.airborne = false;
    h.world = 1;

    HeroInput in;
    in.jump = true;

    const double start_y = h.y;
    step_hero_free(h, in, friction_u(kT2));
    in.jump = false;

    double min_y = h.y;
    int apex_step = 1;
    std::cout << "Jump arc (world 1, no ceiling)\n";
    std::cout << "v0=" << jump_speed() << "  expected apex ~ "
              << start_y - kUnrealHeight << " px\n\n";
    std::cout << "step     y        vY\n";

    int step = 1;
    std::cout << std::setw(4) << step << ' ' << std::fixed << std::setprecision(3)
              << std::setw(10) << h.y << ' ' << std::setw(10) << h.v_y << '\n';

    while (h.y < start_y && step < 200) {
        step_hero_free(h, in, friction_u(kT2));
        ++step;
        if (h.y < min_y) {
            min_y = h.y;
            apex_step = step;
        }
        if (step <= 8 || step % 10 == 0 || h.y >= start_y) {
            std::cout << std::setw(4) << step << ' ' << std::setw(10) << h.y << ' '
                      << std::setw(10) << h.v_y << '\n';
        }
    }

    const double rise = start_y - min_y;
    std::cout << "\napex step=" << apex_step << " y=" << min_y
              << " rise_px=" << rise
              << " (UNREAL_HEIGHT=" << kUnrealHeight << ")\n";
    std::cout << "air time to land ~ " << step * kTime << " s\n";
    return 0;
}
