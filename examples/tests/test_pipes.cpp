#include "check.hpp"
#include "maoliao.hpp"

int main() {
    using namespace maoliao;

    Engine engine(1);
    const auto columns = generate_gauntlet(engine, 7);
    CHECK(columns.size() == 7);
    for (std::size_t i = 0; i < columns.size(); ++i) {
        CHECK(column_sane(columns[i]));
        CHECK(columns[i].x == static_cast<int>(i) * 10 + 10);
        CHECK(columns[i].gap_tiles() == 4);
    }

    const auto tiles = flatten_gauntlet(columns);
    CHECK(tiles.size() == 30);
    CHECK(tiles[tiles.size() - 2].id == 2);
    CHECK(tiles.back().id == 12);
    CHECK(tiles.back().x == 101);

    Engine same(1);
    const auto again = generate_gauntlet(same, 7);
    for (std::size_t i = 0; i < columns.size(); ++i) {
        CHECK(again[i].height == columns[i].height);
    }

    Engine other(99);
    const auto different = generate_gauntlet(other, 7);
    bool differs = false;
    for (std::size_t i = 0; i < columns.size(); ++i) {
        differs = differs || different[i].height != columns[i].height;
    }
    CHECK(differs);
    return report("test_pipes");
}
