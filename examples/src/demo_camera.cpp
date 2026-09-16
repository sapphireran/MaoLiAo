#include "maoliao.hpp"

#include <iomanip>
#include <iostream>

int main() {
    using namespace maoliao;

    HeroState hero;
    hero.fly = false;
    hero.y = 256;
    hero.yy = 256;
    const auto floor = flat_floor();
    Camera cam;
    const int hold_right = command_mask({Key::D});

    std::cout << "Walk right from spawn; camera should pin x=192 and slide x0\n";
    std::cout << std::setw(6) << "tick" << std::setw(8) << "x" << std::setw(10)
              << "x0" << std::setw(10) << "worldX" << std::setw(10) << "xBg"
              << '\n';

    for (int i = 0; i < 400; ++i) {
        step_hero(hero, hold_right, floor);
        follow_hero(cam, hero.x0, hero.vx, hero.x);
        if (i < 5 || i == 80 || i == 120 || i == 399) {
            std::cout << std::setw(6) << i << std::setw(8) << hero.x
                      << std::setw(10) << std::fixed << std::setprecision(2)
                      << hero.x0 << std::setw(10)
                      << world_x(hero.x, hero.x0) << std::setw(10) << cam.x_bg
                      << '\n';
        }
    }
    std::cout << "sky band world2 y=" << sky_band_y(2) << " world3 y="
              << sky_band_y(3) << '\n';
    return 0;
}
