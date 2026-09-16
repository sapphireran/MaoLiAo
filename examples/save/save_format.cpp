#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "check.hpp"
#include "maoliao_const.hpp"
#include "save.hpp"

using namespace maoliao;
using maoliao::test::check;

static std::string tmpPath(const char* name) {
    return std::string("/tmp/maoliao_") + name + ".dat";
}

static void testRoundTrip(int world) {
    const std::string path = tmpPath(("w" + std::to_string(world)).c_str());
    check(writeWorldSave(path, world), "write world");
    int got = -1;
    check(readWorldSave(path, got), "read world");
    check(got == world, "round-trip " + std::to_string(world));
    std::remove(path.c_str());
}

static void testReject(int flag, const char* label) {
    const std::string path = tmpPath(label);
    check(writeWorldSave(path, flag), std::string("write ") + label);
    int got = 99;
    check(!readWorldSave(path, got), std::string("reject ") + label);
    check(got == 99, "rejected save does not mutate out-param");
    std::remove(path.c_str());
}

static void testMissingFile() {
    int got = 0;
    check(!readWorldSave("/tmp/maoliao_does_not_exist_42.dat", got), "missing file");
}

static void testOriginalGrammar() {
    // Control writes "%d" with no newline. A trailing newline must still parse.
    const std::string path = tmpPath("raw");
    {
        std::ofstream out(path, std::ios::trunc);
        out << 2;
    }
    int got = 0;
    check(readWorldSave(path, got) && got == 2, "no-newline file like fprintf_s");
    {
        std::ofstream out(path, std::ios::trunc);
        out << "3\n";
    }
    check(readWorldSave(path, got) && got == 3, "newline-tolerant read");
    std::remove(path.c_str());
}

static void testCommittedSample() {
    // The tree ships MaoLiAo/gameRecord.dat with "3" when present relative to cwd.
    // We accept either location so the example works from examples/ or repo root.
    const char* candidates[] = {"../MaoLiAo/gameRecord.dat", "../../MaoLiAo/gameRecord.dat",
                                "MaoLiAo/gameRecord.dat"};
    bool seen = false;
    for (const char* c : candidates) {
        int got = 0;
        if (readWorldSave(c, got)) {
            check(got >= 1 && got <= 3, "committed save is a legal world");
            std::cout << "  found " << c << " → world " << got << '\n';
            seen = true;
            break;
        }
    }
    if (!seen) {
        std::cout << "  (no committed gameRecord.dat on this path; skipped)\n";
    }
}

int main() {
    std::cout << "save / save_format\n";
    testRoundTrip(1);
    testRoundTrip(2);
    testRoundTrip(3);
    testReject(0, "zero");
    testReject(4, "four");
    testReject(-1, "neg");
    testMissingFile();
    testOriginalGrammar();
    testCommittedSample();
    check(kLife == 5, "load does not restore lives; LIFE stays 5");
    return test::summary("save_format");
}
