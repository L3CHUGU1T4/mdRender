#pragma once
#include <string>

namespace ansi {

const std::string RESET          = "\033[0m";
const std::string BOLD           = "\033[1m";
const std::string DIM            = "\033[2m";
const std::string ITALIC         = "\033[3m";
const std::string UNDERLINE      = "\033[4m";
const std::string STRIKETHROUGH  = "\033[9m";

inline std::string fg(int r, int g, int b) {
    return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

inline std::string bg(int r, int g, int b) {
    return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

namespace color {
    const std::string H1           = fg(139, 233, 253);
    const std::string H2           = fg(80,  200, 220);
    const std::string H3           = fg(50,  170, 200);
    const std::string H4           = fg(30,  140, 180);
    const std::string BOLD_C       = fg(255, 255, 255);
    const std::string ITALIC_C     = fg(200, 200, 255);
    const std::string CODE         = fg(80,  250, 123);
    const std::string LINK         = fg(189, 147, 249);
    const std::string STRIKE       = fg(130, 130, 130);
    const std::string QUOTE        = fg(255, 184, 108);
    const std::string QUOTE_BAR    = fg(200, 120,  60);
    const std::string BULLET       = fg(255, 121, 198);
    const std::string NUMBER       = fg(255, 184, 108);
    const std::string TABLE_BORDER = fg(80,  80,  100);
    const std::string TABLE_HEADER = fg(139, 233, 253);
    const std::string CODE_BG      = bg(40,  42,  54);
    const std::string CODE_LANG    = fg(180, 180, 180);
    const std::string CODE_LINE    = fg(100, 100, 120);
    const std::string HR           = fg(80,  80,  100);
    const std::string SYN_KEYWORD  = fg(255, 121, 198);
    const std::string SYN_STRING   = fg(241, 250, 140);
    const std::string SYN_NUMBER   = fg(189, 147, 249);
    const std::string SYN_COMMENT  = fg(98,  114, 164);
    const std::string SYN_FUNC     = fg(80,  250, 123);
    const std::string SYN_TYPE     = fg(139, 233, 253);
    const std::string SYN_PREPROC  = fg(255, 184, 108);
    const std::string SYN_DEFAULT  = fg(220, 220, 220);
}

} // namespace ansi
