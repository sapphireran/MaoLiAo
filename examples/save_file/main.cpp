#include "../common/save_format.h"

#include <cstdio>
#include <string>

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

std::string dirOf(const char* argv0) {
    std::string s(argv0);
    const auto slash = s.find_last_of("/\\");
    if (slash == std::string::npos) {
        return ".";
    }
    return s.substr(0, slash);
}

}  // namespace

int main(int argc, char** argv) {
    using namespace maoliao;

    std::printf("=== gameRecord.dat parser ===\n");
    std::printf("game write: fprintf_s(fp, \"%%d\", world)  — no newline\n");
    std::printf("title read: 1..3 inclusive, else MessageBox\n\n");

    const auto ok1 = parseSaveText("1");
    expect("parse 1", ok1.status == SaveStatus::Ok && ok1.world == 1);
    const auto ok3 = parseSaveText("3\n");
    expect("parse 3 with newline", ok3.status == SaveStatus::Ok && ok3.world == 3);
    const auto spaced = parseSaveText("  2");
    expect("leading space", spaced.status == SaveStatus::Ok && spaced.world == 2);

    const auto zero = parseSaveText("0");
    expect("0 out of range", zero.status == SaveStatus::OutOfRange);
    const auto nine = parseSaveText("9");
    expect("9 out of range", nine.status == SaveStatus::OutOfRange);
    const auto empty = parseSaveText("");
    expect("empty unreadable", empty.status == SaveStatus::Unreadable);
    const auto junk = parseSaveText("world2");
    expect("text unreadable", junk.status == SaveStatus::Unreadable);

    expect("format matches fprintf %%d", formatSave(3) == "3");

    std::string sampleDir = "save_file/samples";
    if (argc > 0) {
        const std::string binDir = dirOf(argv[0]);
        // When run as bin/save_file, samples sit at ../save_file/samples.
        sampleDir = binDir + "/../save_file/samples";
    }

    const auto w1 = parseSaveFile(sampleDir + "/world1.dat");
    const auto w2 = parseSaveFile(sampleDir + "/world2.dat");
    const auto w3 = parseSaveFile(sampleDir + "/world3.dat");
    const auto bad = parseSaveFile(sampleDir + "/invalid.dat");
    const auto missing = parseSaveFile(sampleDir + "/nope.dat");

    expect("sample world1", w1.status == SaveStatus::Ok && w1.world == 1);
    expect("sample world2", w2.status == SaveStatus::Ok && w2.world == 2);
    expect("sample world3", w3.status == SaveStatus::Ok && w3.world == 3);
    expect("sample invalid", bad.status == SaveStatus::OutOfRange && bad.world == 9);
    expect("missing file", missing.status == SaveStatus::MissingFile);

    if (argc > 1) {
        const auto extra = parseSaveFile(argv[1]);
        std::printf("\nfile %s -> status=%d world=%d\n", argv[1],
                    static_cast<int>(extra.status), extra.world);
    }

    if (g_failures) {
        std::fprintf(stderr, "\n%d check(s) failed\n", g_failures);
        return 1;
    }
    std::printf("\nall save-file checks passed\n");
    return 0;
}
