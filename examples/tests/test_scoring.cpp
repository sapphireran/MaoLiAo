#include "check.hpp"
#include "maoliao.hpp"

int main() {
    using namespace maoliao;

    CHECK(kWorld1Coins.size() == 20);
    CHECK(kWorld2Coins.size() == 11);
    CHECK(kWorld1CoinTotal == 200);
    CHECK(kWorld2CoinTotal == 110);
    CHECK(perfect_stomp_score(1) == 50);
    CHECK(perfect_stomp_score(2) == 30);
    CHECK(perfect_stomp_score(3) == 35);
    CHECK(ending_distance_px(1) == 94 * 32);
    CHECK(ending_distance_px(2) == 104 * 32);
    CHECK(is_ending(1, 94 * 32 + 1));
    CHECK(!is_ending(1, 94 * 32));
    CHECK(is_passed(513));

    RunState run;
    for (int i = 0; i < 7; ++i) {
        add_coin(run);
    }
    add_stomp(run);
    add_stomp(run);
    add_shot_enemy(run);
    eat_flower(run);
    CHECK(run.score == 85);
    CHECK(run.can_shoot);

    RunState lives;
    CHECK(lives.life == 5);
    for (int i = 0; i < 4; ++i) {
        CHECK(apply_death(lives) == LifePhase::LifeBanner);
    }
    CHECK(apply_death(lives) == LifePhase::GameOver);
    CHECK(lives.life == 5);
    CHECK(lives.world == 1);

    RunState clear;
    clear.world = 2;
    apply_clear(clear);
    CHECK(clear.world == 3);
    apply_clear(clear);
    CHECK(clear.world == 1);
    CHECK(clear.life == 5);
    return report("test_scoring");
}
