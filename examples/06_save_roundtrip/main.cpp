#include "check.hpp"
#include "save.hpp"

#include <cstdio>
#include <fstream>
#include <string>

#ifdef _WIN32
#include <direct.h>
#define MLA_MKDIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#define MLA_MKDIR(p) mkdir((p), 0755)
#endif

static std::string writeRaw(const std::string& path, const std::string& body) {
    std::ofstream out(path.c_str(), std::ios::trunc);
    out << body;
    return path;
}

int main() {
    std::printf("=== 06 save roundtrip ===\n");
    MLA_MKDIR("build");
    MLA_MKDIR("build/tmp");

    const std::string okPath = "build/tmp/gameRecord.ok.dat";
    const std::string badPath = "build/tmp/gameRecord.bad.dat";
    const std::string missPath = "build/tmp/gameRecord.missing.dat";
    const std::string emptyPath = "build/tmp/gameRecord.empty.dat";

    mla::Checks c;
    std::string err;
    c.expect(mla::writeWorld(okPath, 2, err), "write world 2");
    const mla::SaveResult ok = mla::readWorld(okPath);
    c.expect(ok.ok && ok.world == 2, "read back world 2");
    std::printf("  file %s -> world=%d ok=%d\n", okPath.c_str(), ok.world,
                ok.ok ? 1 : 0);

    writeRaw(badPath, "9");
    const mla::SaveResult bad = mla::readWorld(badPath);
    c.expect(!bad.ok, "world 9 rejected (title 存档缺失)");
    std::printf("  reject: %s\n", bad.error.c_str());

    writeRaw(emptyPath, "");
    const mla::SaveResult empty = mla::readWorld(emptyPath);
    c.expect(!empty.ok, "empty file rejected");

    const mla::SaveResult miss = mla::readWorld(missPath);
    c.expect(!miss.ok, "missing file rejected");

    c.expect(mla::worldInRange(1) && mla::worldInRange(3) &&
                 !mla::worldInRange(0) && !mla::worldInRange(4),
             "range is 1..3 inclusive");

    // Same payload as Control::pauseClick fprintf("%d", world).
    c.expect(mla::writeWorld(okPath, 3, err), "overwrite with world 3");
    const mla::SaveResult w3 = mla::readWorld(okPath);
    c.expect(w3.ok && w3.world == 3, "roundtrip world 3");
    return c.finish("06_save_roundtrip");
}
