// Miniature main.cpp: command → Role-like step → camera → coin / stomp / pit / ending.
// Prints a short log so `make test` is readable, then asserts the story beats.

#include "check.h"
#include "hero_sim.h"
#include "save_portable.h"

#include <iostream>
#include <vector>

using namespace maoliao;
using maoliao::test::expect;
using maoliao::test::expectNear;

namespace {

void hold(Sim& sim, int command, int frames) {
    for (int i = 0; i < frames; ++i) {
        stepSim(sim, command);
    }
}

void waitLand(Sim& sim, int maxFrames = 200) {
    for (int i = 0; i < maxFrames && sim.hero.isFly && !sim.hero.died; ++i) {
        stepSim(sim, 0);
    }
}

}  // namespace

int main() {
    std::cout << "06_side_scroller_sim — demo world story beats\n";

    // --- spawn and fall onto grass ---------------------------------------
    Sim sim;
    sim.world = makeDemoWorld();
    sim.hero.yy = 0;
    sim.hero.y = 0;
    sim.hero.xx = kX0;
    sim.hero.x = kX0;
    waitLand(sim);
    std::cout << "  landed  " << snapshotLine(sim) << "\n";
    expect(!sim.hero.isFly, "gravity finds the y=9 floor");
    expect(sim.hero.y == 8 * kHeight, "stand on top of the grass row (y=256)");

    // --- run right: camera should park the sprite at XRIGHT and scroll x0
    hold(sim, kCmdRight, 180);
    std::cout << "  ran     " << snapshotLine(sim) << "\n";
    expect(sim.hero.x == kXRight, "sprite sticks to the 192 px rail");
    expect(sim.hero.x0 < 0.0, "camera origin went negative");
    expect(heroWorldX(sim.hero) > kXRight, "world X kept growing");

    // --- first coin at grid (6, 8) is behind us after that sprint
    bool coin6 = false;
    for (const auto& c : sim.world.coins) {
        if (c.gx == 6) {
            coin6 = !c.live;
        }
    }
    expect(coin6, "sprinted through the first coin (+10)");
    expect(sim.hero.score >= 10, "score includes the coin");

    // --- fresh sim: stomp the enemy at (8, 8) -----------------------------
    Sim stomp;
    stomp.world = makeDemoWorld();
    stomp.hero.yy = 0;
    stomp.hero.y = 0;
    waitLand(stomp);
    // Walk until we are short of the enemy, then jump.
    while (heroWorldX(stomp.hero) < 7 * kWidth && !stomp.hero.died) {
        stepSim(stomp, kCmdRight);
    }
    stepSim(stomp, kCmdRight | kCmdUp);
    for (int i = 0; i < 80 && !stomp.hero.died; ++i) {
        stepSim(stomp, kCmdRight);
        if (stomp.world.enemies[0].turn == 0) {
            break;
        }
    }
    std::cout << "  stomp   " << snapshotLine(stomp) << "\n";
    expect(stomp.world.enemies[0].turn == 0, "enemy slot cleared");
    expect(stomp.hero.score % 5 == 0 && stomp.hero.score >= 5, "stomp awards +5");
    expect(!stomp.hero.died, "stomper lives");

    // --- side bump while grounded is lethal ------------------------------
    Sim bump;
    bump.world = makeDemoWorld();
    bump.hero.yy = 8 * kHeight;
    bump.hero.y = 8 * kHeight;
    bump.hero.isFly = false;
    bump.hero.xx = 8 * kWidth;
    bump.hero.x = 8 * kWidth;
    bump.hero.vY = 0.0;
    stepSim(bump, 0);
    std::cout << "  bump    " << snapshotLine(bump) << "\n";
    expect(bump.hero.died, "vY <= 0 + overlap → death");

    // --- walk into the pit at tile 12 ------------------------------------
    Sim pit;
    pit.world = makeDemoWorld();
    pit.hero.yy = 0;
    pit.hero.y = 0;
    waitLand(pit);
    hold(pit, kCmdRight, 400);
    std::cout << "  pit     " << snapshotLine(pit) << "\n";
    expect(pit.hero.died, "gap at x=12 drops y past YSIZE");

    // --- skip the pit with a jump and reach the ending rail --------------
    Sim clear;
    clear.world = makeDemoWorld();
    clear.hero.yy = 0;
    clear.hero.y = 0;
    waitLand(clear);
    // Run, jump over the gap around world X = 12*32 = 384.
    for (int i = 0; i < 600 && !clear.hero.passed && !clear.hero.died; ++i) {
        int cmd = kCmdRight;
        const int wx = heroWorldX(clear.hero);
        if (!clear.hero.isFly && wx > 10 * kWidth && wx < 13 * kWidth) {
            cmd |= kCmdUp;
        }
        stepSim(clear, cmd);
    }
    std::cout << "  clear   " << snapshotLine(clear) << "\n";
    expect(!clear.hero.died, "jumped the pit");
    expect(clear.hero.ending, "past 20 tiles sets ending");
    expect(clear.hero.passed, "auto-run walks off XSIZE");
    expect(clear.hero.score >= 20, "both coins if the line was clean");

    // --- save the world we just cleared (demo id 1) ----------------------
    const char* savePath = "build/sim_clear.dat";
    expect(writeWorld(savePath, clear.world.id) == SaveStatus::Ok, "write after clear");
    const SaveResult loaded = readWorld(savePath);
    expect(loaded.status == SaveStatus::Ok && loaded.world == 1, "read back world 1");

    // Camera math: after parking at XRIGHT, worldX = -x0 + 192.
    expectNear(static_cast<double>(heroWorldX(sim.hero)),
               -sim.hero.x0 + kXRight, 1.0, "worldX = -x0 + XRIGHT");

    return test::summary("06_side_scroller_sim");
}
