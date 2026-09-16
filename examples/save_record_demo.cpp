// gameRecord.dat is one integer: the current world (1..3).
// Mirrors Control::pauseClick write and Control::gameStart load.

#include "maoliao_core.hpp"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

using namespace maoliao;

static int fail = 0;

static void expect(const char* name, bool cond) {
    if (!cond) {
        std::printf("FAIL %s\n", name);
        ++fail;
    } else {
        std::printf("ok   %s\n", name);
    }
}

static std::string slurp(const std::string& path) {
    std::ifstream in(path.c_str());
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    std::string fixture = "fixtures/gameRecord.sample.dat";
    std::string out_path = "fixtures/gameRecord.roundtrip.dat";
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--in" && i + 1 < argc) {
            fixture = argv[++i];
        } else if (a == "--out" && i + 1 < argc) {
            out_path = argv[++i];
        }
    }

    const std::string sample = slurp(fixture);
    std::printf("fixture '%s' bytes=%zu text='%s'\n", fixture.c_str(), sample.size(),
                sample.c_str());

    bool ok = false;
    const int world = parse_save_world(sample.c_str(), &ok);
    expect("sample_accepted", ok);
    expect("sample_world_3", world == 3);

    const std::string written = format_save_world(world);
    {
        std::ofstream out(out_path.c_str(), std::ios::trunc);
        out << written;
    }
    const std::string again = slurp(out_path);
    bool ok2 = false;
    const int world2 = parse_save_world(again.c_str(), &ok2);
    expect("roundtrip_ok", ok2 && world2 == 3);
    std::printf("     wrote '%s'\n", again.c_str());

    struct Case {
        const char* text;
        bool accept;
        int value;
    };
    const Case cases[] = {
        {"1", true, 1},
        {"2", true, 2},
        {"3", true, 3},
        {"0", false, 0},
        {"4", false, 4},
        {"-1", false, -1},
        {"99", false, 99},
        {"abc", false, 0},
        {"2\n", true, 2},
    };
    for (const auto& c : cases) {
        bool a = false;
        const int v = parse_save_world(c.text, &a);
        char name[64];
        if (std::strcmp(c.text, "2\n") == 0) {
            std::snprintf(name, sizeof(name), "parse('2\\\\n')");
        } else {
            std::snprintf(name, sizeof(name), "parse('%s')", c.text);
        }
        if (a != c.accept || (c.accept && v != c.value) || (!c.accept && c.value != 0 && v != c.value && c.text[0] != 'a')) {
            // accept flag is the contract; out-of-range still returns the scanned int
            if (a != c.accept) {
                std::printf("FAIL %s accept=%d want=%d\n", name, (int)a, (int)c.accept);
                ++fail;
                continue;
            }
        }
        if (c.accept && v != c.value) {
            std::printf("FAIL %s value=%d want=%d\n", name, v, c.value);
            ++fail;
            continue;
        }
        std::printf("ok   %s accept=%d value=%d\n", name, (int)a, v);
    }

    if (fail) {
        std::printf("\n%d check(s) failed\n", fail);
        return 1;
    }
    std::printf("\nall save-format checks passed\n");
    return 0;
}
