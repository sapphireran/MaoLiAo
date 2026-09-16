#include "save.hpp"

#include <fstream>
#include <sstream>

namespace mla {

bool worldInRange(int world) { return world >= 1 && world <= 3; }

bool writeWorld(const std::string& path, int world, std::string& error) {
    std::ofstream out(path.c_str(), std::ios::trunc);
    if (!out) {
        error = "cannot open " + path + " for write";
        return false;
    }
    out << world;
    if (!out) {
        error = "write failed: " + path;
        return false;
    }
    return true;
}

SaveResult readWorld(const std::string& path) {
    SaveResult r;
    std::ifstream in(path.c_str());
    if (!in) {
        r.error = "cannot open " + path;
        return r;
    }
    if (!(in >> r.world)) {
        r.error = "not an integer: " + path;
        return r;
    }
    if (!worldInRange(r.world)) {
        std::ostringstream os;
        os << "world " << r.world << " is outside 1..3 (title shows 存档缺失)";
        r.error = os.str();
        return r;
    }
    r.ok = true;
    return r;
}

}  // namespace mla
