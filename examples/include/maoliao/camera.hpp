#pragma once

#include "constants.hpp"

#include <algorithm>
#include <cmath>

namespace maoliao {

struct Camera {
    double x0 = 0.0;   // Hero::x0 world origin of the screen
    double x_bg = 0.0; // Scene::xBg
    int x_map = 0;     // Scene::xMap
};

inline int sky_band_y(int world) { return -(world - 1) * kScreenHeight; }

inline double background_step(double vx) {
    const double xabs = std::fabs(vx);
    return xabs * kTick * kMetersToPixels / static_cast<double>(kMapBgRatio);
}

// Role screen-edge rule: keep the sprite in [0, 192] and slide x0 when
// walking past the right pin, unless the ending auto-walk is on.
inline void pin_hero(int& x, double& xx, double& vx, double& x0, bool ending) {
    if (x < kLeftLimit) {
        x = kLeftLimit;
        xx = x;
        vx = 0.0;
        return;
    }
    if (x > kRightLimit && !ending) {
        x0 -= (x - kRightLimit);
        x = kRightLimit;
        xx = x;
    }
}

// Scene::action parallax: sky creeps only while the hero is pinned right
// and still has positive vX, and only when xMap actually changed.
inline void follow_hero(Camera& cam, double hero_x0, double vx, int screen_x) {
    const int old_map = cam.x_map;
    cam.x0 = hero_x0;
    cam.x_map = static_cast<int>(hero_x0);
    if (screen_x == kRightLimit && vx > 0.0 && old_map != cam.x_map) {
        cam.x_bg -= background_step(vx);
    }
    if (cam.x_bg <= -kScreenWidth) {
        cam.x_bg = 0.0;
    }
}

inline int world_x(int screen_x, double x0) {
    return -static_cast<int>(x0) + screen_x;
}

} // namespace maoliao
