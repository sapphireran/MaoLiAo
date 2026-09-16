#pragma once

#include <cmath>
#include <iostream>

inline int g_failures = 0;

inline void check_impl_(bool cond, const char* expr, const char* file, int line) {
    if (!cond) {
        std::cerr << "FAIL " << file << ':' << line << "  " << expr << '\n';
        ++g_failures;
    }
}

#define CHECK(cond) check_impl_(static_cast<bool>(cond), #cond, __FILE__, __LINE__)

inline void check_near(double got, double want, double eps, const char* label) {
    if (std::fabs(got - want) > eps) {
        std::cerr << "FAIL " << label << " got=" << got << " want=" << want
                  << " eps=" << eps << '\n';
        ++g_failures;
    }
}

inline int report(const char* name) {
    if (g_failures != 0) {
        std::cerr << name << ": " << g_failures << " failure(s)\n";
        return 1;
    }
    std::cout << name << ": ok\n";
    return 0;
}
