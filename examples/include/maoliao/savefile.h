#pragma once

#include <string>

namespace maoliao {

enum class SaveError {
    ok,
    missing_file,
    unreadable,
    out_of_range,
    write_failed,
};

struct SaveResult {
    SaveError error = SaveError::ok;
    int world = 0;
};

SaveResult read_save(const std::string& path);
SaveError write_save(const std::string& path, int world);
const char* save_error_name(SaveError e);

}  // namespace maoliao
