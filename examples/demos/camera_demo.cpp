#include "maoliao/camera.h"
#include "maoliao/constants.h"
#include "maoliao/hero.h"

#include <iomanip>
#include <iostream>

int main() {
    using namespace maoliao;

    HeroState h;
    h.x = 64;
    h.y = 200;
    h.airborne = false;
    h.world = 1;

    CameraState cam;
    cam.screen_x = h.x;
    cam.origin_x = 0;
    cam.bg_x = 0;

    HeroInput in;
    in.right = true;

    std::cout << "Walk right until the XRIGHT=" << kXRight << " camera lock\n";
    std::cout << "step  screen_x  origin_x    bg_x     vX\n";

    for (int i = 1; i <= 80; ++i) {
        const double prev_origin = cam.origin_x;
        step_hero_free(h, in, friction_u(kT2));
        cam.screen_x = h.x;
        lock_camera(cam, h.ending);
        h.x = cam.screen_x;
        scroll_background(cam, h.v_x, prev_origin);
        if (i <= 5 || i % 10 == 0 || cam.screen_x == kXRight) {
            std::cout << std::setw(4) << i << ' ' << std::fixed << std::setprecision(2)
                      << std::setw(10) << cam.screen_x << ' ' << std::setw(10)
                      << cam.origin_x << ' ' << std::setw(8) << cam.bg_x << ' '
                      << std::setw(8) << h.v_x << '\n';
        }
        if (cam.origin_x < -40) {
            break;
        }
    }

    std::cout << "\nWorld X of hero = screen - origin = "
              << (cam.screen_x - cam.origin_x) << "\n";
    std::cout << "Parallax ratio vs world is 1/" << kMapBg << "\n";
    return 0;
}
