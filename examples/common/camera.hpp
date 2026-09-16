#pragma once

namespace mla {

struct Camera {
    double x;
    double x0;
    double vX;
    double xBg;
    Camera() : x(64), x0(0), vX(0), xBg(0) {}
};

// Apply the Role camera-window rules after a horizontal step.
// Returns pixels of world travel this call (for assertions).
void applyCameraWindow(Camera& cam, bool ending);

// Scene::action parallax: sky moves only when glued to XRIGHT, vX>0,
// and x0 actually changed. bgStep = |vX| * dt * px_per_m / K_MAP_BG.
void applyParallax(Camera& cam, double prevX0, double dt);

// World-space X of the sprite's left edge.
inline double worldX(const Camera& cam) { return cam.x - cam.x0; }

}  // namespace mla
