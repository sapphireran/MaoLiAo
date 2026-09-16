// Tile id → mu, same switch as Scene::createMap for worlds 1/2 and 3.

#include "maoliao_model.h"

#include <cmath>
#include <cstdio>

using namespace maoliao;

static bool nearly(double a, double b) { return std::fabs(a - b) < 1e-12; }

int main() {
    const double muHigh = (kVMax / kT1) / kG;
    const double muMid = (kVMax / kT2) / kG;
    const double muLow = (kVMax / kT3) / kG;

    MLA_CHECK(nearly(muFromFeel(Feel::High), muHigh));
    MLA_CHECK(nearly(muFromFeel(Feel::Mid), muMid));
    MLA_CHECK(nearly(muFromFeel(Feel::Low), muLow));
    MLA_CHECK(muHigh > muMid && muMid > muLow);

    // World 1/2: id 6 (pipe brick) is the sticky one.
    MLA_CHECK(feelForIdWorld12(6) == Feel::High);
    MLA_CHECK(feelForIdWorld12(1) == Feel::Mid);
    MLA_CHECK(feelForIdWorld12(2) == Feel::Mid);
    MLA_CHECK(feelForIdWorld12(10) == Feel::Low);

    // World 3: most solid bricks use T1; clouds stay mid.
    MLA_CHECK(feelForIdWorld3(6) == Feel::High);
    MLA_CHECK(feelForIdWorld3(1) == Feel::High);
    MLA_CHECK(feelForIdWorld3(2) == Feel::Mid);
    MLA_CHECK(feelForIdWorld3(8) == Feel::Low);
    MLA_CHECK(feelForIdWorld3(7) == Feel::Low);

    // Count how many loaded world-1 tiles sit on each feel.
    std::size_t n = 0;
    const Tile* tiles = tilesFor(1, n);
    int high = 0, mid = 0, low = 0, deco = 0;
    const int loaded = loadedTileCount(1);
    for (int i = 0; i < loaded; ++i) {
        if (!collides(tiles[i].id)) {
            ++deco;
            continue;
        }
        switch (feelForIdWorld12(tiles[i].id)) {
            case Feel::High: ++high; break;
            case Feel::Mid: ++mid; break;
            case Feel::Low: ++low; break;
        }
    }

    std::printf("friction_demo ok  mu=%.5f/%.5f/%.5f  w1_loaded=%d high=%d mid=%d low=%d deco=%d n_table=%zu\n",
                muHigh, muMid, muLow, loaded, high, mid, low, deco, n);
    return 0;
}
