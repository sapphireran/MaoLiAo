#include "portable/maoliao_math.hpp"

#include <iostream>

// Finite-state walk of the transitions in main.cpp:
//   died  -> life--, rebuild world, or game-over -> title + world 1
//   passed -> world++ or all-clear -> title + world 1
// Lives and world are the only globals the loop mutates.

namespace {

int gFailures = 0;

void expectEq(const char* name, int got, int want) {
    if (got != want) {
        std::cerr << "FAIL " << name << " got " << got << " want " << want << "\n";
        ++gFailures;
    } else {
        std::cout << "PASS " << name << " = " << got << "\n";
    }
}

void expect(const char* name, bool ok) {
    if (!ok) {
        std::cerr << "FAIL " << name << "\n";
        ++gFailures;
    } else {
        std::cout << "PASS " << name << "\n";
    }
}

enum class Screen { Play, Title, GameOver, AllClear };

struct Run {
    int life = mla::kLife;
    int world = 1;
    Screen screen = Screen::Play;
    int rebuilds = 0;

    void die() {
        --life;
        if (life == 0) {
            screen = Screen::GameOver;
            life = mla::kLife;
            world = 1;
            screen = Screen::Title;
            ++rebuilds;
        } else {
            ++rebuilds; // same world
        }
    }

    void clear() {
        if (world == 3) {
            screen = Screen::AllClear;
            life = mla::kLife;
            world = 1;
            screen = Screen::Title;
            ++rebuilds;
        } else {
            ++world;
            ++rebuilds;
        }
    }

    void homeFromPause() {
        life = mla::kLife;
        world = 1;
        screen = Screen::Title;
        ++rebuilds;
    }

    void startFromTitle() { screen = Screen::Play; }
};

} // namespace

int main() {
    using namespace mla;

    expectEq("start_lives", kLife, 5);

    Run r;
    r.die();
    expectEq("after_first_death_life", r.life, 4);
    expectEq("still_world_1", r.world, 1);
    expect("still_playing", r.screen == Screen::Play);

    r.world = 2;
    r.die();
    expectEq("death_keeps_world", r.world, 2);
    expectEq("life_3", r.life, 3);

    r.die();
    r.die();
    r.die(); // life 3 -> 2 -> 1 -> 0
    expectEq("game_over_resets_life", r.life, 5);
    expectEq("game_over_resets_world", r.world, 1);
    expect("game_over_goes_title", r.screen == Screen::Title);

    r.startFromTitle();
    r.clear();
    expectEq("clear_w1_to_w2", r.world, 2);
    r.clear();
    expectEq("clear_w2_to_w3", r.world, 3);
    r.clear();
    expectEq("all_clear_resets_world", r.world, 1);
    expectEq("all_clear_resets_life", r.life, 5);
    expect("all_clear_goes_title", r.screen == Screen::Title);

    r.startFromTitle();
    r.world = 3;
    r.life = 2;
    r.homeFromPause();
    expectEq("pause_home_life", r.life, 5);
    expectEq("pause_home_world", r.world, 1);
    expect("pause_home_title", r.screen == Screen::Title);

    // Save file stores world only — loading does not restore life.
    r.life = 2;
    r.world = 3;
    const int savedWorld = r.world;
    r.life = kLife; // what a fresh process would have
    r.world = savedWorld;
    expectEq("load_keeps_default_lives", r.life, 5);
    expectEq("load_restores_world", r.world, 3);

    if (gFailures) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "life_cycle: all checks passed\n";
    return 0;
}
