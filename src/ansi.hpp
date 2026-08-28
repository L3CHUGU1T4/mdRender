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
    // Headers — purple spectrum
    const std::string H1           = fg(210, 160, 255);  // bright lavender-purple
    const std::string H1_ACCENT    = fg(180, 100, 255);  // deep violet for decorators
    const std::string H2           = fg(190, 130, 250);  // medium purple
    const std::string H2_BAR       = fg(150,  80, 220);  // saturated violet bar
    const std::string H3           = fg(170, 110, 230);  // softer purple
    const std::string H4           = fg(145,  90, 210);  // dim purple

    // Inline elements
    const std::string BOLD_C       = fg(255, 255, 255);
    const std::string ITALIC_C     = fg(210, 190, 255);  // lavender tint
    const std::string CODE         = fg(80,  250, 123);  // green — contrasts well
    const std::string LINK         = fg(189, 147, 249);  // purple link
    const std::string LINK_URL     = fg(110,  80, 160);  // dimmed url
    const std::string STRIKE       = fg(110, 100, 130);

    // Blockquote — warm amber keeps contrast against purple theme
    const std::string QUOTE        = fg(255, 200, 120);
    const std::string QUOTE_BAR    = fg(180, 100, 255);  // purple bar instead of orange

    // Lists
    const std::string BULLET       = fg(200, 130, 255);  // soft purple bullet
    const std::string NUMBER       = fg(210, 160, 255);  // match H1

    // Tables
    const std::string TABLE_BORDER = fg(90,  70, 120);
    const std::string TABLE_HEADER = fg(210, 160, 255);  // match H1

    // Code blocks
    const std::string CODE_BG      = bg(28,  20,  40);   // deep purple-tinted dark bg
    const std::string CODE_BORDER  = fg(100,  70, 150);  // muted purple border
    const std::string CODE_LANG    = fg(160, 120, 210);  // purple-ish lang label
    const std::string CODE_LINE    = fg(90,   70, 120);  // dim line numbers

    // HR
    const std::string HR           = fg(80,   60, 110);

    // Syntax highlighting
    const std::string SYN_KEYWORD  = fg(255, 121, 198);  // pink
    const std::string SYN_STRING   = fg(241, 250, 140);  // yellow
    const std::string SYN_NUMBER   = fg(189, 147, 249);  // purple
    const std::string SYN_COMMENT  = fg(100, 85,  140);  // muted violet
    const std::string SYN_FUNC     = fg(80,  250, 123);  // green
    const std::string SYN_TYPE     = fg(210, 160, 255);  // match headers
    const std::string SYN_PREPROC  = fg(255, 184, 108);  // orange
    const std::string SYN_DEFAULT  = fg(215, 210, 230);  // warm white
}

} // namespace ansi
