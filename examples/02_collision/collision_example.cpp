// Four-corner AABB fixtures copied from Role::isHit / hitMap.
// Also shows why scenery IDs do not collide and why world-3 pipes kill.

#include "maoliao_core.h"

#include <iostream>

using namespace maoliao;

static int gFailures = 0;

static void expect(const char* name, bool cond) {
    if (!cond) {
        std::cerr << "FAIL " << name << "\n";
        ++gFailures;
    } else {
        std::cout << "ok   " << name << "\n";
    }
}

int main() {
    std::cout << "=== MaoLiAo collision extract ===\n";

    // Standing on a 15-wide grass run at row 9, like world 1's opening floor.
    MapRun grass{0, 9, 1, 15, 1, frictionU(1, 1)};
    const Rect floor = tileWorldRect(grass);
    expect("grass left", floor.left == 0);
    expect("grass top", floor.top == 9 * kHeight);
    expect("grass right", floor.right == 15 * kWidth);
    expect("grass bottom", floor.bottom == 10 * kHeight);

    // Cat at screen (64, 256) with x0 = 0 sits on y=8 tiles... wait:
    // floor top is 288. A 32-px cat with y=256 has feet at 288.
    Vec2i hero[4];
    heroWorldCorners(64, 256, 0, hero);
    const Vec2i box[2] = {{floor.left, floor.top}, {floor.right, floor.bottom}};
    expect("feet on grass", cornersHit(hero, box));

    // One pixel above: y=255, feet at 286, no corner inside [288, 320].
    heroWorldCorners(64, 255, 0, hero);
    expect("hover above grass", !cornersHit(hero, box));

    // Scenery grass id 11 is not solid.
    expect("id 11 scenery", isSceneryTileId(11));
    expect("id 11 not solid", !isSolidTileId(11));
    expect("id 10 solid pipe", isSolidTileId(10));

    // Fat pipe mouth: id 8 at (10, 2) is 64×64, not 32×32.
    MapRun mouth{10, 2, 8, 1, 1, frictionU(8, 3)};
    const Rect m = tileWorldRect(mouth);
    expect("pipe width 64", m.right - m.left == 64);
    expect("pipe height 64", m.bottom - m.top == 64);

    // World-3 kill table.
    expect("pipe kills unarmed", world3ContactKills(8, false));
    expect("cloud safe unarmed", !world3ContactKills(2, false));
    expect("pipe safe armed", !world3ContactKills(8, true));
    expect("victory never solid", !world3ContactKills(12, false));

    // Thin tile vs large actor: a 4-px-wide wall in the *center* of the cat
    // does not contain any of the four corners — the shipped helper misses it.
    const Vec2i thin[2] = {{64 + 14, 256}, {64 + 18, 288}};
    heroWorldCorners(64, 256, 0, hero);
    expect("center thin wall missed (known)", !cornersHit(hero, thin));

    if (gFailures != 0) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "all collision checks passed\n";
    return 0;
}
