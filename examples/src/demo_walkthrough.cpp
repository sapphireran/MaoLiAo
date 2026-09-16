#include "maoliao.hpp"

#include <iomanip>
#include <iostream>

int main() {
    using namespace maoliao;

    auto floor = flat_floor(120, 9, 1, 1);
    HeroState hero;
    hero.fly = false;
    hero.y = 256;
    hero.yy = 256;

    std::cout << "200 ticks of D, then a jump, then coast\n";
    const int right = command_mask({Key::D});
    const int right_up = command_mask({Key::D, Key::W});

    int jumped_at = -1;
    double min_y = hero.yy;
    for (int i = 0; i < 320; ++i) {
        const int key = (i == 200) ? right_up : right;
        const auto info = step_hero(hero, key, floor);
        if (info.jumped) {
            jumped_at = i;
        }
        if (hero.yy < min_y) {
            min_y = hero.yy;
        }
        if (i == 0 || i == 50 || i == 199 || i == 200 || i == 248 || i == 319) {
            std::cout << std::setw(6) << i << " x=" << hero.x << " y=" << hero.y
                      << " vx=" << std::fixed << std::setprecision(3) << hero.vx
                      << " fly=" << hero.fly << " worldX="
                      << world_x(hero.x, hero.x0) << '\n';
        }
    }
    std::cout << "jumped at tick " << jumped_at << ", highest y " << min_y
              << " (smaller is higher on screen)\n";
    std::cout << "ending distance world1=" << ending_distance_px(1) << " px\n";
    return jumped_at == 200 ? 0 : 1;
}
