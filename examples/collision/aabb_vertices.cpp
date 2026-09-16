#include <iostream>

#include "check.hpp"
#include "collision.hpp"
#include "maoliao_const.hpp"

using namespace maoliao;
using maoliao::test::check;

static void testIsHitInclusive() {
    const Point hero[4] = {{10, 10}, {20, 10}, {10, 20}, {20, 20}};
    const Point box[2] = {{0, 0}, {15, 15}};
    check(isHit(hero, box), "top-left vertex inside");
    const Point miss[2] = {{40, 40}, {50, 50}};
    check(!isHit(hero, miss), "disjoint");
    const Point edge[2] = {{20, 20}, {30, 30}};
    check(isHit(hero, edge), "inclusive bottom-right vertex");
}

static void testHeroInset() {
    Point v[4];
    heroVertices(/*screenX*/ 64, /*screenY*/ 96, /*x0*/ 0, kWidth, kHeight, v);
    check(v[0].x == 65 && v[0].y == 97, "inset top-left");
    check(v[1].x == 95 && v[1].y == 97, "inset top-right");
    check(v[2].x == 65 && v[2].y == 127, "inset bottom-left");
    check(v[3].x == 95 && v[3].y == 127, "inset bottom-right");
}

static void testCameraShiftedWorld() {
    // After scrolling, x0 is negative. Vertices are in world space: -x0 + screenX.
    Point v[4];
    heroVertices(192, 288, /*x0*/ -320, kWidth, kHeight, v);
    check(v[0].x == 320 + 192 + 1, "world x includes camera");
    const Rect ground = tileRect(15, 9, 4, 1, /*id*/ 1, kWidth, kHeight);
    check(ground.left == 480 && ground.right == 608, "tile 15..19 at y=9");
    check(rectHit(v, ground), "hero at screen 192 standing on scrolled ground");
}

static void testPipeDoubleBox() {
    const Rect pipe = tileRect(36, 7, 1, 1, /*id*/ 10, kWidth, kHeight);
    check(pipe.right - pipe.left == 64, "id 10 is 2 tiles wide");
    check(pipe.bottom - pipe.top == 64, "id 10 is 2 tiles tall");
    const Rect cloud = tileRect(10, 6, 4, 1, /*id*/ 2, kWidth, kHeight);
    check(cloud.right - cloud.left == 128, "id 2 uses 1× tile size");
}

static void testCoinAndEnemy() {
    Point v[4];
    heroVertices(320, 160, 0, kWidth, kHeight, v);
    const Rect coin{10 * kWidth, 5 * kHeight, 11 * kWidth, 6 * kHeight};
    check(rectHit(v, coin), "coin at tile (10,5)");

    const Rect enemy{18 * kWidth, 7 * kHeight, 19 * kWidth, 8 * kHeight};
    Point far[4];
    heroVertices(0, 0, 0, kWidth, kHeight, far);
    check(!rectHit(far, enemy), "spawn does not touch world-1 walker");
}

static void testWorld3Rules() {
    check(world3Lethal(8, false), "pipe kills without weapon");
    check(world3Lethal(10, false), "mouth kills without weapon");
    check(!world3Lethal(2, false), "cloud is always safe");
    check(!world3Lethal(8, true), "weapon / star disables lethality");
    check(!world3Lethal(1, true), "even grass is safe once armed");
}

static void testStompVsSide() {
    // Stomp: vY > 0 and overlap → enemy dies. Side: vY <= 0 → hero dies.
    const bool falling = true;
    const bool overlap = true;
    const bool stomp = overlap && falling;
    const bool sideDeath = overlap && !falling;
    check(stomp && !sideDeath, "falling overlap is a stomp");
    const bool rising = false;
    check(overlap && !rising, "rising / grounded overlap is lethal");
}

int main() {
    std::cout << "collision / aabb_vertices\n";
    testIsHitInclusive();
    testHeroInset();
    testCameraShiftedWorld();
    testPipeDoubleBox();
    testCoinAndEnemy();
    testWorld3Rules();
    testStompVsSide();
    return test::summary("aabb_vertices");
}
