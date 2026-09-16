#include "camera.hpp"
#include "check.hpp"
#include "constants.hpp"
#include "inertia.hpp"

#include <cstdio>

// Drive the hero right from spawn until the camera window starts scrolling,
// then keep going and watch parallax.

int main() {
    std::printf("=== 07 camera scroll ===\n");
    std::printf("window X in [%d, %d]  px/m=%.6f  K_MAP_BG=%d\n", mla::kXLeft,
                mla::kXRight, mla::pxPerMetre(), mla::kMapBg);

    mla::Camera cam;
    cam.x = mla::kX0;
    cam.x0 = 0;
    cam.vX = 0;
    cam.xBg = 0;

    int firstScroll = -1;
    double maxWorld = 0;
    int frames = 0;
    const int holdRight = 400;

    for (; frames < holdRight; ++frames) {
        const double prevX0 = cam.x0;
        const double dx = mla::integrate(cam.vX, mla::kTime, mla::kARole);
        if (cam.vX > mla::kVMax) {
            cam.vX = mla::kVMax;
        }
        cam.x += mla::toPixels(dx);
        mla::applyCameraWindow(cam, false);
        mla::applyParallax(cam, prevX0, mla::kTime);
        if (firstScroll < 0 && cam.x0 < 0) {
            firstScroll = frames;
        }
        const double wx = mla::worldX(cam);
        if (wx > maxWorld) {
            maxWorld = wx;
        }
        if (frames < 8 || frames % 40 == 0 || frames == firstScroll) {
            std::printf("f=%3d  x=%7.2f  x0=%8.2f  worldX=%8.2f  xBg=%7.3f  v=%.3f\n",
                        frames, cam.x, cam.x0, wx, cam.xBg, cam.vX);
        }
    }

    const bool glued = (cam.x == mla::kXRight);
    std::printf("after %d frames: glued=%d  x0=%.2f  worldX=%.2f  xBg=%.3f\n",
                frames, glued ? 1 : 0, cam.x0, mla::worldX(cam), cam.xBg);

    mla::Checks c;
    c.expect(firstScroll > 0, "camera starts scrolling after the 192px window");
    c.expect(glued, "sprite stays on XRIGHT once scrolling");
    c.expect(cam.x0 < 0, "x0 goes negative (world slides left)");
    c.expect(cam.xBg < 0, "sky parallax moves left, slower than ground");
    c.expect(mla::worldX(cam) > mla::kXRight, "world X keeps growing");
    // Parallax is 1/5 of ground travel after scroll starts.
    const double ground = -cam.x0;
    const double sky = -cam.xBg;
    c.expect(sky > 0 && sky < ground / 4.0, "sky travel << ground travel");
    c.expect(mla::endingDistance(1) == 94 * 32, "world1 ending at 3008 px");
    return c.finish("07_camera_scroll");
}
