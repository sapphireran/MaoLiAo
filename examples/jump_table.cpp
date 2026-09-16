// Print a per-frame jump table using the same integrator as Role::action.
// Also reports the designed closed-form numbers from docs/physics.md.

#include "maoliao_core.hpp"

#include <cmath>
#include <cstdio>

using namespace maoliao;

int main() {
    const double v0 = jump_impulse();
    std::printf("# MaoLiAo jump table\n");
    std::printf("# v0=%.6f m/s  G=%.1f  scale=%.6f px/m  TIME=%.3f\n", v0, kG,
                kPxPerMeter, kTime);
    std::printf("# closed_form_apex_m=%.2f  closed_form_apex_px=%.2f  t_apex=%.4fs\n",
                kRealHeight, kUnrealHeight, std::fabs(v0) / kG);
    std::printf("frame\tvY_after\tdy_px\tyy_px\tphase\n");

    double vy = v0;
    double yy = 0.0;
    int frames = 0;
    double min_yy = 0.0;
    int apex_frame = 0;
    const int limit = 200;
    while (frames < limit) {
        const double meters = inertia_move(vy, kTime, kG);
        const double dy = meters * kPxPerMeter;
        yy += dy; // same as yy - (-meters * scale)
        ++frames;
        const char* phase = (vy < 0.0) ? "up" : "down";
        if (yy < min_yy) {
            min_yy = yy;
            apex_frame = frames;
        }
        if (frames <= 8 || (frames % 8) == 0 || (vy >= 0.0 && frames <= apex_frame + 4) ||
            frames == apex_frame) {
            std::printf("%d\t%.5f\t%.4f\t%.4f\t%s\n", frames, vy, dy, yy, phase);
        }
        if (yy > 0.0 && vy > 0.0) {
            std::printf("%d\t%.5f\t%.4f\t%.4f\tlanded_past_start\n", frames, vy, dy, yy);
            break;
        }
    }
    std::printf("# numeric_apex_px=%.4f frame=%d hang_time_s=%.3f\n", -min_yy, apex_frame,
                frames * kTime);
    // Exit 0 always — this program is a table, inertia_demo owns the asserts.
    const double err = std::fabs((-min_yy) - kUnrealHeight);
    if (err > 1.5) {
        std::fprintf(stderr, "apex drifted more than 1.5px (err=%.3f)\n", err);
        return 1;
    }
    return 0;
}
