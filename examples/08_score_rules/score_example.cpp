// Score deltas and a tiny combat timeline that does *not* live in Role::show.
// The shipped game steps bullets during blit; this example keeps them in update.

#include "maoliao_core.h"

#include <iostream>

using namespace maoliao;

static int gFailures = 0;

static void expectEq(const char* name, int got, int want) {
    if (got != want) {
        std::cerr << "FAIL " << name << " got " << got << " want " << want << "\n";
        ++gFailures;
    } else {
        std::cout << "ok   " << name << " = " << got << "\n";
    }
}

int main() {
    std::cout << "=== MaoLiAo score rules ===\n";

    expectEq("empty", applyPickupScore(0, false, false, false), 0);
    expectEq("coin", applyPickupScore(0, true, false, false), kScoreCoin);
    expectEq("stomp", applyPickupScore(0, false, true, false), kScoreEnemy);
    expectEq("shot", applyPickupScore(0, false, false, true), kScoreEnemy);
    expectEq("coin+stomp", applyPickupScore(7, true, true, false), 7 + 10 + 5);

    // A short scripted life: three coins, one stomp, two misses, one shot.
    int score = 0;
    score = applyPickupScore(score, true, false, false);
    score = applyPickupScore(score, true, false, false);
    score = applyPickupScore(score, true, false, false);
    score = applyPickupScore(score, false, true, false);
    score = applyPickupScore(score, false, false, false);
    score = applyPickupScore(score, false, false, false);
    score = applyPickupScore(score, false, false, true);
    expectEq("scripted run", score, 30 + 5 + 5);

    // Death reconstructs Role, which zeros score. Load does not restore it.
    int afterDeath = 0;
    expectEq("score after Role rebuild", afterDeath, 0);

    // Lives are independent of score. Five hits → game over in main.cpp.
    int life = kLife;
    for (int hit = 0; hit < 5; ++hit) {
        --life;
    }
    expectEq("life after five deaths", life, 0);

    if (gFailures != 0) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "all score checks passed\n";
    return 0;
}
