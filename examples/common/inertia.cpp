#include "inertia.hpp"

#include "constants.hpp"

namespace mla {

double integrate(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

double toPixels(double metres) { return metres * pxPerMetre(); }

}  // namespace mla
