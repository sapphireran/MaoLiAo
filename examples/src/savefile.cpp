#include "maoliao/savefile.h"

#include <fstream>

namespace maoliao {

const char* save_error_name(SaveError e) {
    switch (e) {
        case SaveError::ok:
            return "ok";
        case SaveError::missing_file:
            return "missing_file";
        case SaveError::unreadable:
            return "unreadable";
        case SaveError::out_of_range:
            return "out_of_range";
        case SaveError::write_failed:
            return "write_failed";
    }
    return "unknown";
}

SaveResult read_save(const std::string& path) {
    SaveResult r;
    std::ifstream in(path);
    if (!in) {
        r.error = SaveError::missing_file;
        return r;
    }
    int world = 0;
    in >> world;
    if (!in) {
        r.error = SaveError::unreadable;
        return r;
    }
    if (world < 1 || world > 3) {
        r.error = SaveError::out_of_range;
        r.world = world;
        return r;
    }
    r.world = world;
    r.error = SaveError::ok;
    return r;
}

SaveError write_save(const std::string& path, int world) {
    if (world < 1 || world > 3) {
        return SaveError::out_of_range;
    }
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        return SaveError::write_failed;
    }
    out << world;
    if (!out) {
        return SaveError::write_failed;
    }
    return SaveError::ok;
}

}  // namespace maoliao
