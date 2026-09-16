#pragma once

#include "maoliao/constants.hpp"

namespace maoliao {

inline int add_coin(int score) { return score + kScoreCoin; }
inline int add_stomp(int score) { return score + kScoreEnemy; }
inline int add_shot(int score) { return score + kScoreEnemy; }

struct Bullet {
    int x = 0;
    int y = 0;
    int turn = 0;
};

// Advance one bullet. Returns true if the shot should explode
// (enemy / wall tests are left to the caller).
inline bool bullet_step(Bullet& b) {
    if (b.x == 0 && b.y == 0) {
        return false;
    }
    if (b.x > kXSize || b.x < -kWidth) {
        b = {};
        return false;
    }
    if (b.x > kMaxBulletDistance) {
        return true;
    }
    b.x += kBulletStep * b.turn;
    return false;
}

struct LifeMachine {
    int life = kLife;
    int world = 1;
    bool at_home = false;
    bool game_over = false;
};

inline void on_death(LifeMachine& m) {
    --m.life;
    if (m.life == 0) {
        m.game_over = true;
        m.life = kLife;
        m.world = 1;
        m.at_home = true;
    }
}

inline void on_clear(LifeMachine& m) {
    if (m.world == 3) {
        m.life = kLife;
        m.world = 1;
        m.at_home = true;
    } else {
        ++m.world;
    }
}

inline void on_home(LifeMachine& m) {
    m.life = kLife;
    m.world = 1;
    m.at_home = true;
    m.game_over = false;
}

}  // namespace maoliao
