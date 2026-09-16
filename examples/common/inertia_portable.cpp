#include "inertia_portable.h"

#include "define_portable.h"

#include <cmath>

namespace maoliao {

double inertiaMove(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

double jumpLaunchSpeed() {
    return -std::sqrt(2.0 * kG * kRealHeight);
}

}  // namespace maoliao
