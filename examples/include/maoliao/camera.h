#pragma once

namespace maoliao {

struct CameraState {
    double screen_x = 0;
    double origin_x = 0;
    double bg_x = 0;
};

// Apply the XRIGHT lock from Role::action. ending disables the lock.
void lock_camera(CameraState& cam, bool ending);

// Scene::action background step when the hero is pressed against XRIGHT
// and moving right with a changing origin.
void scroll_background(CameraState& cam, double v_x, double prev_origin);

}  // namespace maoliao
