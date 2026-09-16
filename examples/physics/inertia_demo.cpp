// Standalone port of Inertia::move and the Role jump launch.
// Build: g++ -O2 -o inertia_demo examples/physics/inertia_demo.cpp
// No EasyX / Windows headers.

#include <cmath>
#include <iomanip>
#include <iostream>

static const double TIME = 0.01;
static const double G = 30.0;
static const double REAL_HEIGHT = 3.5;
static const double UNREAL_HEIGHT = 3 * 32 + 5; // 101
static const double V_MAX = 8.0;
static const double T1 = 0.5;
static const double T2 = 1.2;
static const double T3 = 1.5;

static double move(double& v, double t, double a)
{
    double X = v * t + 1.0 / 2.0 * a * t * t;
    v = v + a * t;
    return X;
}

static double friction_u(int id, int world)
{
    double t = T3;
    if (world == 3)
    {
        if (id == 1 || id == 3 || id == 4 || id == 5 || id == 6)
            t = T1;
        else if (id == 2)
            t = T2;
    }
    else
    {
        if (id == 1 || id == 2 || id == 3 || id == 4 || id == 5)
            t = T2;
        else if (id == 6)
            t = T1;
    }
    return (V_MAX / t) / G;
}

int main()
{
    const double scale = UNREAL_HEIGHT / REAL_HEIGHT;
    const double launch = -std::sqrt(2.0 * G * REAL_HEIGHT);

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "MaoLiAo inertia demo\n";
    std::cout << "launch vY = " << launch << "\n";
    std::cout << "pixel scale = " << scale << "\n";
    std::cout << "u grass(id=1,w1) = " << friction_u(1, 1) << "\n";
    std::cout << "u ice(id=6,w1)   = " << friction_u(6, 1) << "\n";
    std::cout << "u pipe(id=7,w3)  = " << friction_u(7, 3) << "\n\n";

    double vY = launch;
    double y_up = 0.0;
    double peak = 0.0;
    int peak_frame = 0;
    std::cout << "frame  vY_before     dpx       y_up\n";
    for (int frame = 1; frame <= 120; ++frame)
    {
        const double before = vY;
        const double meters = move(vY, TIME, G);
        const double dpx = -meters * scale;
        y_up += dpx;
        if (y_up > peak)
        {
            peak = y_up;
            peak_frame = frame;
        }
        if (frame <= 8 || (vY > 0 && before <= 0) || frame == 96)
        {
            std::cout << std::setw(5) << frame << "  "
                      << std::setw(10) << before << "  "
                      << std::setw(8) << dpx << "  "
                      << std::setw(8) << y_up << "\n";
        }
        if (frame > 2 && y_up <= 0.0 && vY > 0)
            break;
    }
    std::cout << "\npeak rise " << peak << " px at frame " << peak_frame << "\n";
    return 0;
}
