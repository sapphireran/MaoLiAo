#include "save_format.hpp"

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

namespace {

int fail(const char* msg) {
    std::cerr << "FAIL: " << msg << "\n";
    return 1;
}

}  // namespace

int main(int argc, char** argv) {
    using namespace maoliao;

    if (argc >= 2 && std::string(argv[1]) == "--read") {
        if (argc < 3) {
            std::cerr << "usage: save_roundtrip --read <file>\n";
            return 2;
        }
        const auto s = readSaveFile(argv[2]);
        if (!s) {
            std::cerr << "could not read " << argv[2] << "\n";
            return 1;
        }
        std::cout << "world=" << s->world << " valid=" << (validWorld(s->world) ? "yes" : "no")
                  << "\n";
        return 0;
    }

    const auto empty = decodeSave("   ");
    if (empty) {
        return fail("empty save should be rejected");
    }
    const auto bad = decodeSave("nope");
    if (bad) {
        return fail("non-numeric save should be rejected");
    }
    const auto ok = decodeSave("2");
    if (!ok || ok->world != 2) {
        return fail("decode 2");
    }
    const auto spaced = decodeSave("  3\n");
    if (!spaced || spaced->world != 3) {
        return fail("decode spaced 3");
    }
    if (validWorld(0) || validWorld(4) || !validWorld(1)) {
        return fail("validWorld range");
    }
    if (encodeSave(SaveData{1}) != "1") {
        return fail("encode");
    }

    const fs::path tmp = fs::temp_directory_path() / "maoliao_save_roundtrip.dat";
    if (!writeSaveFile(tmp.string(), SaveData{3})) {
        return fail("write temp save");
    }
    const auto back = readSaveFile(tmp.string());
    std::error_code ec;
    fs::remove(tmp, ec);
    if (!back || back->world != 3) {
        return fail("roundtrip 3");
    }

    // Walk toward the repo root looking for the committed sample.
    fs::path here = fs::current_path();
    std::optional<SaveData> shipped;
    fs::path shippedPath;
    for (int up = 0; up < 6; ++up) {
        const fs::path candidate = here / "MaoLiAo" / "gameRecord.dat";
        if (fs::exists(candidate)) {
            shipped = readSaveFile(candidate.string());
            shippedPath = candidate;
            break;
        }
        if (!here.has_parent_path() || here.parent_path() == here) {
            break;
        }
        here = here.parent_path();
    }
    if (shipped) {
        std::cout << "shipped " << shippedPath << " world=" << shipped->world
                  << " valid=" << (validWorld(shipped->world) ? "yes" : "no") << "\n";
        if (!validWorld(shipped->world)) {
            return fail("shipped sample is not worlds 1–3");
        }
    }

    std::cout << "OK\n";
    return 0;
}
