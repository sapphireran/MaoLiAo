#include "check.hpp"
#include "maoliao.hpp"

int main() {
    using namespace maoliao;

    CHECK(sky_band_y(1) == 0);
    CHECK(sky_band_y(2) == -384);
    CHECK(sky_band_y(3) == -768);

    HeroState hero;
    hero.fly = false;
    hero.y = 256;
    hero.yy = 256;
    const auto floor = flat_floor();
    Camera cam;
    const int right = command_mask({Key::D});

    bool pinned = false;
    for (int i = 0; i < 500; ++i) {
        step_hero(hero, right, floor);
        follow_hero(cam, hero.x0, hero.vx, hero.x);
        if (hero.x == kRightLimit) {
            pinned = true;
        }
    }
    CHECK(pinned);
    CHECK(hero.x == kRightLimit);
    CHECK(hero.x0 < 0.0);
    CHECK(world_x(hero.x, hero.x0) > kRightLimit);
    CHECK(cam.x_bg <= 0.0);

    int x = -4;
    double xx = -4;
    double vx = -1;
    double x0 = 0;
    pin_hero(x, xx, vx, x0, false);
    CHECK(x == 0);
    CHECK(vx == 0.0);
    return report("test_camera");
}
