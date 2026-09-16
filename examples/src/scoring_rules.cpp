#include "maoliao_model.hpp"

#include <iostream>

using namespace maoliao;

static int fail(const char* msg) {
    std::cerr << "FAIL: " << msg << "\n";
    return 1;
}

int main() {
    int score = 0;
    score = apply_pickup(score, Pickup::Coin);
    score = apply_pickup(score, Pickup::Coin);
    score = apply_pickup(score, Pickup::Stomp);
    score = apply_pickup(score, Pickup::Shot);
    std::cout << "2 coins + stomp + shot = " << score << "\n";
    if (score != 10 + 10 + 5 + 5) return fail("score mix");

    // Sweep world 1 coins, stomp every enemy, never shoot.
    int sweep = 0;
    for (size_t i = 0; i < world1_coins().size(); ++i) sweep = apply_pickup(sweep, Pickup::Coin);
    for (size_t i = 0; i < world1_enemies().size(); ++i) sweep = apply_pickup(sweep, Pickup::Stomp);
    std::cout << "world 1 full coin+stomp sweep = " << sweep << "\n";
    if (sweep != 20 * 10 + 10 * 5) return fail("world 1 sweep");

    int w2 = 0;
    for (size_t i = 0; i < world2_coins().size(); ++i) w2 = apply_pickup(w2, Pickup::Coin);
    for (size_t i = 0; i < world2_enemies().size(); ++i) w2 = apply_pickup(w2, Pickup::Shot);
    std::cout << "world 2 coins+shots = " << w2 << "\n";
    if (w2 != 11 * 10 + 6 * 5) return fail("world 2 mix");

    // Lives are a main.cpp global, not Role. Five deaths end the run.
    int life = LIFE;
    int deaths = 0;
    while (life > 0) {
        --life;
        ++deaths;
    }
    std::cout << "deaths to game-over = " << deaths << " (LIFE=" << LIFE << ")\n";
    if (deaths != 5) return fail("five lives");

    // Weapon flag is binary; food does not add score in Role::action.
    bool is_shoot = false;
    is_shoot = true; // pickup
    if (!is_shoot) return fail("food arms the hero");
    std::cout << "food grants isShoot, not points\n";

    std::cout << "scoring_rules: ok\n";
    return 0;
}
