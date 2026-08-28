#pragma once
#include <string>
#include <vector>
#include <sstream>

enum class BlockType {
    H1, H2, H3, H4, H5, H6,
    PARAGRAPH,
    CODE_BLOCK,
    BLOCKQUOTE,
    HR,
    UL_ITEM,
    OL_ITEM,
    TABLE_ROW,
    TABLE_HEADER,
    TABLE_SEPARATOR,
    BLANK,
};

struct Block {
    BlockType   type;
    std::string content;   // raw text
    std::string meta;      // e.g. language for code blocks
    int         level = 0; // indent / list nesting level
};

inline std::vector<Block> parse(const std::string& src) {
    std::vector<Block> blocks;
    std::istringstream stream(src);
    std::string line;

    bool inCodeBlock = false;
    std::string codeLang;
    std::string codeAccum;

    auto pushParagraph = [&](std::string& buf) {
        if (!buf.empty()) {
            // trim trailing newline
            if (!buf.empty() && buf.back() == '\n') buf.pop_back();
            blocks.push_back({BlockType::PARAGRAPH, buf, "", 0});
            buf.clear();
        }
    };

    std::string paragraphBuf;

    while (std::getline(stream, line)) {
        // ── Code fence ──────────────────────────────────────────────────────
        if (line.rfind("```", 0) == 0) {
            if (!inCodeBlock) {
                pushParagraph(paragraphBuf);
                inCodeBlock = true;
                codeLang = line.substr(3);
                // strip trailing whitespace/cr
                while (!codeLang.empty() && (codeLang.back() == '\r' || codeLang.back() == ' '))
                    codeLang.pop_back();
                codeAccum.clear();
            } else {
                blocks.push_back({BlockType::CODE_BLOCK, codeAccum, codeLang, 0});
                inCodeBlock = false;
                codeLang.clear();
                codeAccum.clear();
            }
            continue;
        }

        if (inCodeBlock) {
            codeAccum += line + "\n";
            continue;
        }

        // ── Strip CR ────────────────────────────────────────────────────────
        if (!line.empty() && line.back() == '\r') line.pop_back();

        // ── Blank line ──────────────────────────────────────────────────────
        if (line.empty()) {
            pushParagraph(paragraphBuf);
            blocks.push_back({BlockType::BLANK, "", "", 0});
            continue;
        }

        // ── HR ──────────────────────────────────────────────────────────────
        auto isHR = [](const std::string& l) {
            if (l.size() < 3) return false;
            char c = l[0];
            if (c != '-' && c != '*' && c != '_') return false;
            for (char ch : l) if (ch != c && ch != ' ') return false;
            return true;
        };
        if (isHR(line)) {
            pushParagraph(paragraphBuf);
            blocks.push_back({BlockType::HR, "", "", 0});
            continue;
        }

        // ── ATX Headers ─────────────────────────────────────────────────────
        if (line[0] == '#') {
            pushParagraph(paragraphBuf);
            int level = 0;
            while (level < (int)line.size() && line[level] == '#') ++level;
            std::string content = line.substr(level);
            while (!content.empty() && content[0] == ' ') content = content.substr(1);
            BlockType bt = static_cast<BlockType>(static_cast<int>(BlockType::H1) + std::min(level - 1, 5));
            blocks.push_back({bt, content, "", level});
            continue;
        }

        // ── Setext headers (underline style) ────────────────────────────────
        // peek next line? — we handle them inline by checking previous paragraph
        // (skipped for simplicity — ATX covers 99% of real usage)

        // ── Blockquote ──────────────────────────────────────────────────────
        if (line[0] == '>') {
            pushParagraph(paragraphBuf);
            std::string content = line.substr(1);
            while (!content.empty() && content[0] == ' ') content = content.substr(1);
            blocks.push_back({BlockType::BLOCKQUOTE, content, "", 0});
            continue;
        }

        // ── Unordered list ──────────────────────────────────────────────────
        if ((line[0] == '-' || line[0] == '*' || line[0] == '+') && line.size() > 1 && line[1] == ' ') {
            pushParagraph(paragraphBuf);
            int indent = 0;
            blocks.push_back({BlockType::UL_ITEM, line.substr(2), "", indent});
            continue;
        }
        // Indented list items
        if (line.size() > 2 && (line[0] == ' ' || line[0] == '\t')) {
            size_t i = 0;
            int indent = 0;
            while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) { ++i; ++indent; }
            if (i < line.size() && (line[i] == '-' || line[i] == '*' || line[i] == '+') && i + 1 < line.size() && line[i + 1] == ' ') {
                pushParagraph(paragraphBuf);
                blocks.push_back({BlockType::UL_ITEM, line.substr(i + 2), "", indent / 2});
                continue;
            }
        }

        // ── Ordered list ────────────────────────────────────────────────────
        {
            size_t i = 0;
            while (i < line.size() && std::isdigit(line[i])) ++i;
            if (i > 0 && i < line.size() && (line[i] == '.' || line[i] == ')') && i + 1 < line.size() && line[i + 1] == ' ') {
                pushParagraph(paragraphBuf);
                blocks.push_back({BlockType::OL_ITEM, line.substr(i + 2), line.substr(0, i), 0});
                continue;
            }
        }

        // ── Table ───────────────────────────────────────────────────────────
        if (!line.empty() && line[0] == '|') {
            pushParagraph(paragraphBuf);
            // Detect separator row
            bool isSep = true;
            for (char c : line) if (c != '|' && c != '-' && c != ':' && c != ' ') { isSep = false; break; }
            if (isSep && line.find('-') != std::string::npos) {
                blocks.push_back({BlockType::TABLE_SEPARATOR, line, "", 0});
            } else if (!blocks.empty() && blocks.back().type == BlockType::TABLE_SEPARATOR) {
                blocks.push_back({BlockType::TABLE_ROW, line, "", 0});
            } else {
                // Check if next sibling will be separator — treat as header candidate
                blocks.push_back({BlockType::TABLE_HEADER, line, "", 0});
            }
            continue;
        }

        // ── Paragraph ───────────────────────────────────────────────────────
        if (!paragraphBuf.empty()) paragraphBuf += ' ';
        paragraphBuf += line;
    }

    pushParagraph(paragraphBuf);

    // Retroactively mark table headers: if a TABLE_HEADER is followed by TABLE_SEPARATOR
    for (size_t i = 0; i + 1 < blocks.size(); ++i) {
        if (blocks[i].type == BlockType::TABLE_HEADER && blocks[i + 1].type == BlockType::TABLE_SEPARATOR)
            ; // already correct
        else if (blocks[i].type == BlockType::TABLE_HEADER)
            blocks[i].type = BlockType::TABLE_ROW; // wasn't a real header
    }

    return blocks;
}
