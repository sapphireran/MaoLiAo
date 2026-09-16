#pragma once

namespace maoliao {

// Same suvat step as MaoLiAo/inertia.cpp: mutates v, returns Δx in model units.
double inertia_move(double& v, double t, double a);

// Pixel displacement used by Role::action after inertia_move.
double inertia_move_pixels(double& v, double t, double a);

}  // namespace maoliao
