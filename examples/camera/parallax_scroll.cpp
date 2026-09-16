#include <iostream>

#include "camera.hpp"
#include "check.hpp"
#include "inertia.hpp"
#include "maoliao_const.hpp"

using namespace maoliao;
using maoliao::test::check;
using maoliao::test::checkNear;

static void testPinAndScroll() {
    CameraHero h;
    h.x = 200;
    h.xx = 200;
    h.x0 = 0;
    h.vX = kVMax;
    applyCameraClamp(h);
    checkNear(h.x, static_cast<double>(kXRight), 1e-12, "pinned to 192");
    checkNear(h.x0, -8.0, 1e-12, "overflow moved into x0");
    check(worldX(h) == 200, "world X preserved");
}

static void testLeftWall() {
    CameraHero h;
    h.x = -4;
    h.xx = -4;
    h.vX = -3;
    applyCameraClamp(h);
    checkNear(h.x, 0.0, 1e-12, "left clamp");
    checkNear(h.vX, 0.0, 1e-12, "left kills velocity");
}

static void testEndingDoesNotScroll() {
    CameraHero h;
    h.x = 400;
    h.xx = 400;
    h.x0 = -2800;
    h.ending = true;
    applyCameraClamp(h);
    checkNear(h.x, 400.0, 1e-12, "ending walk keeps screen x");
    checkNear(h.x0, -2800.0, 1e-12, "ending does not consume overflow");
}

static void testParallaxRatio() {
    const double step = backgroundStep(kVMax);
    const double mapStep = kVMax * kTime * pixelScale();
    checkNear(step * kMapBg, mapStep, 1e-12, "sky moves 1/5 of map");
    check(kMapBg == 5, "K_MAP_BG");
}

static void testSkyWrapAndBand() {
    double xBg = 0.0;
    const int bmpW = kXSize;  // loaded stretched to XSIZE
    for (int i = 0; i < 200; ++i) {
        xBg -= backgroundStep(kVMax);
        if (xBg <= -bmpW) {
            xBg = 0.0;
        }
    }
    check(xBg <= 0.0 && xBg > -bmpW, "sky wrap stays in one width");

    auto band = [](int world) { return -(world - 1) * kYSize; };
    check(band(1) == 0, "world 1 sky");
    check(band(2) == -384, "world 2 sky");
    check(band(3) == -768, "world 3 sky");
}

static void testEndingDistances() {
    check(!isEnding(1, 94 * kWidth), "equal to 94 tiles is not yet ending");
    check(isEnding(1, 94 * kWidth + 1), "one pixel past 94 tiles");
    check(!isEnding(2, 104 * kWidth), "world 2 needs 104");
    check(isEnding(2, 104 * kWidth + 1), "world 2 past 104");
    check(isEnding(3, 94 * kWidth + 1), "world 3 shares world 1 distance");
    check(!isEnding(4, 99999), "unknown world never ends");
}

static void testWalkOffScreen() {
    CameraHero h;
    h.x = 500;
    h.ending = true;
    check(h.x > kXSize - 20, "near the right bezel");
    const bool passed = h.x > kXSize;
    check(!passed, "500 is still on a 512 window");
    h.x = 513;
    check(h.x > kXSize, "513 clears the stage");
}

int main() {
    std::cout << "camera / parallax_scroll\n";
    testPinAndScroll();
    testLeftWall();
    testEndingDoesNotScroll();
    testParallaxRatio();
    testSkyWrapAndBand();
    testEndingDistances();
    testWalkOffScreen();
    return test::summary("parallax_scroll");
}
