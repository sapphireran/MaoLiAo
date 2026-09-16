#pragma once

// Twin of MaoLiAo/inertia.cpp: Inertia::move.

namespace maoliao {

// Advance velocity by a*t and return the displacement in "metres".
// Mutates v, same as the game.
double inertiaMove(double& v, double t, double a);

// Jump launch speed used when CMD_UP is accepted.
double jumpLaunchSpeed();

}  // namespace maoliao
