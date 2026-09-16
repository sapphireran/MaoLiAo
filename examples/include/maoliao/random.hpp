#pragma once

#include <cstdint>

namespace maoliao {

// Minimal LCG so pipe / coin demos do not depend on libc rand() seeding.
class Engine {
public:
    explicit Engine(std::uint32_t seed = 1) : state_(seed == 0 ? 1 : seed) {}

    std::uint32_t next() {
        state_ = state_ * 1103515245u + 12345u;
        return (state_ >> 16) & 0x7fffu;
    }

    // define.h: random(a,b) = rand() % (b - a) + a
    int range(int a, int b) {
        if (b <= a) {
            return a;
        }
        return static_cast<int>(next() % static_cast<std::uint32_t>(b - a)) + a;
    }

private:
    std::uint32_t state_;
};

} // namespace maoliao
