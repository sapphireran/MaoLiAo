#pragma once

// Tiny playfield used by 06_side_scroller_sim.
// Same integration order as main.cpp: command → hero → camera → flags.

#include "collision_portable.h"
#include "command_portable.h"
#include "define_portable.h"
#include "map_portable.h"

#include <string>
#include <vector>

namespace maoliao {

struct Coin {
    int gx = 0;
    int gy = 0;
    bool live = true;
};

struct SimHero {
    double xx = kX0;
    double yy = kY0;
    int x = kX0;
    int y = kY0;
    double x0 = 0.0;
    double vX = 0.0;
    double vY = 0.0;
    bool isFly = true;
    bool died = false;
    bool ending = false;
    bool passed = false;
    int score = 0;
};

struct SimWorld {
    int id = 1;
    std::vector<MapCell> tiles;
    std::vector<Coin> coins;
    std::vector<Enemy> enemies;
    int endingTiles = 20;
};

struct Sim {
    SimHero hero;
    SimWorld world;
};

struct DemoOptions {
    bool pit = true;
    bool enemy = true;
    bool coins = true;
};

SimWorld makeDemoWorld(DemoOptions opt = {});

int heroWorldX(const SimHero& h);

void stepSim(Sim& sim, int command);

std::string snapshotLine(const Sim& sim);

}  // namespace maoliao
