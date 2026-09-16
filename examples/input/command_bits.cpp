#include <iostream>

#include "check.hpp"
#include "input.hpp"
#include "maoliao_const.hpp"

using namespace maoliao;
using maoliao::test::check;

static void testSingleKeys() {
    check(commandFromKeys(static_cast<int>(Key::A)) == kCmdLeft, "A → LEFT");
    check(commandFromKeys(static_cast<int>(Key::D)) == kCmdRight, "D → RIGHT");
    check(commandFromKeys(static_cast<int>(Key::W)) == kCmdUp, "W → UP");
    check(commandFromKeys(static_cast<int>(Key::K)) == kCmdUp, "K → UP (not its own bit)");
    check(commandFromKeys(static_cast<int>(Key::S)) == kCmdDown, "S → DOWN");
    check(commandFromKeys(static_cast<int>(Key::J)) == kCmdShoot, "J → SHOOT");
    check(commandFromKeys(static_cast<int>(Key::Esc)) == kCmdEsc, "Esc → ESC");
}

static void testChords() {
    const int runJump = commandFromKeys(static_cast<int>(Key::D) | static_cast<int>(Key::W));
    check(runJump == (kCmdRight | kCmdUp), "D+W run-jump");
    const int bothJump = commandFromKeys(static_cast<int>(Key::W) | static_cast<int>(Key::K));
    check(bothJump == kCmdUp, "W+K still a single UP bit");
    const int fireWalk = commandFromKeys(static_cast<int>(Key::A) | static_cast<int>(Key::J));
    check(fireWalk == (kCmdLeft | kCmdShoot), "A+J");
    const int everything = commandFromKeys(static_cast<int>(Key::A) | static_cast<int>(Key::D) |
                                           static_cast<int>(Key::W) | static_cast<int>(Key::S) |
                                           static_cast<int>(Key::J) | static_cast<int>(Key::Esc));
    check(everything == (kCmdLeft | kCmdRight | kCmdUp | kCmdDown | kCmdShoot | kCmdEsc),
          "full chord is 63");
}

static void testVirtualKeysDistinct() {
    check((kVirReturn & 63) == 0, "VIR_RETURN above physical bits");
    check((kVirRestart & 63) == 0, "VIR_RESTART above physical bits");
    check((kVirHome & 63) == 0, "VIR_HOME above physical bits");
    check(kVirRestart == 128, "restart value");
    check(kVirHome == 256, "home value");
}

static void testMenuHitBoxes() {
    const HitBox start = menuRow(0);
    const HitBox intro = menuRow(1);
    const HitBox guide = menuRow(2);
    const HitBox quit = menuRow(3);
    const HitBox load = menuRow(4);
    check(start.left == 211 && start.right == 301, "90 px centered column");
    check(start.top == 128 && start.bottom == 158, "first row at YSIZE/3");
    check(load.top == 248 && load.bottom == 278, "fifth home row");
    check(inside(start, 256, 140), "click center of Start");
    check(!inside(start, 256, 160), "click just into Introduction");
    check(inside(intro, 256, 160), "Introduction row");
    check(inside(guide, 220, 200), "Directions row");
    check(inside(quit, 280, 230), "Exit row");
    check(inside(load, 256, 260), "Read-save row");
    check(!inside(start, 210, 140), "left edge is exclusive (original uses >)");
    check(!inside(start, 301, 140), "right edge exclusive");

    const HitBox pauseSave = menuRow(3);  // pause only has 4 rows
    check(inside(pauseSave, 256, 230), "pause Write Data");

    const HitBox back = backButton();
    check(back.left == 466 && back.top == 358, "返回 corner");
    check(inside(back, 480, 370), "inside 返回");
    check(!inside(back, 465, 370), "outside 返回");
}

static void testStickyMaskModel() {
    // getKey only refreshes when _kbhit(); last mask sticks. Document the rule.
    int key = commandFromKeys(static_cast<int>(Key::D));
    check(key == kCmdRight, "initial D");
    const bool kbhit = false;
    if (kbhit) {
        key = commandFromKeys(0);
    }
    check(key == kCmdRight, "empty buffer keeps last mask");
    key = kVirHome;
    check((key & kCmdEsc) == 0, "virtual home is not Esc");
    check(key == kVirHome, "pause can overwrite the physical mask");
}

int main() {
    std::cout << "input / command_bits\n";
    testSingleKeys();
    testChords();
    testVirtualKeysDistinct();
    testMenuHitBoxes();
    testStickyMaskModel();
    return test::summary("command_bits");
}
