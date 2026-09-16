#include "maoliao_model.hpp"

#include <iomanip>
#include <iostream>

using namespace maoliao;

static int fail(const char* msg) {
    std::cerr << "FAIL: " << msg << "\n";
    return 1;
}

static void dump(const char* title, const std::vector<MapRec>& authored) {
    const auto live = loaded_map(authored);
    std::cout << title << " authored=" << authored.size() << " loaded=" << live.size()
              << " dropped=" << (authored.size() - live.size()) << "\n";
    std::cout << "  idx  x   y  id  xa ya  collide  name\n";
    for (size_t i = 0; i < authored.size(); ++i) {
        const auto& r = authored[i];
        const bool kept = i < live.size();
        std::cout << (kept ? "   " : "  D") << std::setw(2) << i << " " << std::setw(3) << r.x
                  << " " << std::setw(3) << r.y << " " << std::setw(2) << r.id << "  "
                  << std::setw(2) << r.x_amount << " " << std::setw(2) << r.y_amount << "  "
                  << (collides_as_tile(r.id) ? "yes     " : "no      ") << tile_name(r.id)
                  << "\n";
    }
}

static int colliding_count(const std::vector<MapRec>& recs) {
    int n = 0;
    for (const auto& r : recs)
        if (collides_as_tile(r.id)) ++n;
    return n;
}

int main() {
    const auto w1 = world1_map();
    const auto w2 = world2_map();
    dump("world 1", w1);
    dump("world 2", w2);

    if (w1.size() != 32) return fail("world 1 authors 32 records");
    if (w2.size() != 36) return fail("world 2 authors 36 records");
    if (loaded_map(w1).size() != 30) return fail("world 1 keeps 30");
    if (loaded_map(w2).size() != 30) return fail("world 2 keeps 30");

    // Last two world-1 rows are water scenery — dropped, not colliders.
    if (w1[30].id != 13 || w1[31].id != 13) return fail("dropped world-1 rows should be water");

    // World 2: first 30 include the staircase; sign and trees are dropped.
    if (w2[29].x != 75 || w2[29].id != 2) return fail("30th world-2 record is the last stair cloud");
    if (w2[31].id != 12) return fail("victory sign is the 32nd authored row");

    const int h[7] = {3, 5, 2, 6, 1, 4, 3};
    const auto w3 = world3_map_from_heights(h);
    dump("world 3 (sample heights 3,5,2,6,1,4,3)", w3);
    if (w3.size() != 30) return fail("world 3 sample should be exactly 30 records");
    if (loaded_map(w3).size() != 30) return fail("world 3 sample all fits");
    if (w3[28].id != 2 || w3[29].id != 12) return fail("runway then sign");

    if (ending_distance(1) != 94 * 32 || ending_distance(2) != 104 * 32) {
        return fail("finish-line pixels");
    }
    if (!is_ending(1, 94 * 32 + 1) || is_ending(1, 94 * 32)) return fail("ending is strict >");

    std::cout << "coins w1=" << world1_coins().size() << " w2=" << world2_coins().size() << "\n";
    std::cout << "enemies w1=" << world1_enemies().size() << " w2=" << world2_enemies().size()
              << " w3=" << world3_enemies().size() << "\n";
    std::cout << "colliding loaded w1=" << colliding_count(loaded_map(w1))
              << " w2=" << colliding_count(loaded_map(w2)) << "\n";

    if (world1_coins().size() != 20) return fail("world 1 coin count");
    if (world1_enemies().size() != 10) return fail("world 1 enemy count");

    std::cout << "map_layout: ok\n";
    return 0;
}
