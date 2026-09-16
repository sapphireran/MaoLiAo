#include "../common/maoliao_constants.h"
#include "../common/map_data.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

namespace {

int g_failures = 0;

void expect(const char* name, bool cond) {
    if (!cond) {
        std::fprintf(stderr, "FAIL %s\n", name);
        ++g_failures;
    } else {
        std::printf("ok   %s\n", name);
    }
}

void dumpMaps(const char* title, const std::vector<maoliao::MapRec>& maps) {
    std::printf("\n%s (%zu records)\n", title, maps.size());
    std::printf(" idx   x   y  id  xAmt yAmt  name\n");
    for (std::size_t i = 0; i < maps.size(); ++i) {
        const auto& m = maps[i];
        std::printf("%4zu %4d %3d %3d %5d %4d  %s\n", i, m.x, m.y, m.id, m.xAmount,
                    m.yAmount, maoliao::tileName(m.id));
    }
}

bool hasId(const std::vector<maoliao::MapRec>& maps, int id) {
    return std::any_of(maps.begin(), maps.end(),
                       [id](const maoliao::MapRec& m) { return m.id == id; });
}

// Walkable tops: ids 1,2,5,6 as a coarse ground silhouette for docs.
void asciiGround(const std::vector<maoliao::MapRec>& maps, int world) {
    const int width = (world == 2) ? 120 : 110;
    std::vector<int> top(width, 99);
    for (const auto& m : maps) {
        if (m.id != 1 && m.id != 2 && m.id != 5 && m.id != 6) {
            continue;
        }
        for (int j = 0; j < m.xAmount; ++j) {
            const int x = m.x + j;
            if (x >= 0 && x < width && m.y < top[x]) {
                top[x] = m.y;
            }
        }
    }
    std::printf("\nASCII walkable tops (ids 1,2,5,6), row numbers are tile y:\n");
    for (int row = 3; row <= 10; ++row) {
        std::string line;
        line.push_back(char('0' + (row % 10)));
        line.push_back(' ');
        for (int x = 0; x < width; ++x) {
            line.push_back(top[x] == row ? '#' : (top[x] < 99 && top[x] < row ? '.' : ' '));
        }
        std::printf("%s\n", line.c_str());
    }
    std::printf("  ");
    for (int x = 0; x < width; ++x) {
        std::putchar((x % 10 == 0) ? '0' + ((x / 10) % 10) : ' ');
    }
    std::putchar('\n');
}

}  // namespace

int main() {
    using namespace maoliao;

    std::printf("=== MaoLiAo authored maps vs MAP_NUMBER=%d ===\n", kMapNumber);

    const LevelTables w1 = world1();
    const LevelTables w2 = world2();
    const auto s1 = storedMaps(w1.authoredMaps);
    const auto s2 = storedMaps(w2.authoredMaps);

    dumpMaps("world 1 authored", w1.authoredMaps);
    dumpMaps("world 1 stored in Scene::map[30]", s1);
    asciiGround(s1, 1);

    dumpMaps("world 2 authored", w2.authoredMaps);
    dumpMaps("world 2 stored in Scene::map[30]", s2);
    asciiGround(s2, 2);

    std::printf("\n-- coins / enemies --\n");
    std::printf("world 1 coins %zu  enemies %zu\n", w1.coins.size(), w1.enemies.size());
    std::printf("world 2 coins %zu  enemies %zu\n", w2.coins.size(), w2.enemies.size());

    expect("w1 authored 32", w1.authoredMaps.size() == 32);
    expect("w1 stored 30", s1.size() == 30);
    expect("w1 keeps goal sign", hasId(s1, 12));
    expect("w1 drops last water", !std::any_of(s1.begin(), s1.end(), [](const MapRec& m) {
        return m.id == 13 && m.x == 75;
    }));

    expect("w2 authored 36", w2.authoredMaps.size() == 36);
    expect("w2 stored 30", s2.size() == 30);
    expect("w2 drops goal sign", !hasId(s2, 12));
    expect("w2 drops trees", !hasId(s2, 14));
    expect("w2 still has stair clouds", std::any_of(s2.begin(), s2.end(), [](const MapRec& m) {
        return m.x == 70 && m.id == 2;
    }));

    expect("w1 coin count 20", w1.coins.size() == 20);
    expect("w1 enemy count 10", w1.enemies.size() == 10);
    expect("w2 coin count 11", w2.coins.size() == 11);
    expect("w2 enemy count 6", w2.enemies.size() == 6);

    if (g_failures) {
        std::fprintf(stderr, "\n%d check(s) failed\n", g_failures);
        return 1;
    }
    std::printf("\nall map-layout checks passed\n");
    return 0;
}
