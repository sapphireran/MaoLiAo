#include "maoliao_model.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace maoliao;

static int fail(const char* msg) {
    std::cerr << "FAIL: " << msg << "\n";
    return 1;
}

static std::string decode(int mask) {
    const int bits[] = {CMD_LEFT, CMD_RIGHT, CMD_UP, CMD_DOWN, CMD_SHOOT, CMD_ESC,
                        VIR_RETURN, VIR_RESTART, VIR_HOME};
    std::ostringstream o;
    bool first = true;
    for (int b : bits) {
        if (!has_bit(mask, b)) continue;
        if (!first) o << "|";
        o << command_name(b);
        first = false;
    }
    if (first) o << "0";
    return o.str();
}

int main() {
    const int walk_jump = encode_keys(false, true, true, false, false, false);
    std::cout << "D+W = " << walk_jump << " (" << decode(walk_jump) << ")\n";
    if (walk_jump != (CMD_RIGHT | CMD_UP)) return fail("D+W mask");
    if (!has_bit(walk_jump, CMD_UP) || has_bit(walk_jump, CMD_LEFT)) return fail("has_bit");

    const int all = encode_keys(true, true, true, true, true, true);
    if (all != 63) return fail("six play bits should sum to 63");
    std::cout << "all play bits = " << all << " (" << decode(all) << ")\n";

    if (!is_virtual_command(VIR_RESTART) || !is_virtual_command(VIR_HOME)) {
        return fail("virtual commands");
    }
    if (is_virtual_command(CMD_ESC) || is_virtual_command(CMD_ESC | VIR_HOME)) {
        return fail("main compares virtual values with ==, not &");
    }
    std::cout << "VIR_RESTART=" << VIR_RESTART << " VIR_HOME=" << VIR_HOME << "\n";

    // Title load rules: 1..3 only.
    const struct {
        const char* text;
        bool ok;
        int world;
    } saves[] = {
        {"1", true, 1},
        {"2", true, 2},
        {"3", true, 3},
        {"0", false, 0},
        {"4", false, 4},
        {"-1", false, -1},
        {"nope", false, 0},
        {"2\n", true, 2},
    };
    for (const auto& s : saves) {
        const auto p = parse_save(s.text);
        std::cout << "  save '" << s.text << "' -> ok=" << p.ok << " world=" << p.world;
        if (!p.ok) std::cout << " (" << p.error << ")";
        std::cout << "\n";
        if (p.ok != s.ok) return fail("save parse ok flag");
        if (s.ok && p.world != s.world) return fail("save world");
    }
    if (format_save(3) != "3") return fail("format_save");

    std::cout << "command_bits: ok\n";
    return 0;
}
