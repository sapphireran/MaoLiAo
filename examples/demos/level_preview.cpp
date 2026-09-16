#include "maoliao/constants.h"
#include "maoliao/levels.h"
#include "maoliao/render.h"

#include <iostream>

namespace {

void dump(const maoliao::Level& authored, const maoliao::Level& loaded) {
    using namespace maoliao;
    std::cout << "world " << authored.world << "\n";
    std::cout << "  authored tiles=" << authored.tiles.size()
              << "  loaded tiles=" << loaded.tiles.size()
              << "  cap=" << kMapNumber << "\n";
    std::cout << "  coins=" << authored.coins.size()
              << "  food=" << authored.food.size()
              << "  enemies=" << authored.enemies.size() << "\n";
    std::cout << "  ending " << authored.ending_tiles << " tiles ("
              << ending_distance_px(authored.world) << " px)\n";
    if (authored.tiles.size() > loaded.tiles.size()) {
        std::cout << "  dropped ids:";
        for (std::size_t i = loaded.tiles.size(); i < authored.tiles.size(); ++i) {
            std::cout << ' ' << authored.tiles[i].id << "@(" << authored.tiles[i].x
                      << ',' << authored.tiles[i].y << ')';
        }
        std::cout << "\n";
    }
}

void print_slice(const maoliao::Level& level, int x0, int x1, int y0, int y1) {
    std::cout << "  ASCII tiles x=[" << x0 << "," << x1 << "] y=[" << y0 << ","
              << y1 << "]  (# solid  . scenery  o coin  * food  e enemy)\n";
    const auto rows = maoliao::render_ascii(level, x0, x1, y0, y1);
    int y = y0;
    for (const std::string& row : rows) {
        std::cout << "  y=" << y;
        if (y < 10) {
            std::cout << ' ';
        }
        std::cout << " |" << row << "|\n";
        ++y;
    }
}

}  // namespace

int main() {
    using namespace maoliao;

    for (int w = 1; w <= 2; ++w) {
        const Level a = authored_level(w);
        const Level b = loaded_level(w);
        dump(a, b);
        print_slice(a, 0, 40, 3, 11);
        std::cout << '\n';
    }

    const Level w3a = authored_world3(2020);
    const Level w3b = loaded_level(3);
    dump(w3a, w3b);
    print_slice(w3a, 0, 40, 0, 11);
    std::cout << "\nworld 3 food is at pixel (" << w3a.food[0].pixel_x << ","
              << w3a.food[0].pixel_y << ")\n";
    return 0;
}
