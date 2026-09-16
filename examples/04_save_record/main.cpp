// Twin of Control pause-save and title 读档.
// Writes only under examples/build/ — never MaoLiAo/gameRecord.dat.

#include "check.h"
#include "save_portable.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

using namespace maoliao;
using maoliao::test::expect;

namespace {

std::string tempPath() {
    return "build/gameRecord.example.dat";
}

void writeRaw(const std::string& path, const std::string& body) {
    std::ofstream out(path, std::ios::trunc);
    out << body;
}

}  // namespace

int main() {
    std::cout << "04_save_record — gameRecord.dat world index\n";

    const std::string path = tempPath();

    expect(worldInRange(1) && worldInRange(2) && worldInRange(3), "1..3 ok");
    expect(!worldInRange(0) && !worldInRange(4) && !worldInRange(-1), "0 and 4 rejected");

    expect(writeWorld(path, 0) == SaveStatus::OutOfRange, "refuse to write 0");
    expect(writeWorld(path, 4) == SaveStatus::OutOfRange, "refuse to write 4");

    for (int w = 1; w <= 3; ++w) {
        expect(writeWorld(path, w) == SaveStatus::Ok, "write world");
        const SaveResult r = readWorld(path);
        expect(r.status == SaveStatus::Ok && r.world == w, "round-trip");
        std::cout << "  round-trip world " << w << "\n";
    }

    // Same bytes as the checked-in MaoLiAo/gameRecord.dat ("3").
    writeRaw(path, "3");
    const SaveResult likeRepo = readWorld(path);
    expect(likeRepo.status == SaveStatus::Ok && likeRepo.world == 3,
           "ASCII '3' matches the repo sample");

    writeRaw(path, "0");
    expect(readWorld(path).status == SaveStatus::OutOfRange, "0 → 存档缺失 path");
    writeRaw(path, "4");
    expect(readWorld(path).status == SaveStatus::OutOfRange, "4 → 存档缺失 path");
    writeRaw(path, "99");
    expect(readWorld(path).status == SaveStatus::OutOfRange, "99 out of range");

    const SaveResult missing = readWorld("build/does-not-exist.dat");
    expect(missing.status == SaveStatus::Missing, "absent file");

    writeRaw(path, "not-an-int");
    expect(readWorld(path).status == SaveStatus::Missing, "garbage body");

    std::remove(path.c_str());
    return test::summary("04_save_record");
}
