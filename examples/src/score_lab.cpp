#include <iostream>

#include "maoliao/check.hpp"
#include "maoliao/combat.hpp"
#include "maoliao/worlds.hpp"

int main() {
    using namespace maoliao;

    int score = 0;
    for (std::size_t i = 0; i < world1_coins().size(); ++i) {
        score = add_coin(score);
    }
    MLA_CHECK(score == 20 * kScoreCoin);

    for (std::size_t i = 0; i < world1_enemies().size(); ++i) {
        score = add_stomp(score);
    }
    MLA_CHECK(score == 20 * 10 + 10 * 5);

    score = add_shot(score);
    MLA_CHECK(score == 255);
    std::cout << "world1 all-coin + all-stomp + 1 shot  " << score << "\n";

    LifeMachine m;
    MLA_CHECK(m.life == 5 && m.world == 1);
    on_death(m);
    MLA_CHECK(m.life == 4 && !m.game_over);
    m.life = 1;
    on_death(m);
    MLA_CHECK(m.game_over);
    MLA_CHECK(m.life == 5 && m.world == 1 && m.at_home);

    LifeMachine c;
    c.world = 2;
    on_clear(c);
    MLA_CHECK(c.world == 3 && !c.at_home);
    on_clear(c);
    MLA_CHECK(c.world == 1 && c.at_home && c.life == 5);

    LifeMachine h;
    h.world = 3;
    h.life = 2;
    on_home(h);
    MLA_CHECK(h.world == 1 && h.life == 5 && h.at_home);

    return done("score_lab");
}
