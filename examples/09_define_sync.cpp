// 09_define_sync — keep portable constants honest against define.h.
//
// Reads MaoLiAo/define.h (the EasyX header) and checks that the
// numbers copied into maoliao_core.hpp still match. Also flags the
// known MYDIFINE / trailing-semicolon bugs so they stay documented.

#include "maoliao_core.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace {

int g_failed = 0;

void expect(bool ok, const char* name)
{
    std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) {
        ++g_failed;
    }
}

void expect_eq_i(int got, int want, const char* name)
{
    const bool ok = got == want;
    std::printf("  [%s] %s  got=%d want=%d\n", ok ? "PASS" : "FAIL", name, got,
                want);
    if (!ok) {
        ++g_failed;
    }
}

void expect_near(double got, double want, double eps, const char* name)
{
    const bool ok = std::fabs(got - want) <= eps;
    std::printf("  [%s] %s  got=%g want=%g\n", ok ? "PASS" : "FAIL", name, got,
                want);
    if (!ok) {
        ++g_failed;
    }
}

const char* first_existing(const char* const* paths)
{
    for (int i = 0; paths[i] != 0; ++i) {
        FILE* fp = std::fopen(paths[i], "r");
        if (fp) {
            std::fclose(fp);
            return paths[i];
        }
    }
    return 0;
}

std::string strip_comment(const std::string& line)
{
    const std::size_t pos = line.find("//");
    if (pos == std::string::npos) {
        return line;
    }
    return line.substr(0, pos);
}

bool is_ws(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool parse_define(const std::string& line, std::string& name, std::string& value)
{
    const char* s = line.c_str();
    while (is_ws(*s)) {
        ++s;
    }
    if (std::strncmp(s, "#define", 7) != 0) {
        return false;
    }
    s += 7;
    while (is_ws(*s)) {
        ++s;
    }
    const char* n0 = s;
    while (*s && !is_ws(*s) && *s != '(') {
        ++s;
    }
    if (s == n0) {
        return false;
    }
    name.assign(n0, s);
    if (*s == '(') {
        // function-style macro (max, random) — not a constant.
        return false;
    }
    while (is_ws(*s)) {
        ++s;
    }
    value = s;
    while (!value.empty() && is_ws(value[value.size() - 1])) {
        value.erase(value.size() - 1);
    }
    return true;
}

// Evaluate the handful of define.h right-hand sides we care about.
// Expressions use only the macros already parsed into a tiny table.
enum { kMaxDefs = 64 };

struct Def {
    char name[48];
    double value;
    bool numeric;
};

Def g_defs[kMaxDefs];
int g_ndefs = 0;

const Def* find_def(const char* name)
{
    for (int i = 0; i < g_ndefs; ++i) {
        if (std::strcmp(g_defs[i].name, name) == 0) {
            return &g_defs[i];
        }
    }
    return 0;
}

bool eval_simple(const std::string& expr, double& out)
{
    // Accept: N, NAME, NAME*N, N*NAME, NAME*NAME, N*NAME+N, (N*NAME+N)
    std::string e = expr;
    if (!e.empty() && e[e.size() - 1] == ';') {
        e.erase(e.size() - 1);
    }
    // Drop spaces.
    std::string compact;
    for (std::size_t i = 0; i < e.size(); ++i) {
        if (!is_ws(e[i]) && e[i] != '(' && e[i] != ')') {
            compact.push_back(e[i]);
        }
    }
    if (compact.empty()) {
        return false;
    }

    // Single identifier?
    if (find_def(compact.c_str()) && find_def(compact.c_str())->numeric) {
        out = find_def(compact.c_str())->value;
        return true;
    }

    // Try strtod of the whole thing.
    char* end = 0;
    out = std::strtod(compact.c_str(), &end);
    if (end && *end == 0) {
        return true;
    }

    // NAME*N or N*NAME or NAME*NAME [+N]
    std::string left;
    std::string right;
    char op = 0;
    std::string rest;
    for (std::size_t i = 0; i < compact.size(); ++i) {
        if (compact[i] == '*' || compact[i] == '/') {
            left = compact.substr(0, i);
            op = compact[i];
            std::size_t j = i + 1;
            while (j < compact.size() && compact[j] != '+' && compact[j] != '-') {
                ++j;
            }
            right = compact.substr(i + 1, j - (i + 1));
            rest = compact.substr(j);
            break;
        }
    }
    if (op == 0) {
        return false;
    }

    double a = 0.0;
    double b = 0.0;
    const Def* da = find_def(left.c_str());
    const Def* db = find_def(right.c_str());
    if (da && da->numeric) {
        a = da->value;
    } else {
        char* e1 = 0;
        a = std::strtod(left.c_str(), &e1);
        if (e1 == left.c_str() || *e1 != 0) {
            return false;
        }
    }
    if (db && db->numeric) {
        b = db->value;
    } else {
        char* e2 = 0;
        b = std::strtod(right.c_str(), &e2);
        if (e2 == right.c_str() || *e2 != 0) {
            return false;
        }
    }
    out = (op == '*') ? (a * b) : (a / b);
    if (!rest.empty()) {
        char sign = rest[0];
        const Def* dc = find_def(rest.c_str() + 1);
        double c = 0.0;
        if (dc && dc->numeric) {
            c = dc->value;
        } else {
            char* e3 = 0;
            c = std::strtod(rest.c_str() + 1, &e3);
            if (!e3 || *e3 != 0) {
                return false;
            }
        }
        out = (sign == '-') ? (out - c) : (out + c);
    }
    return true;
}

bool load_define_h(const char* path)
{
    FILE* fp = std::fopen(path, "r");
    if (!fp) {
        return false;
    }
    char buf[512];
    g_ndefs = 0;
    while (std::fgets(buf, sizeof(buf), fp)) {
        std::string name;
        std::string value;
        if (!parse_define(strip_comment(buf), name, value)) {
            continue;
        }
        if (g_ndefs >= kMaxDefs) {
            break;
        }
        std::strncpy(g_defs[g_ndefs].name, name.c_str(), 47);
        g_defs[g_ndefs].name[47] = 0;
        double v = 0.0;
        g_defs[g_ndefs].numeric = eval_simple(value, v);
        g_defs[g_ndefs].value = v;
        ++g_ndefs;
        // define.h lists WIDTH before X0 and TIME before F, so
        // dependents evaluate against earlier rows in this same pass.
    }
    std::fclose(fp);
    return true;
}

double must(const char* name)
{
    const Def* d = find_def(name);
    return (d && d->numeric) ? d->value : 1e300;
}

bool has_name(const char* name)
{
    return find_def(name) != 0;
}

}  // namespace

int main(int argc, char** argv)
{
    std::printf("09_define_sync\n");

    const char* paths[] = {
        (argc > 1) ? argv[1] : "../MaoLiAo/define.h",
        "../MaoLiAo/define.h",
        "../../MaoLiAo/define.h",
        "MaoLiAo/define.h",
        0,
    };
    const char* path = first_existing(paths);
    expect(path != 0, "found MaoLiAo/define.h");
    if (!path) {
        std::printf("09_define_sync: %d failed\n", g_failed);
        return 1;
    }
    std::printf("  reading %s\n", path);

    FILE* fp = std::fopen(path, "r");
    expect(fp != 0, "opened define.h");
    if (!fp) {
        std::printf("09_define_sync: %d failed\n", g_failed);
        return 1;
    }

    char raw[8192];
    const std::size_t nread = std::fread(raw, 1, sizeof(raw) - 1, fp);
    raw[nread] = 0;
    std::fclose(fp);

    // Documented bugs must still be present in the personal game header.
    expect(std::strstr(raw, "#define MYDIFINE") != 0,
           "include guard still misspelled MYDIFINE");
    expect(std::strstr(raw, "#define LIFE 5;") != 0,
           "LIFE still has a trailing semicolon");
    expect(std::strstr(raw, "#define F TIME*0.3;") != 0,
           "F still has a trailing semicolon");

    if (!load_define_h(path)) {
        expect(false, "parsed define.h");
        std::printf("09_define_sync: %d failed\n", g_failed);
        return 1;
    }

    expect_eq_i(static_cast<int>(must("XSIZE")), maoliao::kXSize, "XSIZE");
    expect_eq_i(static_cast<int>(must("YSIZE")), maoliao::kYSize, "YSIZE");
    expect_eq_i(static_cast<int>(must("WIDTH")), maoliao::kWidth, "WIDTH");
    expect_eq_i(static_cast<int>(must("HEIGHT")), maoliao::kHeight, "HEIGHT");
    expect_eq_i(static_cast<int>(must("X0")), maoliao::kX0, "X0");
    expect_eq_i(static_cast<int>(must("Y0")), maoliao::kY0, "Y0");
    expect_near(must("TIME"), maoliao::kTime, 1e-12, "TIME");
    expect_eq_i(static_cast<int>(must("STEP")), maoliao::kStep, "STEP");
    expect_eq_i(static_cast<int>(must("ENEMY_STEP")), maoliao::kEnemyStep,
                "ENEMY_STEP");
    expect_eq_i(static_cast<int>(must("K_MAP_BG")), maoliao::kMapBg, "K_MAP_BG");
    expect_eq_i(static_cast<int>(must("XLEFT")), maoliao::kXLeft, "XLEFT");
    expect_eq_i(static_cast<int>(must("XRIGHT")), maoliao::kXRight, "XRIGHT");
    expect_near(must("REAL_HEIGHT"), maoliao::kRealHeight, 1e-12, "REAL_HEIGHT");
    expect_near(must("UNREAL_HEIGHT"), maoliao::kUnrealHeight, 1e-12,
                "UNREAL_HEIGHT");
    expect_near(must("G"), maoliao::kG, 1e-12, "G");
    expect_near(must("V_MAX"), maoliao::kVMax, 1e-12, "V_MAX");
    expect_near(must("A_ROLE"), maoliao::kARole, 1e-12, "A_ROLE");
    expect_near(must("T1"), maoliao::kT1, 1e-12, "T1");
    expect_near(must("T2"), maoliao::kT2, 1e-12, "T2");
    expect_near(must("T3"), maoliao::kT3, 1e-12, "T3");
    expect_near(must("LIFE"), static_cast<double>(maoliao::kLife), 1e-12,
                "LIFE (semicolon stripped)");
    expect_near(must("F"), maoliao::kSceneryRate, 1e-12, "F = TIME*0.3");

    expect_eq_i(static_cast<int>(must("CMD_LEFT")), maoliao::kCmdLeft, "CMD_LEFT");
    expect_eq_i(static_cast<int>(must("CMD_RIGHT")), maoliao::kCmdRight,
                "CMD_RIGHT");
    expect_eq_i(static_cast<int>(must("CMD_UP")), maoliao::kCmdUp, "CMD_UP");
    expect_eq_i(static_cast<int>(must("CMD_DOWN")), maoliao::kCmdDown, "CMD_DOWN");
    expect_eq_i(static_cast<int>(must("CMD_SHOOT")), maoliao::kCmdShoot,
                "CMD_SHOOT");
    expect_eq_i(static_cast<int>(must("CMD_ESC")), maoliao::kCmdEsc, "CMD_ESC");
    expect_eq_i(static_cast<int>(must("VIR_RETURN")), maoliao::kVirReturn,
                "VIR_RETURN");
    expect_eq_i(static_cast<int>(must("VIR_RESTART")), maoliao::kVirRestart,
                "VIR_RESTART");
    expect_eq_i(static_cast<int>(must("VIR_HOME")), maoliao::kVirHome, "VIR_HOME");

    expect(has_name("MYDIFINE") && !has_name("MYDEFINE"),
           "guard token is MYDIFINE, not MYDEFINE");

    if (g_failed != 0) {
        std::printf("09_define_sync: %d failed\n", g_failed);
        return 1;
    }
    std::printf("09_define_sync: all passed\n");
    return 0;
}
