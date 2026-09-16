#pragma once

namespace mla {

// Faithful copy of Inertia::move (MaoLiAo/inertia.cpp):
//   X = v t + 1/2 a t^2
//   v = v + a t
// Returns displacement in the same units as v and a (metres in the game model).
double integrate(double& v, double t, double a);

// Pixel displacement used by Role after each integrate() call.
double toPixels(double metres);

}  // namespace mla
