#include "check.hpp"
#include "constants.hpp"
#include "inertia.hpp"

#include <cmath>
#include <cstdio>

// Horizontal stop on grass vs ice, using Role's friction:
//   k  = -3 when vX > 0
//   a1 = k * G * u
//   u  = (V_MAX / T*) / G

struct Stop {
    const char* name;
    double mu;
    int frames;
    double lastV;
};

Stop runStop(const char* name, double mu, double v0) {
    double v = v0;
    int frames = 0;
    const double k = (v0 > 0) ? -3.0 : 3.0;
    const double a1 = k * mla::kG * mu;
    for (; frames < 400; ++frames) {
        if (std::fabs(v) < 1e-9) {
            break;
        }
        const double before = v;
        mla::integrate(v, mla::kTime, a1);
        if (before * v < 0) {
            v = 0;  // Role zeroes a sign change
            ++frames;
            break;
        }
    }
    Stop s;
    s.name = name;
    s.mu = mu;
    s.frames = frames;
    s.lastV = v;
    return s;
}

int main() {
    std::printf("=== 02 friction run ===\n");
    std::printf("mu_high (T1)=%.6f  mu_mid (T2)=%.6f  mu_low (T3)=%.6f\n",
                mla::muHigh(), mla::muMid(), mla::muLow());

    const Stop grass = runStop("grass T2", mla::muMid(), mla::kVMax);
    const Stop ice = runStop("ice T1", mla::muHigh(), mla::kVMax);
    const Stop slick = runStop("slick T3", mla::muLow(), mla::kVMax);

    auto report = [](const Stop& s) {
        const double a = 3.0 * mla::kG * s.mu;
        const double t = mla::kVMax / a;
        std::printf("%-10s  mu=%.4f  |a|=%.3f  analytic t=%.3fs (%d f)  "
                    "sim frames=%d  v=%.4f\n",
                    s.name, s.mu, a, t, static_cast<int>(t / mla::kTime + 0.5),
                    s.frames, s.lastV);
    };
    report(grass);
    report(ice);
    report(slick);

    // Full-speed run: accelerate from 0 with A_ROLE, no friction (same sign).
    double v = 0;
    int accelFrames = 0;
    while (std::fabs(v) < mla::kVMax - 1e-9 && accelFrames < 200) {
        mla::integrate(v, mla::kTime, mla::kARole);
        if (v > mla::kVMax) {
            v = mla::kVMax;
        }
        ++accelFrames;
    }
    std::printf("accel 0→V_MAX with A_ROLE: %d frames  v=%.4f\n", accelFrames, v);

    mla::Checks c;
    c.near(mla::muMid(), (8.0 / 1.2) / 30.0, 1e-9, "grass mu");
    c.near(mla::muHigh(), (8.0 / 0.5) / 30.0, 1e-9, "ice mu");
    c.near(3.0 * mla::kG * mla::muMid(), 20.0, 1e-6,
           "grass |a1| equals A_ROLE (20)");
    c.expect(grass.frames >= 38 && grass.frames <= 42, "grass stop ~40 frames");
    c.expect(ice.frames >= 15 && ice.frames <= 19, "ice stop ~17 frames");
    c.expect(slick.frames > grass.frames, "slick takes longer than grass");
    c.expect(accelFrames >= 39 && accelFrames <= 42, "0→8 m/s at 20 m/s² ~40 f");
    return c.finish("02_friction_run");
}
