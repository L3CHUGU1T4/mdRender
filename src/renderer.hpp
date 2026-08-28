#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <sys/ioctl.h>
#include "ansi.hpp"
#include "parser.hpp"
#include "highlighter.hpp"

// Repeat a UTF-8 string n times
inline std::string rep(const std::string& s, int n) {
    std::string r;
    if (n <= 0) return r;
    r.reserve(s.size() * static_cast<size_t>(n));
    for (int i = 0; i < n; ++i) r += s;
    return r;
}

inline int termWidth() {
    struct winsize w{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col > 20 ? w.ws_col : 80;
}

// ── Inline formatting ────────────────────────────────────────────────────────

inline std::string renderInline(const std::string& text) {
    std::string result;
    size_t i = 0;
    const size_t n = text.size();

    while (i < n) {
        // Bold + italic ***text***
        if (i + 2 < n && text[i] == '*' && text[i+1] == '*' && text[i+2] == '*') {
            size_t end = text.find("***", i + 3);
            if (end != std::string::npos) {
                result += ansi::BOLD + ansi::ITALIC + ansi::color::BOLD_C
                        + text.substr(i + 3, end - i - 3) + ansi::RESET;
                i = end + 3; continue;
            }
        }
        // Bold **text**
        if (i + 1 < n && text[i] == '*' && text[i+1] == '*') {
            size_t end = text.find("**", i + 2);
            if (end != std::string::npos) {
                result += ansi::BOLD + ansi::color::BOLD_C
                        + text.substr(i + 2, end - i - 2) + ansi::RESET;
                i = end + 2; continue;
            }
        }
        // Italic *text* or _text_
        if ((text[i] == '*' || text[i] == '_') && (i == 0 || text[i-1] == ' ')) {
            char delim = text[i];
            size_t end = text.find(delim, i + 1);
            if (end != std::string::npos) {
                result += ansi::ITALIC + ansi::color::ITALIC_C
                        + text.substr(i + 1, end - i - 1) + ansi::RESET;
                i = end + 1; continue;
            }
        }
        // Strikethrough ~~text~~
        if (i + 1 < n && text[i] == '~' && text[i+1] == '~') {
            size_t end = text.find("~~", i + 2);
            if (end != std::string::npos) {
                result += ansi::STRIKETHROUGH + ansi::color::STRIKE
                        + text.substr(i + 2, end - i - 2) + ansi::RESET;
                i = end + 2; continue;
            }
        }
        // Inline code `code`
        if (text[i] == '`') {
            size_t end = text.find('`', i + 1);
            if (end != std::string::npos) {
                result += ansi::color::CODE + "`"
                        + text.substr(i + 1, end - i - 1) + "`" + ansi::RESET;
                i = end + 1; continue;
            }
        }
        // Link [text](url)
        if (text[i] == '[') {
            size_t mid = text.find("](", i + 1);
            if (mid != std::string::npos) {
                size_t end = text.find(')', mid + 2);
                if (end != std::string::npos) {
                    std::string linkText = text.substr(i + 1, mid - i - 1);
                    std::string url      = text.substr(mid + 2, end - mid - 2);
                    result += ansi::UNDERLINE + ansi::color::LINK + linkText + ansi::RESET
                            + ansi::color::LINK_URL + " (" + url + ")" + ansi::RESET;
                    i = end + 1; continue;
                }
            }
        }
        result += text[i++];
    }
    return result;
}

// ── Table cell splitter ──────────────────────────────────────────────────────

inline std::vector<std::string> splitCells(const std::string& row) {
    std::vector<std::string> cells;
    std::string cell;
    bool first = true;
    for (size_t i = 0; i < row.size(); ++i) {
        if (row[i] == '|') {
            if (first) { first = false; continue; }
            size_t s = cell.find_first_not_of(' ');
            size_t e = cell.find_last_not_of(' ');
            cells.push_back(s == std::string::npos ? "" : cell.substr(s, e - s + 1));
            cell.clear();
        } else {
            cell += row[i];
        }
    }
    if (!cell.empty()) {
        size_t s = cell.find_first_not_of(' ');
        size_t e = cell.find_last_not_of(' ');
        if (s != std::string::npos) cells.push_back(cell.substr(s, e - s + 1));
    }
    return cells;
}

// ── Block renderer ───────────────────────────────────────────────────────────

inline std::string render(const std::vector<Block>& blocks) {
    const int width = termWidth();
    std::string out;
    bool lastWasBlank = false;
    int  olCounter    = 1;

    // UTF-8 box / decoration bytes
    // ═  \xe2\x95\x90    ─  \xe2\x94\x80    ━  \xe2\x94\x81
    // ◆  \xe2\x97\x86    ◇  \xe2\x97\x87    ◈  \xe2\x97\x88
    // ▌  \xe2\x96\x8c    ▐  \xe2\x96\x90    ▎  \xe2\x96\x8e
    // ▸  \xe2\x96\xb8    ◦  \xe2\x97\xa6    ●  \xe2\x97\x8f
    // ┃  \xe2\x94\x83    │  \xe2\x94\x82
    // ╸  \xe2\x95\xb8    ╺  \xe2\x95\xba

    for (size_t idx = 0; idx < blocks.size(); ++idx) {
        const Block& b = blocks[idx];

        switch (b.type) {

        case BlockType::BLANK:
            if (!lastWasBlank) out += "\n";
            lastWasBlank = true;
            continue;

        // ── HR ──────────────────────────────────────────────────────────────
        case BlockType::HR:
            out += "\n"
                 + ansi::color::H1_ACCENT + "  \xe2\x97\x86" + ansi::RESET
                 + ansi::color::HR + rep("\xe2\x94\x80", width - 6) + ansi::RESET
                 + ansi::color::H1_ACCENT + "\xe2\x97\x86" + ansi::RESET + "\n\n";
            break;

        // ── H1 ──────────────────────────────────────────────────────────────
        case BlockType::H1: {
            // ◆━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━◆
            //   ◆  Title Text
            // ◆━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━◆
            int lineW = width - 2;
            out += "\n";
            out += ansi::color::H1_ACCENT + "  \xe2\x97\x86"
                 + rep("\xe2\x94\x81", lineW - 2)
                 + "\xe2\x97\x86" + ansi::RESET + "\n";
            out += ansi::BOLD + ansi::color::H1 + "  \xe2\x97\x86  " + ansi::RESET
                 + ansi::BOLD + ansi::color::H1 + b.content + ansi::RESET + "\n";
            out += ansi::color::H1_ACCENT + "  \xe2\x97\x86"
                 + rep("\xe2\x94\x81", lineW - 2)
                 + "\xe2\x97\x86" + ansi::RESET + "\n\n";
            break;
        }

        // ── H2 ──────────────────────────────────────────────────────────────
        case BlockType::H2: {
            // ▐░ Section Title
            // ━━━━━━━━━━━━━━━━
            int underLen = std::min((int)b.content.size() + 6, width - 2);
            out += "\n";
            out += ansi::color::H2_BAR + "  \xe2\x96\x90" + ansi::RESET
                 + ansi::color::H2_BAR + "\xe2\x96\x8c " + ansi::RESET
                 + ansi::BOLD + ansi::color::H2 + b.content + ansi::RESET + "\n";
            out += "  " + ansi::color::H2_BAR + rep("\xe2\x94\x81", underLen) + ansi::RESET + "\n\n";
            break;
        }

        // ── H3 ──────────────────────────────────────────────────────────────
        case BlockType::H3:
            out += "\n  " + ansi::color::H3 + "\xe2\x97\x88 " + ansi::RESET
                 + ansi::BOLD + ansi::color::H3 + b.content + ansi::RESET
                 + "  " + ansi::color::H3 + ansi::DIM + rep("\xe2\x94\x80", 20) + ansi::RESET + "\n\n";
            break;

        // ── H4 ──────────────────────────────────────────────────────────────
        case BlockType::H4:
            out += "    " + ansi::color::H4 + "\xe2\x95\xb8 " + ansi::RESET
                 + ansi::BOLD + ansi::color::H4 + b.content + ansi::RESET + "\n";
            break;

        case BlockType::H5:
        case BlockType::H6:
            out += "      " + ansi::color::H4 + ansi::DIM + "\xe2\x80\xa2 "
                 + b.content + ansi::RESET + "\n";
            break;

        // ── Paragraph ────────────────────────────────────────────────────────
        case BlockType::PARAGRAPH:
            out += "  " + renderInline(b.content) + "\n";
            break;

        // ── Code block ──────────────────────────────────────────────────────
        case BlockType::CODE_BLOCK: {
            out += "\n";
            std::string langLabel = b.meta.empty() ? "code" : b.meta;

            // top: "  ╭─ lang ────────────────────────────────╮"
            std::string topPrefix = "  \xe2\x94\x8c\xe2\x94\x80 " + langLabel + " ";
            int fillLen = width - 2 - (int)langLabel.size() - 5;
            out += ansi::color::CODE_BORDER + topPrefix;
            if (fillLen > 0) out += rep("\xe2\x94\x80", fillLen);
            out += "\xe2\x94\x90" + ansi::RESET + "\n";

            // lines
            std::istringstream cs(b.content);
            std::string codeLine;
            auto rules = rulesFor(detectLang(b.meta));
            int lineNum = 1;
            while (std::getline(cs, codeLine)) {
                if (!codeLine.empty() && codeLine.back() == '\r') codeLine.pop_back();
                std::string numStr = std::to_string(lineNum++);
                while ((int)numStr.size() < 3) numStr = " " + numStr;
                out += ansi::color::CODE_BG
                     + "  \xe2\x94\x82" + ansi::color::CODE_LINE + numStr + "  " + ansi::RESET
                     + ansi::color::CODE_BG + highlightLine(codeLine, rules) + ansi::RESET + "\n";
            }

            // bottom
            out += ansi::color::CODE_BORDER + "  \xe2\x94\x94"
                 + rep("\xe2\x94\x80", width - 3) + "\xe2\x94\x98" + ansi::RESET + "\n\n";
            break;
        }

        // ── Blockquote ───────────────────────────────────────────────────────
        case BlockType::BLOCKQUOTE:
            out += ansi::color::QUOTE_BAR + "  \xe2\x94\x83 " + ansi::RESET
                 + ansi::ITALIC + ansi::color::QUOTE + renderInline(b.content) + ansi::RESET + "\n";
            break;

        // ── Unordered list ───────────────────────────────────────────────────
        case BlockType::UL_ITEM: {
            std::string indent(static_cast<size_t>(b.level) * 3 + 2, ' ');
            // ◆ level0  ◇ level1  ╸ level2+
            const char* bullet = b.level == 0 ? "\xe2\x97\x86"   // ◆
                               : b.level == 1 ? "\xe2\x97\x87"   // ◇
                               :                "\xe2\x95\xb8";  // ╸
            out += indent + ansi::color::BULLET + bullet + ansi::RESET
                 + " " + renderInline(b.content) + "\n";
            break;
        }

        // ── Ordered list ─────────────────────────────────────────────────────
        case BlockType::OL_ITEM:
            if (idx == 0 || blocks[idx-1].type != BlockType::OL_ITEM) olCounter = 1;
            out += "  " + ansi::color::NUMBER + "\xe2\x97\x86 " + std::to_string(olCounter++) + "." + ansi::RESET
                 + " " + renderInline(b.content) + "\n";
            break;

        // ── Table ─────────────────────────────────────────────────────────
        case BlockType::TABLE_HEADER: {
            auto cells = splitCells(b.content);
            out += "\n  " + ansi::color::TABLE_BORDER + "\xe2\x94\x82" + ansi::RESET;
            for (auto& c : cells)
                out += " " + ansi::BOLD + ansi::color::TABLE_HEADER + c + ansi::RESET
                     + " " + ansi::color::TABLE_BORDER + "\xe2\x94\x82" + ansi::RESET;
            out += "\n";
            break;
        }
        case BlockType::TABLE_SEPARATOR: {
            auto cells = splitCells(b.content);
            out += "  " + ansi::color::TABLE_BORDER + "\xe2\x94\x9c";
            for (size_t ci = 0; ci < cells.size(); ++ci) {
                out += rep("\xe2\x94\x80", (int)cells[ci].size() + 2);
                out += (ci + 1 < cells.size()) ? "\xe2\x94\xbc" : "\xe2\x94\xa4";
            }
            out += ansi::RESET + "\n";
            break;
        }
        case BlockType::TABLE_ROW: {
            auto cells = splitCells(b.content);
            out += "  " + ansi::color::TABLE_BORDER + "\xe2\x94\x82" + ansi::RESET;
            for (auto& c : cells)
                out += " " + renderInline(c) + " " + ansi::color::TABLE_BORDER + "\xe2\x94\x82" + ansi::RESET;
            out += "\n";
            break;
        }

        default: break;
        }

        lastWasBlank = false;
    }

    return out;
}
