#include "maoliao.hpp"

#include <iostream>

int main() {
    using namespace maoliao;

    Engine engine(2020);
    const auto columns = generate_gauntlet(engine, 7);
    std::cout << "World 3 pipe gauntlet, seed 2020, 7 columns\n";
    bool all_sane = true;
    for (const auto& col : columns) {
        const bool ok = column_sane(col);
        all_sane = all_sane && ok;
        std::cout << "  x=" << col.x << " height=" << col.height
                  << " gap=[" << col.gap_top() << "," << col.gap_bottom()
                  << ") tiles=" << col.gap_tiles()
                  << (ok ? " ok" : " BAD") << '\n';
    }
    const auto tiles = flatten_gauntlet(columns);
    std::cout << "flattened records: " << tiles.size()
              << " (28 pipe pieces + cloud + flag)\n";
    std::cout << "last two ids: " << tiles[tiles.size() - 2].id << " "
              << tiles.back().id << '\n';
    return all_sane ? 0 : 1;
}
