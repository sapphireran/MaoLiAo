#pragma once

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace mla {

struct Checks {
    int failed = 0;
    int passed = 0;

    void expect(bool cond, const char* msg) {
        if (cond) {
            ++passed;
            std::printf("  ok  %s\n", msg);
        } else {
            ++failed;
            std::printf("  FAIL  %s\n", msg);
        }
    }

    void near(double got, double want, double eps, const char* msg) {
        const bool ok = std::fabs(got - want) <= eps;
        if (ok) {
            ++passed;
            std::printf("  ok  %s  (got %.6f)\n", msg, got);
        } else {
            ++failed;
            std::printf("  FAIL  %s  (got %.6f want %.6f eps %.6f)\n", msg, got,
                        want, eps);
        }
    }

    int finish(const char* name) const {
        std::printf("%s: %d passed, %d failed\n", name, passed, failed);
        return failed == 0 ? 0 : 1;
    }
};

}  // namespace mla
