#include "maoliao/inertia.h"

#include "maoliao/constants.h"

namespace maoliao {

double inertia_move(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

double inertia_move_pixels(double& v, double t, double a) {
    return inertia_move(v, t, a) * kPxPerMetre;
}

}  // namespace maoliao
