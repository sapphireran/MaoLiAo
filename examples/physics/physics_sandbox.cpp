#include "inertia_portable.h"
#include "physics_constants.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>

namespace {

struct JumpResult {
    double apex_metres = 0.0;
    double apex_pixels = 0.0;
    int frames = 0;
    double time = 0.0;
};

JumpResult simulate_jump()
{
    using namespace maoliao;
    double vY = jump_takeoff();
    double metres = 0.0;
    JumpResult out;
    const int kMax = 2000;
    for (int i = 0; i < kMax; ++i) {
        // Role::action while airborne:
        //   yy -= -Inertia::move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT
        // move() returns metres (design units) and applies +G to vY.
        const double step = move(vY, kTime, kG);
        metres += -step;
        if (metres > out.apex_metres)
            out.apex_metres = metres;
        ++out.frames;
        if (vY >= 0.0)
            break;
    }
    out.time = out.frames * kTime;
    out.apex_pixels = metres_to_pixels(out.apex_metres);
    return out;
}

void cmd_jump()
{
    using namespace maoliao;
    const double v0 = jump_takeoff();
    const double closed_time = std::fabs(v0) / kG;
    const JumpResult r = simulate_jump();
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "takeoff_vY        " << v0 << " m/s\n";
    std::cout << "closed_apex_m     " << kRealHeight << "\n";
    std::cout << "closed_apex_px    " << static_cast<double>(kUnrealHeight) << "\n";
    std::cout << "closed_apex_s     " << closed_time << "\n";
    std::cout << "sim_apex_m        " << r.apex_metres << "\n";
    std::cout << "sim_apex_px       " << r.apex_pixels << "\n";
    std::cout << "sim_frames        " << r.frames << "\n";
    std::cout << "sim_seconds       " << r.time << "\n";
    std::cout << "pixel_error       " << (r.apex_pixels - kUnrealHeight) << "\n";
}

void cmd_friction()
{
    using namespace maoliao;
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "id  u_overworld  u_pipes  a1_right  a1_left\n";
    for (int id = 1; id <= 14; ++id) {
        std::cout << std::setw(2) << id << "  "
                  << std::setw(11) << friction_u(id, false) << "  "
                  << std::setw(7) << friction_u(id, true) << "  "
                  << std::setw(8) << friction_accel(id, false, +1.0) << "  "
                  << std::setw(7) << friction_accel(id, false, -1.0) << "\n";
    }
}

int cmd_rail()
{
    using namespace maoliao;
    double xx = static_cast<double>(kX0);
    double x0 = 0.0;
    double vx = 0.0;
    int rail_frame = -1;
    const int frames = 400;
    for (int i = 0; i < frames; ++i) {
        const double a = kARole;  // hold CMD_RIGHT
        const double tmp = vx;
        double H = move(vx, kTime, a) * static_cast<double>(kUnrealHeight) / kRealHeight;
        if (tmp * vx < 0.0)
            vx = 0.0;
        xx += H;
        double x = xx;
        const double ax = std::fabs(vx);
        if (ax > kVMax)
            vx = vx / ax * kVMax;
        if (x > kXRight) {
            if (rail_frame < 0)
                rail_frame = i;
            x0 -= (x - kXRight);
            x = kXRight;
            xx = x;
        }
    }
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "start_x           " << kX0 << "\n";
    std::cout << "rail_x            " << kXRight << "\n";
    std::cout << "frames_to_rail    " << rail_frame << "\n";
    std::cout << "final_x           " << xx << "\n";
    std::cout << "final_x0          " << x0 << "\n";
    std::cout << "final_vx          " << vx << "\n";
    std::cout << "map_x             " << (-x0 + xx) << "\n";
    return 0;
}

}  // namespace

int main(int argc, char** argv)
{
    const std::string cmd = (argc > 1) ? argv[1] : "jump";
    if (cmd == "jump") {
        cmd_jump();
        return 0;
    }
    if (cmd == "friction") {
        cmd_friction();
        return 0;
    }
    if (cmd == "rail") {
        return cmd_rail();
    }
    std::cerr << "usage: physics_sandbox [jump|friction|rail]\n";
    return 2;
}
