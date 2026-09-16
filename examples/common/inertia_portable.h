#pragma once

namespace maoliao {

// Same contract as Inertia::move in MaoLiAo/inertia.cpp:
// X = v*t + 1/2 a t^2; v += a*t; return X.
double integrate(double& v, double t, double a);

}  // namespace maoliao
