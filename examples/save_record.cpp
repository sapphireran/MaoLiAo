#include "portable/save_io.hpp"

#include <cstdio>
#include <iostream>
#include <string>

namespace {

int gFailures = 0;

void expect(const char* name, bool ok) {
    if (!ok) {
        std::cerr << "FAIL " << name << "\n";
        ++gFailures;
    } else {
        std::cout << "PASS " << name << "\n";
    }
}

} // namespace

int main() {
    const std::string path = "gameRecord.example.dat";

    expect("write_world_2", mla::writeWorld(path, 2));

    int world = 0;
    expect("read_world_2", mla::readWorld(path, world) && world == 2);

    expect("write_world_3", mla::writeWorld(path, 3));
    expect("read_world_3", mla::readWorld(path, world) && world == 3);

    expect("write_world_1", mla::writeWorld(path, 1));
    expect("read_world_1", mla::readWorld(path, world) && world == 1);

    expect("reject_zero", mla::writeWorld(path, 0) && !mla::readWorld(path, world));
    expect("reject_four", mla::writeWorld(path, 4) && !mla::readWorld(path, world));
    expect("missing_file", !mla::readWorld("no-such-maoliao-save.dat", world));

    std::remove(path.c_str());

    if (gFailures) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "save_record: all checks passed\n";
    return 0;
}
