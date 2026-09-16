#pragma once

#include "constants.hpp"

namespace maoliao {

// Faithful copy of Inertia::move: constant-acceleration step that mutates v.
inline double integrate(double& velocity, double dt, double acceleration) {
    const double displacement =
        velocity * dt + 0.5 * acceleration * dt * dt;
    velocity = velocity + acceleration * dt;
    return displacement;
}

inline double integrate_tick(double& velocity, double acceleration) {
    return integrate(velocity, kTick, acceleration);
}

// Real-space ballistic height with downward-positive gravity and upward-negative v0.
inline double real_height(double v0, double t, double gravity = kGravity) {
    return v0 * t + 0.5 * gravity * t * t;
}

inline double pixel_height(double v0, double t, double gravity = kGravity) {
    return real_height(v0, t, gravity) * kMetersToPixels;
}

inline double apex_time(double v0, double gravity = kGravity) {
    return -v0 / gravity;
}

struct JumpSample {
    int tick;
    double time;
    double vy;
    double real_y;
    double pixel_y;
};

// Integrate a jump the same way Role::action does: mutate vy with +G each tick
// and accumulate pixel y with the UNREAL/REAL scale.
inline JumpSample sample_jump_tick(int tick, double& vy, double& pixel_y) {
    const double dy = integrate_tick(vy, kGravity);
    pixel_y += dy * kMetersToPixels;
    JumpSample s{};
    s.tick = tick;
    s.time = tick * kTick;
    s.vy = vy;
    s.real_y = pixel_y / kMetersToPixels;
    s.pixel_y = pixel_y;
    return s;
}

} // namespace maoliao
