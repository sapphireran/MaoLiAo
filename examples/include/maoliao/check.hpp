#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

namespace maoliao {

inline int& failures() {
    static int n = 0;
    return n;
}

inline void require(bool cond, const char* expr, const char* file, int line) {
    if (cond) {
        return;
    }
    ++failures();
    std::cerr << "FAIL " << file << ":" << line << "  " << expr << "\n";
}

inline void require_near(double got, double want, double eps,
                         const char* file, int line) {
    if (std::fabs(got - want) <= eps) {
        return;
    }
    ++failures();
    std::cerr << "FAIL " << file << ":" << line
              << "  got " << got << " want " << want
              << " (eps " << eps << ")\n";
}

inline int done(const char* name) {
    if (failures() == 0) {
        std::cout << name << ": ok\n";
        return 0;
    }
    std::cout << name << ": FAILED (" << failures() << ")\n";
    return 1;
}

}  // namespace maoliao

#define MLA_CHECK(cond) ::maoliao::require((cond), #cond, __FILE__, __LINE__)
#define MLA_NEAR(got, want, eps) \
    ::maoliao::require_near((got), (want), (eps), __FILE__, __LINE__)
