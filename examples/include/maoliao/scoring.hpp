#pragma once

#include "constants.hpp"

#include <string>

namespace maoliao {

constexpr int kCoinPoints = 10;
constexpr int kStompPoints = 5;
constexpr int kBulletEnemyPoints = 5;
constexpr int kFlowerPoints = 0;

constexpr int kDeathPoseMs = 3500;
constexpr int kVictoryWaitMs = 6500;
constexpr int kCreditsWaitMs = 7800;
constexpr int kPassedLevelWaitMs = 2000;
constexpr int kDiedBannerMs = 2000;

enum class LifePhase {
    Playing,
    DeathPose,
    LifeBanner,
    GameOver,
};

struct RunState {
    int score = 0;
    int life = kLifeCount;
    int world = 1;
    bool can_shoot = false;
    LifePhase phase = LifePhase::Playing;
};

inline void add_coin(RunState& run) { run.score += kCoinPoints; }
inline void add_stomp(RunState& run) { run.score += kStompPoints; }
inline void add_shot_enemy(RunState& run) { run.score += kBulletEnemyPoints; }

inline void eat_flower(RunState& run) {
    run.score += kFlowerPoints;
    run.can_shoot = true;
}

// main.cpp death branch: decrement first, then Game Over or banner.
inline LifePhase apply_death(RunState& run) {
    run.phase = LifePhase::DeathPose;
    run.life -= 1;
    if (run.life == 0) {
        run.phase = LifePhase::GameOver;
        run.life = kLifeCount;
        run.world = 1;
        run.can_shoot = false;
        return run.phase;
    }
    run.phase = LifePhase::LifeBanner;
    return run.phase;
}

inline void apply_clear(RunState& run) {
    if (run.world == 3) {
        run.life = kLifeCount;
        run.world = 1;
        run.can_shoot = false;
        return;
    }
    run.world += 1;
    run.can_shoot = false;
}

inline void apply_home(RunState& run) {
    run.life = kLifeCount;
    run.world = 1;
    run.can_shoot = false;
    run.phase = LifePhase::Playing;
}

inline std::string phase_name(LifePhase phase) {
    switch (phase) {
    case LifePhase::Playing:
        return "playing";
    case LifePhase::DeathPose:
        return "death-pose";
    case LifePhase::LifeBanner:
        return "life-banner";
    case LifePhase::GameOver:
        return "game-over";
    }
    return "unknown";
}

} // namespace maoliao
