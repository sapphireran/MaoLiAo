#include "save_portable.h"

#include <fstream>

namespace maoliao {

SaveStatus writeWorld(const std::string& path, int world) {
    if (!worldInRange(world)) {
        return SaveStatus::OutOfRange;
    }
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        return SaveStatus::IoError;
    }
    out << world;
    return out ? SaveStatus::Ok : SaveStatus::IoError;
}

SaveResult readWorld(const std::string& path) {
    SaveResult r;
    std::ifstream in(path);
    if (!in) {
        r.status = SaveStatus::Missing;
        return r;
    }
    int flag = 0;
    if (!(in >> flag)) {
        r.status = SaveStatus::Missing;
        return r;
    }
    if (!worldInRange(flag)) {
        r.status = SaveStatus::OutOfRange;
        r.world = flag;
        return r;
    }
    r.status = SaveStatus::Ok;
    r.world = flag;
    return r;
}

}  // namespace maoliao
