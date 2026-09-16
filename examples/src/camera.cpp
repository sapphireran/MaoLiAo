#include "maoliao/camera.h"

#include "maoliao/constants.h"

#include <cmath>

namespace maoliao {

void lock_camera(CameraState& cam, bool ending) {
    if (cam.screen_x < kXLeft) {
        cam.screen_x = kXLeft;
        return;
    }
    if (cam.screen_x > kXRight && !ending) {
        cam.origin_x -= (cam.screen_x - kXRight);
        cam.screen_x = kXRight;
    }
}

void scroll_background(CameraState& cam, double v_x, double prev_origin) {
    const double xabs = std::fabs(v_x);
    const double bg_step = xabs * kTime * kPxPerMetre / kMapBg;
    const bool pressed_right = (cam.screen_x == kXRight && v_x > 0 &&
                                prev_origin != cam.origin_x);
    if (pressed_right) {
        cam.bg_x -= bg_step;
    }
}

}  // namespace maoliao
