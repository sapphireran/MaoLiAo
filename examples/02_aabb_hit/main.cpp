// Twin of Role::isHit, hitMap, hitEnemy, and the world-3 lethal rule.

#include "check.h"
#include "collision_portable.h"
#include "define_portable.h"
#include "map_portable.h"

#include <iostream>
#include <vector>

using namespace maoliao;
using maoliao::test::expect;

int main() {
    std::cout << "02_aabb_hit — inset corners, pipes, scenery, stomp\n";

    const auto c = spriteCorners(0, 0);
    expect(c[0].x == 1 && c[0].y == 1, "top-left inset");
    expect(c[1].x == kWidth - 1 && c[1].y == 1, "top-right inset");
    expect(c[2].x == 1 && c[2].y == kHeight - 1, "bottom-left inset");
    expect(c[3].x == kWidth - 1 && c[3].y == kHeight - 1, "bottom-right inset");

    // A 1-cell grass tile at grid (2, 8) occupies pixels [64,96] × [256,288].
    MapCell grass{2, 8, 1, 1, 1, 0.0};
    const Rect g = tileWorldRect(grass);
    expect(g.x0 == 64 && g.y0 == 256 && g.x1 == 96 && g.y1 == 288, "32×32 grass box");

    MapCell pipe{5, 7, 10, 1, 1, 0.0};
    const Rect p = tileWorldRect(pipe);
    expect(p.x0 == 160 && p.y0 == 224 && p.x1 == 224 && p.y1 == 288, "pipe mouth is 64×64");

    MapCell scenery{0, 0, 11, 1, 1, 0.0};
    std::vector<MapCell> cells = {grass, pipe, scenery};

    bool lethal = false;
    const MapCell* hit = hitMap(64, 256, cells.data(), cells.size(), 1, false, &lethal);
    expect(hit == &cells[0], "standing on grass hits id 1");
    expect(!lethal, "world 1 never sets lethal");

    // Scenery-only world: id 11 is skipped, so a sprite on (0,0) is free.
    MapCell onlyGrass{0, 0, 11, 4, 1, 0.0};
    expect(hitMap(0, 0, &onlyGrass, 1, 1, false) == nullptr, "id 11 is not solid");

    // World 3: overlapping a pipe without the star is lethal; a cloud is not.
    MapCell cloud{0, 6, 2, 4, 1, 0.0};
    MapCell w3pipe{3, 4, 8, 1, 1, 0.0};
    std::vector<MapCell> w3 = {cloud, w3pipe};

    lethal = false;
    expect(hitMap(10, 6 * kHeight, w3.data(), w3.size(), 3, false, &lethal) == &w3[0],
           "cloud is solid");
    expect(!lethal, "id 2 is safe in world 3");

    lethal = false;
    const int pipeWorldX = 3 * kWidth + 8;
    const int pipeY = 4 * kHeight + 8;
    const MapCell* pipeHit = hitMap(pipeWorldX, pipeY, w3.data(), w3.size(), 3, false, &lethal);
    expect(pipeHit == &w3[1], "pipe overlap");
    expect(lethal, "world 3 pipe kills without star");

    lethal = false;
    hitMap(pipeWorldX, pipeY, w3.data(), w3.size(), 3, true, &lethal);
    expect(!lethal, "star / isShoot disables world-3 tile death");

    // Enemies: 32×32 at world pixels, turn == 0 is a free slot.
    Enemy foes[3] = {{200, 100, 1}, {0, 0, 0}, {400, 80, -1}};
    expect(hitEnemy(200, 100, foes, 3) == &foes[0], "body overlap");
    expect(hitEnemy(0, 0, foes, 3) == nullptr, "zeroed slot ignored");
    expect(classifyEnemyContact(200, 100, 5.0, foes, 3) == EnemyContact::Stomp,
           "falling vY > 0 stomps");
    expect(classifyEnemyContact(200, 100, 0.0, foes, 3) == EnemyContact::Lethal,
           "grounded bump kills");
    expect(classifyEnemyContact(200, 100, -4.0, foes, 3) == EnemyContact::Lethal,
           "rising into an enemy kills");
    expect(classifyEnemyContact(0, 0, 5.0, foes, 3) == EnemyContact::None, "no contact");

    // Near-miss: sprite whose corners miss a 32×32 tile (documents isHit's limit).
    MapCell thin{4, 4, 1, 1, 1, 0.0};
    // Sprite at world (4*32 - 2, 4*32) overlaps 2 px but inset corners start at +1.
    const bool barely = cornersHit(spriteCorners(4 * kWidth - 2, 4 * kHeight),
                                   tileWorldRect(thin));
    expect(barely, "2 px overlap still catches an inset corner on this alignment");

    return test::summary("02_aabb_hit");
}
