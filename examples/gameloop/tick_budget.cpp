#include <iostream>

#include "check.hpp"
#include "maoliao_const.hpp"

using namespace maoliao;
using maoliao::test::check;
using maoliao::test::checkNear;

enum class Phase { Play, DiedPose, LifeCard, GameOver, BetweenWorld, AllClear };

struct Loop {
    int life = kLife;
    int world = 1;
    int score = 0;
    bool died = false;
    bool passed = false;
    Phase phase = Phase::Play;
};

static void collectCoin(Loop& L) { L.score += kCoinScore; }
static void stomp(Loop& L) { L.score += kEnemyScore; }

static void onDeath(Loop& L) {
    L.died = true;
    L.life -= 1;
    if (L.life == 0) {
        L.phase = Phase::GameOver;
        L.life = kLife;
        L.world = 1;
        L.score = 0;
        L.died = false;
    } else {
        L.phase = Phase::LifeCard;
        L.died = false;
    }
}

static void onClear(Loop& L) {
    if (L.world == 3) {
        L.phase = Phase::AllClear;
        L.life = kLife;
        L.world = 1;
        L.score = 0;
    } else {
        L.world += 1;
        L.phase = Phase::BetweenWorld;
    }
    L.passed = false;
}

static void testTimingConstants() {
    checkNear(kTime, 0.01, 1e-12, "TIME");
    check(static_cast<int>(kTime * 1000) == 10, "Timer::Sleep 10 ms");
    check(3500 / 10 == 350, "death pose is 350 ticks");
    check(2000 / 10 == 200, "life card 200 ticks");
    check(6500 / 10 == 650, "clear fanfare 650 ticks");
}

static void testLifeCycle() {
    Loop L;
    check(L.life == 5 && L.world == 1, "fresh run");
    for (int i = 0; i < 4; ++i) {
        onDeath(L);
        check(L.phase == Phase::LifeCard, "still has lives");
    }
    check(L.life == 1, "one life left");
    onDeath(L);
    check(L.phase == Phase::GameOver, "fifth death");
    check(L.life == 5 && L.world == 1, "game over resets");
}

static void testWorldAdvance() {
    Loop L;
    onClear(L);
    check(L.world == 2 && L.phase == Phase::BetweenWorld, "1→2");
    onClear(L);
    check(L.world == 3, "2→3");
    onClear(L);
    check(L.world == 1 && L.phase == Phase::AllClear, "3→credits→1");
}

static void testHomeReset() {
    Loop L;
    L.world = 3;
    L.life = 2;
    L.score = 90;
    // VIR_HOME path in main.
    L.life = kLife;
    L.world = 1;
    L.score = 0;
    check(L.life == 5 && L.world == 1 && L.score == 0, "home wipes progress");
}

static void testScoreMix() {
    Loop L;
    for (int i = 0; i < 3; ++i) {
        collectCoin(L);
    }
    stomp(L);
    stomp(L);
    check(L.score == 40, "3 coins + 2 stomps");
}

static void testRestartKeepsWorld() {
    Loop L;
    L.world = 2;
    L.score = 70;
    // VIR_RESTART rebuilds Role/Scene but main does not reset world or life.
    L.score = 0;
    check(L.world == 2, "restart stays on world 2");
    check(L.life == 5, "restart does not spend a life");
}

int main() {
    std::cout << "gameloop / tick_budget\n";
    testTimingConstants();
    testLifeCycle();
    testWorldAdvance();
    testHomeReset();
    testScoreMix();
    testRestartKeepsWorld();
    return test::summary("tick_budget");
}
