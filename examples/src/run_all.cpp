// Print a one-page summary of the portable model. The Makefile runs the
// other demos as separate processes; this binary is the human-facing recap.

#include "maoliao_model.h"

#include <cstdio>

using namespace maoliao;

int main() {
    std::size_t t1 = 0, t2 = 0, e1 = 0, e2 = 0, e3 = 0, c1 = 0, c2 = 0;
    tilesFor(1, t1);
    tilesFor(2, t2);
    enemiesFor(1, e1);
    enemiesFor(2, e2);
    enemiesFor(3, e3);
    coinsFor(1, c1);
    coinsFor(2, c2);

    std::printf("MaoLiAo portable model\n");
    std::printf("  window      %d x %d   tile %d x %d\n", kXSize, kYSize, kWidth, kHeight);
    std::printf("  dt          %.3f s    camera rail x=%d..%d\n", kTime, kXLeft, kXRight);
    std::printf("  jump        v0=%.4f  unreal=%g px  g=%g\n", jumpSpeed(), kUnrealHeight, kG);
    std::printf("  lives       %d        vmax=%g\n", kLife, kVMax);
    std::printf("  world 1     tiles %zu (load %d) enemies %zu coins %zu end %d\n",
                t1, loadedTileCount(1), e1, c1, endingDistance(1));
    std::printf("  world 2     tiles %zu (load %d) enemies %zu coins %zu end %d\n",
                t2, loadedTileCount(2), e2, c2, endingDistance(2));
    std::printf("  world 3     tiles runtime/random enemies %zu end %d\n",
                e3, endingDistance(3));
    std::printf("  score       coin +10  stomp/shot +5\n");
    return 0;
}
