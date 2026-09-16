#include <iostream>

#include "check.hpp"
#include "maoliao_const.hpp"
#include "sprites.hpp"

using namespace maoliao;
using maoliao::test::check;
using maoliao::test::checkNear;

static void testWalkParity() {
    // q = rolePos / 10. Frame 1 on q % 4 == 0; frame 2 on even q not multiple of 4.
    check(heroIFrame(0) == 1, "pos 0 → q=0 → frame 1");
    check(heroIFrame(10) == 0, "pos 10 → q=1 odd → keep");
    check(heroIFrame(20) == 2, "pos 20 → q=2 → frame 2");
    check(heroIFrame(30) == 0, "pos 30 → q=3 odd → keep");
    check(heroIFrame(40) == 1, "pos 40 → q=4 → frame 1");
    check(heroIFrame(15) == 0, "pos 15 still q=1");

    int frame = 1;
    frame = applyHeroIFrame(frame, 10);
    check(frame == 1, "odd cell keeps frame 1");
    frame = applyHeroIFrame(frame, 20);
    check(frame == 2, "even non-multiple-of-4 switches to 2");
    frame = applyHeroIFrame(frame, 30);
    check(frame == 2, "next odd cell keeps frame 2");
    frame = applyHeroIFrame(frame, 40);
    check(frame == 1, "multiple of 4 returns to frame 1");
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
    // TIME*10 is 0.1; 1 → 5 takes ~40 additions, ±1 for binary 0.01.
    check(frames >= 39 && frames <= 41, "bomb lasts about 0.4 s");
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
