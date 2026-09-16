#include <iostream>

#include "check.hpp"
#include "maoliao_const.hpp"
#include "sprites.hpp"

using namespace maoliao;
using maoliao::test::check;
using maoliao::test::checkNear;

static void testWalkParity() {
    // rolePos / STEP % 4 == 0 → frame 1; % 2 == 0 && % 4 != 0 → frame 2.
    check(heroIFrame(0) == 1, "pos 0");
    check(heroIFrame(10) == 2, "pos 10 (one STEP, even not multiple of 4)");
    check(heroIFrame(20) == 1, "pos 20");
    check(heroIFrame(30) == 2, "pos 30");
    check(heroIFrame(40) == 1, "pos 40");
    check(heroIFrame(15) == 1, "odd STEP cell");
}

static void testFacingColumns() {
    check(heroSheetColumn(1, 1) == 0, "right frame 1");
    check(heroSheetColumn(2, 1) == 1, "right frame 2");
    check(heroSheetColumn(1, -1) == 3, "left frame 1");
    check(heroSheetColumn(2, -1) == 4, "left frame 2");
}

static void testEnemyWrap() {
    double iframe = 1.0;
    int wraps = 0;
    for (int i = 0; i < 500; ++i) {
        const double next = wrapIFrame(iframe, kTime * 5.0, 3);
        if (next == 1.0 && iframe > 1.0) {
            ++wraps;
        }
        iframe = next;
    }
    check(wraps > 0, "enemy iframe wraps at 3");
    checkNear(kTime * 5.0, 0.05, 1e-12, "enemy step");
}

static void testCoinAndBomb() {
    double coin = 1.0;
    int coinWraps = 0;
    for (int i = 0; i < 200; ++i) {
        const double next = wrapIFrame(coin, kTime * 7.0, 5);
        if (next == 1.0 && coin > 1.0) {
            ++coinWraps;
        }
        coin = next;
    }
    check(coinWraps > 0, "coin wraps at 5");

    double bomb = 1.0;
    int frames = 0;
    while (static_cast<int>(bomb) != 5 && frames < 100) {
        bomb += kTime * 10.0;
        ++frames;
    }
    check(frames == 40, "bomb lasts 0.4 s (4 extra frames of TIME*10 from 1 to 5)");
}

static void testScenery() {
    checkNear(kSceneryF, 0.003, 1e-12, "F = TIME * 0.3");
    double s = 1.0;
    int steps = 0;
    while (s < 3.0 && steps < 10000) {
        s += kSceneryF;
        ++steps;
    }
    check(steps >= 660 && steps <= 670, "scenery loops about every 6.67 s");
}

static void testEnemyGate() {
    // (int)(iframe * 100) % 2 == 0
    check(enemyMovesThisFrame(1.00), "1.00 even");
    check(!enemyMovesThisFrame(1.01), "1.01 odd");
    check(enemyMovesThisFrame(1.02), "1.02 even");
}

static void testBulletBudget() {
    check(kBulletFlight == 4, "4 px per blit");
    check(kMaxBulletDistance == 480, "screen-space cap");
    const int maxSteps = kMaxBulletDistance / kBulletFlight;
    check(maxSteps == 120, "at most 120 flight steps");
    const int holdFrames = static_cast<int>(kBulletHoldGap / kTime);
    check(holdFrames == 20, "held J every 20 ticks");
}

int main() {
    std::cout << "sprites / walk_frames\n";
    testWalkParity();
    testFacingColumns();
    testEnemyWrap();
    testCoinAndBomb();
    testScenery();
    testEnemyGate();
    testBulletBudget();
    return test::summary("walk_frames");
}
