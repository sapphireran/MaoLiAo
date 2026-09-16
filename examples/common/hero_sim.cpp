#include "hero_sim.h"

#include "inertia_portable.h"

#include <cmath>
#include <sstream>

namespace maoliao {

int heroWorldX(const SimHero& h) { return -static_cast<int>(h.x0) + h.x; }

SimWorld makeDemoWorld() {
    SimWorld w;
    w.id = 1;
    w.endingTiles = 20;
    // 24 tiles of grass at y = 9, a one-tile gap at x = 12 (pit), then more grass.
    w.tiles.push_back({0, 9, 1, 12, 1, 0.0});
    w.tiles.push_back({13, 9, 1, 16, 1, 0.0});
    assignFriction(w.tiles.data(), static_cast<int>(w.tiles.size()), 1);
    w.coins.push_back({6, 8, true});
    w.coins.push_back({16, 8, true});
    w.enemies.push_back({8 * kWidth, 8 * kHeight, 1});
    return w;
}

namespace {

const MapCell* groundUnder(const Sim& sim, int worldX, int y) {
    return hitMap(worldX, y, sim.world.tiles.data(), sim.world.tiles.size(),
                  sim.world.id, false, nullptr);
}

void collectCoins(Sim& sim) {
    const int wx = heroWorldX(sim.hero);
    const auto corners = spriteCorners(wx, sim.hero.y);
    for (auto& c : sim.world.coins) {
        if (!c.live) {
            continue;
        }
        const Rect box{c.gx * kWidth, c.gy * kHeight, c.gx * kWidth + kWidth,
                       c.gy * kHeight + kHeight};
        if (cornersHit(corners, box)) {
            c.live = false;
            sim.hero.score += 10;
        }
    }
}

void resolveEnemy(Sim& sim) {
    const int wx = heroWorldX(sim.hero);
    const EnemyContact kind = classifyEnemyContact(
        wx, sim.hero.y, sim.hero.vY, sim.world.enemies.data(), sim.world.enemies.size());
    if (kind == EnemyContact::Stomp) {
        for (auto& e : sim.world.enemies) {
            if (e.turn == 0) {
                continue;
            }
            if (hitEnemy(wx, sim.hero.y, &e, 1) != nullptr) {
                e.turn = 0;
                e.x = 0;
                e.y = 0;
                sim.hero.score += 5;
            }
        }
    } else if (kind == EnemyContact::Lethal) {
        sim.hero.died = true;
    }
}

}  // namespace

void stepSim(Sim& sim, int command) {
    SimHero& h = sim.hero;
    if (h.died || h.passed) {
        return;
    }

    double a = 0.0;
    if ((command & kCmdUp) && !h.isFly && !h.ending) {
        h.isFly = true;
        h.vY = jumpLaunchSpeed();
    }

    if (h.isFly) {
        h.yy += inertiaMove(h.vY, kTime, kG) * kPixelScale;
        h.y = static_cast<int>(h.yy);
        const MapCell* floor = groundUnder(sim, heroWorldX(h), h.y + 1);
        if (floor != nullptr) {
            if (h.vY > 0.0) {
                h.isFly = false;
            }
            h.vY = 0.0;
            h.y = (h.y + kHeight / 2) / kHeight * kHeight;
            h.yy = h.y;
        }
        if (h.vY > 0.0 && h.y > kYSize) {
            h.died = true;
            return;
        }
    } else if (groundUnder(sim, heroWorldX(h), h.y + 1) == nullptr) {
        h.isFly = true;
    }

    if ((command & kCmdLeft) && !h.ending) {
        a -= kARole;
    }
    if ((command & kCmdRight) || h.ending) {
        a += kARole;
    }

    const MapCell* stand = groundUnder(sim, heroWorldX(h), h.y + 1);
    double a1 = 0.0;
    if (h.vX * a <= 0.0 && h.vX != 0.0 && stand != nullptr) {
        const double k = (h.vX < 0.0) ? 2.0 : -2.0;
        a1 = k * kG * stand->u;
    }

    const double before = h.vX;
    const double metres = inertiaMove(h.vX, kTime, a + a1);
    if (before * h.vX < 0.0) {
        h.vX = 0.0;
    }
    if (std::fabs(h.vX) > kVMax) {
        h.vX = (h.vX > 0.0 ? kVMax : -kVMax);
    }

    h.xx += metres * kPixelScale;
    h.x = static_cast<int>(h.xx);

    if (heroWorldX(h) > sim.world.endingTiles * kWidth) {
        h.ending = true;
    }
    if (h.x > kXSize) {
        h.passed = true;
    }

    if (h.x < kXLeft) {
        h.x = kXLeft;
        h.xx = h.x;
        h.vX = 0.0;
    } else if (h.x > kXRight && !h.ending) {
        h.x0 -= (h.x - kXRight);
        h.x = kXRight;
        h.xx = h.x;
    }

    collectCoins(sim);
    resolveEnemy(sim);
}

std::string snapshotLine(const Sim& sim) {
    std::ostringstream o;
    o << "wx=" << heroWorldX(sim.hero) << " x=" << sim.hero.x << " y=" << sim.hero.y
      << " x0=" << static_cast<int>(sim.hero.x0) << " vX=" << sim.hero.vX
      << " fly=" << sim.hero.isFly << " score=" << sim.hero.score
      << " end=" << sim.hero.ending << " pass=" << sim.hero.passed
      << " dead=" << sim.hero.died;
    return o.str();
}

}  // namespace maoliao
