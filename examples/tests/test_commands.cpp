#include "check.hpp"
#include "maoliao.hpp"

int main() {
    using namespace maoliao;

    CHECK(bit_for_key(Key::A) == 1);
    CHECK(bit_for_key(Key::D) == 2);
    CHECK(bit_for_key(Key::W) == 4);
    CHECK(bit_for_key(Key::K) == 4);
    CHECK(bit_for_key(Key::S) == 8);
    CHECK(bit_for_key(Key::J) == 16);
    CHECK(bit_for_key(Key::Esc) == 32);

    const int mask = command_mask({Key::A, Key::D, Key::W, Key::J});
    CHECK(mask == (1 | 2 | 4 | 16));
    CHECK(has_cmd(mask, kCmdLeft));
    CHECK(has_cmd(mask, kCmdShoot));
    CHECK(!has_cmd(mask, kCmdEsc));

    CHECK(is_virtual_only(kVirRestart));
    CHECK(is_virtual_only(kVirHome));
    CHECK(!is_virtual_only(kCmdLeft | kVirRestart));
    CHECK(decode_mask(mask).size() == 4);
    return report("test_commands");
}
