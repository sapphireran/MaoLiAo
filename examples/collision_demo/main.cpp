#include "../common/aabb.h"
#include "../common/maoliao_constants.h"

#include <cstdio>
#include <cstdlib>

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

}  // namespace

int main() {
    using namespace maoliao;

    std::printf("=== MaoLiAo four-corner AABB ===\n");

    Point hero[4];
    heroCorners(/*screenX=*/64, /*screenY=*/64, /*x0=*/0, hero);
    expect("inset left", hero[0].x == 65);
    expect("inset top", hero[0].y == 65);
    expect("inset right", hero[1].x == 64 + 32 - 1);
    expect("inset bottom", hero[2].y == 64 + 32 - 1);

    // Camera: x0 = -200 means the world origin is 200 px left of the screen.
    heroCorners(192, 288, -200, hero);
    expect("world x uses -x0", hero[0].x == 200 + 192 + 1);

    Point grass[2];
    tileBox(0, 9, /*id=*/1, /*xAmount=*/15, /*yAmount=*/1, grass);
    expect("grass left", grass[0].x == 0 && grass[0].y == 9 * 32);
    expect("grass right", grass[1].x == 15 * 32 && grass[1].y == 10 * 32);

    Point pipe[2];
    tileBox(36, 7, /*id=*/10, 1, 1, pipe);
    expect("pipe mouth 64x64",
           pipe[1].x - pipe[0].x == 64 && pipe[1].y - pipe[0].y == 64);

    // Standing on world-1 grass: hero feet at y=256 (row 8), grass at y=288.
    heroCorners(64, 8 * 32, 0, hero);
    Point under[2];
    tileBox(0, 9, 1, 15, 1, under);
    expect("on top of grass, no overlap", !isHit(hero, under));

    // One pixel into the tile (the +1 probe Role uses when airborne).
    heroCorners(64, 8 * 32 + 1, 0, hero);
    expect("y+1 probe overlaps grass", isHit(hero, under));

    // Completely to the right of the 15-tile strip.
    heroCorners(15 * 32 + 10, 9 * 32, 0, hero);
    expect("past the strip", !isHit(hero, under));

    // Inclusive edges: a corner exactly on the box counts (Role::isHit).
    Point box[2] = {{100, 100}, {132, 132}};
    Point corners[4] = {{100, 100}, {110, 90}, {90, 110}, {90, 90}};
    expect("corner on min edge", isHit(corners, box));
    Point miss[4] = {{99, 100}, {99, 132}, {99, 116}, {50, 50}};
    expect("just left of box", !isHit(miss, box));

    expect("w3 pipe lethal without weapon", world3Lethal(8, false));
    expect("w3 cloud safe", !world3Lethal(2, false));
    expect("w3 pipe safe with weapon", !world3Lethal(8, true));

    // Score reminders (documentation, not simulation).
    std::printf("\nscore table: coin +10, stomp +5, bullet +5\n");
    expect("life count", kLife == 5);
    expect("screen", kXSize == 512 && kYSize == 384);
    expect("right lock", kXRight == 192);

    if (g_failures) {
        std::fprintf(stderr, "\n%d check(s) failed\n", g_failures);
        return 1;
    }
    std::printf("\nall collision checks passed\n");
    return 0;
}
