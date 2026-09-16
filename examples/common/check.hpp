#pragma once

#include <cmath>
#include <iostream>
#include <string>

namespace maoliao {
namespace test {

inline int& passed() {
    static int n = 0;
    return n;
}

inline int& failed() {
    static int n = 0;
    return n;
}

inline void check(bool cond, const std::string& msg) {
    if (cond) {
        ++passed();
        return;
    }
    ++failed();
    std::cerr << "  FAIL: " << msg << '\n';
}

inline void checkNear(double got, double expected, double eps, const std::string& msg) {
    const double d = std::fabs(got - expected);
    check(d <= eps, msg + " got " + std::to_string(got) + " expected " + std::to_string(expected) +
                        " delta " + std::to_string(d));
}

inline int summary(const char* name) {
    std::cout << name << ": " << passed() << " passed, " << failed() << " failed\n";
    return failed() == 0 ? 0 : 1;
}

}  // namespace test
}  // namespace maoliao
