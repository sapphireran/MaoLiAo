#include "maoliao.hpp"

#include <iostream>

int main() {
    using namespace maoliao;

    RunState run;
    for (int i = 0; i < 7; ++i) {
        add_coin(run);
    }
    add_stomp(run);
    add_stomp(run);
    add_shot_enemy(run);
    eat_flower(run);

    std::cout << "Scripted pickup: 7 coins, 2 stomps, 1 shot, 1 flower\n";
    std::cout << "  score=" << run.score << " (expect 85) can_shoot="
              << run.can_shoot << '\n';
    std::cout << "  world1 coins only=" << kWorld1CoinTotal
              << " stomps=" << perfect_stomp_score(1) << " perfect="
              << (kWorld1CoinTotal + perfect_stomp_score(1)) << '\n';

    RunState lives;
    for (int i = 0; i < 4; ++i) {
        apply_death(lives);
        std::cout << "  death " << (i + 1) << " life=" << lives.life
                  << " phase=" << phase_name(lives.phase) << '\n';
    }
    apply_death(lives);
    std::cout << "  death 5 phase=" << phase_name(lives.phase)
              << " reset life=" << lives.life << " world=" << lives.world << '\n';
    return run.score == 85 ? 0 : 1;
}
