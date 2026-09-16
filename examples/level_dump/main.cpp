#include "worlds.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Grid {
    int w = 0;
    int h = 0;
    std::vector<char> cells;

    Grid(int width, int height, char fill)
        : w(width), h(height), cells(static_cast<std::size_t>(width * height), fill) {}

    char& at(int x, int y) { return cells[static_cast<std::size_t>(y * w + x)]; }

    void stamp(const maoliao::MapStamp& m, bool storedOnlyLimit, std::size_t index) {
        if (storedOnlyLimit && index >= static_cast<std::size_t>(maoliao::kMapNumber)) {
            return;
        }
        const int tw = (m.id == 8 || m.id == 10) ? m.xAmount * 2 : m.xAmount;
        const int th = (m.id == 8 || m.id == 10) ? m.yAmount * 2 : m.yAmount;
        const char g = maoliao::tileGlyph(m.id);
        for (int j = 0; j < tw; ++j) {
            for (int k = 0; k < th; ++k) {
                const int x = m.x + j;
                const int y = m.y + k;
                if (x >= 0 && y >= 0 && x < w && y < h) {
                    at(x, y) = g;
                }
            }
        }
    }

    void mark(int x, int y, char g) {
        if (x >= 0 && y >= 0 && x < w && y < h) {
            at(x, y) = g;
        }
    }

    void print(std::ostream& os) const {
        for (int y = 0; y < h; ++y) {
            os << (y < 10 ? " " : "") << y << "|";
            for (int x = 0; x < w; ++x) {
                os << cells[static_cast<std::size_t>(y * w + x)];
            }
            os << "\n";
        }
    }
};

void dumpWorld(const maoliao::WorldDef& world, bool storedOnly) {
    using namespace maoliao;
    int maxX = 16;
    int maxY = 14;
    for (std::size_t i = 0; i < world.mapCount; ++i) {
        const MapStamp& m = world.maps[i];
        maxX = std::max(maxX, m.x + m.xAmount + 2);
        maxY = std::max(maxY, m.y + m.yAmount + 2);
    }
    for (std::size_t i = 0; i < world.coinCount; ++i) {
        maxX = std::max(maxX, world.coins[i].tx + 1);
        maxY = std::max(maxY, world.coins[i].ty + 1);
    }

    Grid grid(maxX, maxY, '.');
    const std::size_t limit = storedOnly ? storedMapCount(world) : world.mapCount;
    for (std::size_t i = 0; i < world.mapCount; ++i) {
        if (storedOnly && i >= limit) {
            break;
        }
        grid.stamp(world.maps[i], storedOnly, i);
    }
    for (std::size_t i = 0; i < world.coinCount; ++i) {
        grid.mark(world.coins[i].tx, world.coins[i].ty, 'o');
    }
    for (std::size_t i = 0; i < world.enemyCount; ++i) {
        grid.mark(world.enemies[i].tx, world.enemies[i].ty, 'e');
    }

    std::cout << "=== world " << world.id << " " << world.name
              << (storedOnly ? " (runtime 30-stamp cap)" : " (authored)") << " ===\n";
    std::cout << "stamps authored=" << world.mapCount << " stored=" << storedMapCount(world)
              << " overflow=" << (world.mapCount > kMapNumber ? world.mapCount - kMapNumber : 0)
              << " coins=" << world.coinCount << " enemies=" << world.enemyCount
              << " ending_tiles=" << world.endingTiles << "\n";
    if (world.food.pixels) {
        std::cout << "food_px=(" << world.food.x << "," << world.food.y << ") tile≈("
                  << world.food.x / kWidth << "," << world.food.y / kHeight << ")\n";
    } else {
        std::cout << "food_raw=(" << world.food.x << "," << world.food.y
                  << ")  # world 3 stores this as pixels anyway\n";
    }
    grid.print(std::cout);
    std::cout << "legend: # grass  = dirt  S/s snow  I ice  c cloud  ^/n/| pipe  "
                 "g grass-scenery  W goal  ~ water  T tree  o coin  e enemy\n\n";
}

void dumpWorld3Sample() {
    using namespace maoliao;
    auto rng = []() {
        static unsigned s = 1;  // MSVC-like default seed story
        s = s * 1103515245u + 12345u;
        return static_cast<int>((s / 65536u) % 32768u);
    };
    const std::vector<MapStamp> maps = generateWorld3Pipes(rng);
    auto rng2 = []() {
        static unsigned s = 1;
        s = s * 1103515245u + 12345u;
        return static_cast<int>((s / 65536u) % 32768u);
    };
    const std::vector<CoinStamp> coins = generateWorld3Coins(rng2);

    int maxX = 110;
    int maxY = 14;
    Grid grid(maxX, maxY, '.');
    for (std::size_t i = 0; i < maps.size(); ++i) {
        grid.stamp(maps[i], true, i);
    }
    for (const auto& c : coins) {
        grid.mark(c.tx, c.ty, 'o');
    }
    for (std::size_t i = 0; i < kWorld3Fixed.enemyCount; ++i) {
        grid.mark(kWorld3Fixed.enemies[i].tx, kWorld3Fixed.enemies[i].ty, 'e');
    }

    std::cout << "=== world 3 pipes (LCG seed 1, not MSVC rand) ===\n";
    std::cout << "stamps=" << maps.size() << " coins=" << coins.size()
              << " enemies=" << kWorld3Fixed.enemyCount << "\n";
    grid.print(std::cout);
    std::cout << "\n";
}

}  // namespace

int main(int argc, char** argv) {
    using namespace maoliao;
    bool storedOnly = true;
    int which = 0;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--authored") {
            storedOnly = false;
        } else if (a == "--stored") {
            storedOnly = true;
        } else if (a == "1" || a == "2" || a == "3") {
            which = a[0] - '0';
        } else if (a == "--help" || a == "-h") {
            std::cout << "usage: level_dump [--stored|--authored] [1|2|3]\n";
            return 0;
        } else {
            std::cerr << "unknown arg " << a << "\n";
            return 2;
        }
    }

    if (which == 0 || which == 1) {
        dumpWorld(kWorld1, storedOnly);
    }
    if (which == 0 || which == 2) {
        dumpWorld(kWorld2, storedOnly);
    }
    if (which == 0 || which == 3) {
        dumpWorld3Sample();
    }

    if (std::size(kWorld1Maps) != 32) {
        std::cerr << "FAIL: world 1 stamp count\n";
        return 1;
    }
    if (std::size(kWorld2Maps) != 36) {
        std::cerr << "FAIL: world 2 stamp count\n";
        return 1;
    }
    if (storedMapCount(kWorld1) != 30 || storedMapCount(kWorld2) != 30) {
        std::cerr << "FAIL: stored cap\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
