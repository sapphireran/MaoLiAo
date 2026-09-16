#include "inertia_portable.h"

namespace maoliao {

double integrate(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

}  // namespace maoliao
