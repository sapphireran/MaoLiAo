#pragma once

#include <string>

namespace maoliao {

enum class SaveStatus {
    Ok,
    MissingFile,
    Unreadable,
    OutOfRange,
};

struct SaveResult {
    SaveStatus status;
    int world;  // 1–3 when status == Ok
};

// Title-screen rule: 1 <= world <= 3. Matches control.cpp 读档.
SaveResult parseSaveText(const std::string& text);
SaveResult parseSaveFile(const std::string& path);
std::string formatSave(int world);  // fprintf_s "%d" — no trailing newline

}  // namespace maoliao
