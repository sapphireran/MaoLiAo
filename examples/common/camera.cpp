#include "camera.hpp"

#include "constants.hpp"

#include <cmath>

namespace mla {

void applyCameraWindow(Camera& cam, bool ending) {
    if (cam.x < kXLeft) {
        cam.x = kXLeft;
        cam.vX = 0;
        return;
    }
    if (cam.x > kXRight && !ending) {
        cam.x0 -= (cam.x - kXRight);
        cam.x = kXRight;
    }
}

void applyParallax(Camera& cam, double prevX0, double dt) {
    if (!(cam.x == kXRight && cam.vX > 0 && prevX0 != cam.x0)) {
        return;
    }
    const double xabs = std::fabs(cam.vX);
    const double bgStep = xabs * dt * pxPerMetre() / kMapBg;
    cam.xBg -= bgStep;
}

}  // namespace mla
