#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

namespace maoliao {
namespace test {

inline int& failures() {
    static int n = 0;
    return n;
}

inline void expect(bool cond, const std::string& msg) {
    if (!cond) {
        std::cerr << "FAIL: " << msg << "\n";
        ++failures();
    }
}

inline void expectNear(double got, double want, double eps, const std::string& msg) {
    if (!(std::fabs(got - want) <= eps)) {
        std::cerr << "FAIL: " << msg << " got=" << got << " want=" << want << "\n";
        ++failures();
    }
}

inline int summary(const char* name) {
    if (failures() == 0) {
        std::cout << name << ": all checks passed\n";
        return 0;
    }
    std::cout << name << ": " << failures() << " check(s) failed\n";
    return 1;
}

}  // namespace test
}  // namespace maoliao
