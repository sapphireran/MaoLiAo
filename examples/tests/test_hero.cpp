#include "check.hpp"
#include "maoliao.hpp"

int main() {
    using namespace maoliao;

    auto floor = flat_floor();
    HeroState hero;
    hero.fly = false;
    hero.y = 256;
    hero.yy = 256;

    const auto jump = step_hero(hero, command_mask({Key::W}), floor);
    CHECK(jump.jumped);
    CHECK(hero.fly);
    check_near(hero.vy, jump_launch_speed() + kGravity * kTick, 1e-9, "vy after 1");

    double min_y = hero.yy;
    bool landed = false;
    for (int i = 0; i < 120; ++i) {
        const auto info = step_hero(hero, 0, floor);
        if (hero.yy < min_y) {
            min_y = hero.yy;
        }
        landed = landed || info.landed;
    }
    CHECK(landed);
    CHECK(!hero.fly);
    CHECK(hero.y == 256);
    CHECK(min_y < 256.0 - 90.0);

    HeroState walker;
    walker.fly = false;
    walker.y = 256;
    walker.yy = 256;
    const int right = command_mask({Key::D});
    for (int i = 0; i < 80; ++i) {
        step_hero(walker, right, floor);
    }
    CHECK(walker.x > kSpawnX);
    CHECK(walker.vx > 0.0);

    HeroState flappy;
    flappy.world = 3;
    flappy.fly = true;
    flappy.y = 200;
    flappy.yy = 200;
    const auto flap = step_hero(flappy, command_mask({Key::W}), {});
    CHECK(flap.jumped);
    CHECK(flappy.fly);

    HeroState pit;
    pit.fly = true;
    pit.y = 400;
    pit.yy = 400;
    pit.vy = 1.0;
    step_hero(pit, 0, {});
    CHECK(pit.died);
    return report("test_hero");
}
