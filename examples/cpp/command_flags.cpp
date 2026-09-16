#include "constants.hpp"

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

static const std::pair<int, const char*> kFlags[] = {
    {kCmdLeft, "CMD_LEFT"},   {kCmdRight, "CMD_RIGHT"}, {kCmdUp, "CMD_UP"},
    {kCmdDown, "CMD_DOWN"},   {kCmdShoot, "CMD_SHOOT"}, {kCmdEsc, "CMD_ESC"},
    {kVirReturn, "VIR_RETURN"}, {kVirRestart, "VIR_RESTART"}, {kVirHome, "VIR_HOME"},
};

static std::string upper(std::string s) {
    for (char& c : s) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return s;
}

static int keyBit(const std::string& token) {
    const std::string t = upper(token);
    if (t == "A") return kCmdLeft;
    if (t == "D") return kCmdRight;
    if (t == "W" || t == "K") return kCmdUp;
    if (t == "S") return kCmdDown;
    if (t == "J") return kCmdShoot;
    if (t == "ESC") return kCmdEsc;
    return -1;
}

static int parseKeys(const std::string& chord) {
    int mask = 0;
    std::string cur;
    for (char c : chord) {
        if (c == '+' || c == ',') {
            if (!cur.empty()) {
                const int bit = keyBit(cur);
                if (bit < 0) {
                    std::cerr << "unknown key " << cur << "\n";
                    return -1;
                }
                mask |= bit;
                cur.clear();
            }
        } else if (!std::isspace(static_cast<unsigned char>(c))) {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) {
        const int bit = keyBit(cur);
        if (bit < 0) {
            std::cerr << "unknown key " << cur << "\n";
            return -1;
        }
        mask |= bit;
    }
    return mask;
}

static void printMask(int mask) {
    std::vector<std::string> names;
    int leftover = mask;
    for (const auto& flag : kFlags) {
        if (mask & flag.first) {
            names.emplace_back(flag.second);
            leftover &= ~flag.first;
        }
    }
    std::cout << mask << " = ";
    if (names.empty()) {
        std::cout << "(none)";
    } else {
        for (size_t i = 0; i < names.size(); ++i) {
            if (i) std::cout << " | ";
            std::cout << names[i];
        }
    }
    if (leftover) {
        std::cout << " | unknown:0x" << std::hex << leftover << std::dec;
    }
    std::cout << "\n";
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: command_flags <mask-int|A+W+J>\n";
        return 2;
    }
    const std::string arg = argv[1];
    int mask = 0;
    if (!arg.empty() && (std::isdigit(static_cast<unsigned char>(arg[0])) || arg[0] == '-')) {
        mask = std::stoi(arg, nullptr, 0);
    } else {
        mask = parseKeys(arg);
        if (mask < 0) return 1;
        std::cout << "mask " << mask << " (0x" << std::hex << mask << std::dec << ") from keys "
                  << arg << "\n";
    }
    printMask(mask);
    return 0;
}
