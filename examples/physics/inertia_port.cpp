// Standalone copy of MaoLiAo/inertia.cpp plus the jump / run numbers
// from define.h. No EasyX. Build: g++ -std=c++17 -O2 inertia_port.cpp

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {

constexpr double TIME = 0.01;
constexpr double G = 30.0;
constexpr double REAL_HEIGHT = 3.5;
constexpr double UNREAL_HEIGHT = 101.0;
constexpr double V_MAX = 8.0;
constexpr double A_ROLE = 20.0;

double move(double& v, double t, double a)
{
    double X = v * t + 1.0 / 2.0 * a * t * t;
    v = v + a * t;
    return X;
}

void require(bool cond, const char* msg)
{
    if (!cond)
    {
        std::fprintf(stderr, "fail  %s\n", msg);
        std::exit(1);
    }
}

}  // namespace

int main()
{
    {
        double v = 3.0;
        double x = move(v, TIME, -2.0);
        require(std::fabs(x - (3.0 * TIME + 0.5 * -2.0 * TIME * TIME)) < 1e-12, "step x");
        require(std::fabs(v - (3.0 + -2.0 * TIME)) < 1e-12, "step v");
        std::printf("ok  cpp step\n");
    }

    const double launch = -std::sqrt(2.0 * G * REAL_HEIGHT);
    require(std::fabs(launch + std::sqrt(210.0)) < 1e-12, "launch");
    std::printf("ok  cpp launch %.9f\n", launch);

    double vY = launch;
    double yy = 0.0;
    double peak = 0.0;
    int ticks = 0;
    while (vY < 0.0)
    {
        yy = yy - (-move(vY, TIME, G) * UNREAL_HEIGHT / REAL_HEIGHT);
        if (-yy > peak)
            peak = -yy;
        ++ticks;
        require(ticks < 10000, "apex timeout");
    }
    require(ticks == 49, "apex ticks");
    require(peak > 100.0 && peak < 101.6, "peak pixels");
    std::printf("ok  cpp jump  ticks=%d  peak=%.6f\n", ticks, peak);

    double vX = 0.0;
    int run = 0;
    while (vX < V_MAX - 1e-12)
    {
        move(vX, TIME, A_ROLE);
        ++run;
        require(run < 10000, "vmax timeout");
    }
    require(run == 40, "vmax ticks");
    require(std::fabs(vX - V_MAX) < 1e-12, "vmax value");
    std::printf("ok  cpp run   ticks=%d  vX=%.4f\n", run, vX);
    std::printf("3 cpp inertia checks passed\n");
    return 0;
}
