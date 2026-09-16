// gameRecord.dat is one integer. The EasyX code fopen_s's without checking.
// This extract rejects 0 / 4+ / missing files without crashing.

#include "maoliao_core.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace maoliao;

static int gFailures = 0;

static void expectStatus(const char* name, SaveStatus got, SaveStatus want) {
    if (got != want) {
        std::cerr << "FAIL " << name << " got " << static_cast<int>(got)
                  << " want " << static_cast<int>(want) << "\n";
        ++gFailures;
    } else {
        std::cout << "ok   " << name << "\n";
    }
}

int main() {
    std::cout << "=== MaoLiAo save record ===\n";
    const auto dir = std::filesystem::temp_directory_path() / "maoliao_save_example";
    std::filesystem::create_directories(dir);
    const std::string path = (dir / "gameRecord.dat").string();

    expectStatus("reject 0", writeWorldSave(path, 0), SaveStatus::OutOfRange);
    expectStatus("reject 4", writeWorldSave(path, 4), SaveStatus::OutOfRange);

    expectStatus("write world 2", writeWorldSave(path, 2), SaveStatus::Ok);
    int world = 0;
    expectStatus("read world 2", readWorldSave(path, world), SaveStatus::Ok);
    if (world != 2) {
        std::cerr << "FAIL readback " << world << "\n";
        ++gFailures;
    } else {
        std::cout << "ok   readback 2\n";
    }

    // Same bytes the shipped writer emits: "%d" with no newline.
    {
        std::ofstream out(path, std::ios::trunc);
        out << 3;
    }
    expectStatus("read bare 3", readWorldSave(path, world), SaveStatus::Ok);
    if (world != 3) {
        std::cerr << "FAIL bare 3 -> " << world << "\n";
        ++gFailures;
    }

    {
        std::ofstream out(path, std::ios::trunc);
        out << 99;
    }
    expectStatus("reject 99", readWorldSave(path, world), SaveStatus::OutOfRange);

    {
        std::ofstream out(path, std::ios::trunc);
    }
    expectStatus("empty file", readWorldSave(path, world), SaveStatus::Missing);

    const std::string missing = (dir / "nope.dat").string();
    std::filesystem::remove(missing);
    expectStatus("missing file", readWorldSave(missing, world), SaveStatus::Missing);

    std::filesystem::remove_all(dir);

    if (gFailures != 0) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "all save-record checks passed\n";
    return 0;
}
