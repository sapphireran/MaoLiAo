#include "check.hpp"
#include "constants.hpp"
#include "inertia.hpp"

#include <cmath>
#include <cstdio>
#include <vector>

// Replay Role's vertical integrator for a standing jump.
// Prints a compact trajectory and checks takeoff, hang time, and landing.

struct Sample {
    int frame = 0;
    double t = 0;
    double vY = 0;
    double yPx = 0;
};

int main() {
    std::printf("=== 01 inertia jump ===\n");
    std::printf("G=%.1f  REAL_HEIGHT=%.1f  UNREAL_HEIGHT=%.1f  dt=%.3f\n",
                mla::kG, mla::kRealHeight, mla::kUnrealHeight, mla::kTime);

    double vY = mla::jumpTakeoffVy();
    std::printf("takeoff vY = %.6f m/s  (analytic -sqrt(2gH))\n", vY);
    std::printf("Role: yy += move(vY,dt,G) * px/m   (EasyX Y grows down, so jump "
                "decreases yy)\n");

    // height = pixels above the takeoff ground (up positive).
    // Game EasyX: yy += toPixels(move);  height = -delta_yy.
    double height = 0;
    std::vector<Sample> log;
    int apexFrame = 0;
    double apexY = 0;
    int landFrame = -1;

    for (int f = 0; f < 200; ++f) {
        Sample s;
        s.frame = f;
        s.t = f * mla::kTime;
        s.vY = vY;
        s.yPx = height;
        log.push_back(s);
        if (height > apexY) {
            apexY = height;
            apexFrame = f;
        }
        const double dy = mla::integrate(vY, mla::kTime, mla::kG);
        height -= mla::toPixels(dy);
        if (f > 0 && height <= 0.0 && landFrame < 0) {
            landFrame = f + 1;
            break;
        }
    }

    std::printf("frame     t(s)        vY(m/s)      y(px)\n");
    for (std::size_t i = 0; i < log.size(); i += 8) {
        const Sample& s = log[i];
        std::printf("%5d  %7.3f  %12.6f  %10.3f\n", s.frame, s.t, s.vY, s.yPx);
    }
    if (!log.empty()) {
        const Sample& last = log.back();
        std::printf("%5d  %7.3f  %12.6f  %10.3f  (last before land)\n",
                    last.frame, last.t, last.vY, last.yPx);
    }

    const double analyticHang = 2.0 * std::fabs(mla::jumpTakeoffVy()) / mla::kG;
    std::printf("apex frame=%d y=%.3f px  land frame=%d  analytic hang=%.4f s\n",
                apexFrame, apexY, landFrame, analyticHang);

    mla::Checks c;
    c.near(mla::jumpTakeoffVy(), -std::sqrt(210.0), 1e-9, "takeoff matches -sqrt(210)");
    c.expect(apexY > 90.0 && apexY <= mla::kUnrealHeight + 2.0,
             "apex near UNREAL_HEIGHT (discrete gravity undershoots 101px)");
    c.expect(landFrame > 90 && landFrame < 110, "lands near 97 frames (0.97s)");
    c.near(analyticHang, 0.966092, 1e-4, "analytic hang time 2|v|/g");
    c.expect(mla::pxPerMetre() > 28.85 && mla::pxPerMetre() < 28.86,
             "px/m = 101/3.5");
    return c.finish("01_inertia_jump");
}
