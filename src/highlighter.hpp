#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include <algorithm>
#include "ansi.hpp"

enum class Lang { CPP, C, PYTHON, JS, TS, RUST, GO, JAVA, BASH, JSON, UNKNOWN };

inline Lang detectLang(const std::string& hint) {
    if (hint == "cpp" || hint == "c++") return Lang::CPP;
    if (hint == "c")                    return Lang::C;
    if (hint == "python" || hint == "py") return Lang::PYTHON;
    if (hint == "js" || hint == "javascript") return Lang::JS;
    if (hint == "ts" || hint == "typescript") return Lang::TS;
    if (hint == "rust" || hint == "rs") return Lang::RUST;
    if (hint == "go")                   return Lang::GO;
    if (hint == "java")                 return Lang::JAVA;
    if (hint == "bash" || hint == "sh" || hint == "zsh") return Lang::BASH;
    if (hint == "json")                 return Lang::JSON;
    return Lang::UNKNOWN;
}

struct Token {
    enum class Type { KEYWORD, STRING, NUMBER, COMMENT, FUNC, TYPE, PREPROC, DEFAULT };
    Type type;
    std::string text;
};

inline const std::string& tokenColor(Token::Type t) {
    using namespace ansi::color;
    switch (t) {
        case Token::Type::KEYWORD:  return SYN_KEYWORD;
        case Token::Type::STRING:   return SYN_STRING;
        case Token::Type::NUMBER:   return SYN_NUMBER;
        case Token::Type::COMMENT:  return SYN_COMMENT;
        case Token::Type::FUNC:     return SYN_FUNC;
        case Token::Type::TYPE:     return SYN_TYPE;
        case Token::Type::PREPROC:  return SYN_PREPROC;
        default:                    return SYN_DEFAULT;
    }
}

struct Rule {
    std::regex  pattern;
    Token::Type type;
};

inline std::vector<Rule> rulesFor(Lang lang) {
    using T = Token::Type;
    std::vector<Rule> rules;

    if (lang == Lang::CPP || lang == Lang::C) {
        rules = {
            { std::regex(R"(//[^\n]*)"),  T::COMMENT },
            { std::regex(R"(#\s*\w+)"),   T::PREPROC },
            { std::regex(R"("(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*')"), T::STRING },
            { std::regex(R"(\b(int|long|short|char|void|bool|float|double|unsigned|signed|size_t|auto|const|constexpr|static|inline|extern|volatile|struct|class|enum|union|template|typename|namespace|using|typedef|public|private|protected|virtual|override|final|noexcept|explicit|operator|friend|nullptr|true|false|this|new|delete|return|if|else|for|while|do|switch|case|break|continue|default|goto|sizeof|alignof|decltype|static_cast|dynamic_cast|reinterpret_cast|const_cast|throw|try|catch)\b)"), T::KEYWORD },
            { std::regex(R"(\b([A-Z][a-zA-Z0-9_]*)\b)"), T::TYPE },
            { std::regex(R"(\b([a-zA-Z_]\w*)\s*(?=\())"), T::FUNC },
            { std::regex(R"(\b(0x[0-9a-fA-F]+|\d+\.?\d*([eE][+-]?\d+)?[fFlLuU]*)\b)"), T::NUMBER },
        };
    } else if (lang == Lang::PYTHON) {
        rules = {
            { std::regex(R"(#[^\n]*)"), T::COMMENT },
            { std::regex(R"("(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*')"), T::STRING },
            { std::regex(R"(\b(def|class|import|from|as|return|if|elif|else|for|while|in|not|and|or|is|None|True|False|pass|break|continue|raise|try|except|finally|with|yield|lambda|global|nonlocal|del|assert|async|await)\b)"), T::KEYWORD },
            { std::regex(R"(\b(int|str|float|bool|list|dict|tuple|set|bytes|type|object|super|self|cls)\b)"), T::TYPE },
            { std::regex(R"(\b([a-zA-Z_]\w*)\s*(?=\())"), T::FUNC },
            { std::regex(R"(\b\d+\.?\d*\b)"), T::NUMBER },
        };
    } else if (lang == Lang::JS || lang == Lang::TS) {
        rules = {
            { std::regex(R"(//[^\n]*)"), T::COMMENT },
            { std::regex(R"(`(?:\\.|[^`\\])*`|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*')"), T::STRING },
            { std::regex(R"(\b(const|let|var|function|return|if|else|for|while|do|switch|case|break|continue|default|class|new|this|super|import|export|from|as|async|await|typeof|instanceof|in|of|try|catch|finally|throw|null|undefined|true|false|void|delete|yield)\b)"), T::KEYWORD },
            { std::regex(R"(\b(string|number|boolean|any|void|never|object|symbol|bigint|Array|Promise|Record|Partial|Required|Readonly|Pick|Omit|unknown)\b)"), T::TYPE },
            { std::regex(R"(\b([a-zA-Z_]\w*)\s*(?=\())"), T::FUNC },
            { std::regex(R"(\b\d+\.?\d*\b)"), T::NUMBER },
        };
    } else if (lang == Lang::RUST) {
        rules = {
            { std::regex(R"(//[^\n]*)"), T::COMMENT },
            { std::regex(R"("(?:\\.|[^"\\])*")"), T::STRING },
            { std::regex(R"(\b(fn|let|mut|const|static|struct|enum|impl|trait|use|mod|pub|priv|crate|super|self|Self|type|where|for|in|if|else|match|loop|while|break|continue|return|as|ref|move|async|await|dyn|box|true|false|unsafe|extern|macro_rules)\b)"), T::KEYWORD },
            { std::regex(R"(\b(i8|i16|i32|i64|i128|isize|u8|u16|u32|u64|u128|usize|f32|f64|bool|char|str|String|Vec|Option|Result|Box|Rc|Arc)\b)"), T::TYPE },
            { std::regex(R"(\b([a-zA-Z_]\w*)\s*(?=\())"), T::FUNC },
            { std::regex(R"(\b\d+\.?\d*\b)"), T::NUMBER },
        };
    } else if (lang == Lang::GO) {
        rules = {
            { std::regex(R"(//[^\n]*)"), T::COMMENT },
            { std::regex(R"("(?:\\.|[^"\\])*"|`[^`]*`)"), T::STRING },
            { std::regex(R"(\b(func|var|const|type|struct|interface|map|chan|go|defer|return|if|else|for|range|switch|case|break|continue|default|select|fallthrough|import|package|nil|true|false)\b)"), T::KEYWORD },
            { std::regex(R"(\b(int|int8|int16|int32|int64|uint|uint8|uint16|uint32|uint64|float32|float64|complex64|complex128|bool|string|byte|rune|error)\b)"), T::TYPE },
            { std::regex(R"(\b([a-zA-Z_]\w*)\s*(?=\())"), T::FUNC },
            { std::regex(R"(\b\d+\.?\d*\b)"), T::NUMBER },
        };
    } else if (lang == Lang::BASH) {
        rules = {
            { std::regex(R"(#[^\n]*)"), T::COMMENT },
            { std::regex(R"("(?:\\.|[^"\\])*"|'[^']*')"), T::STRING },
            { std::regex(R"(\b(if|then|else|elif|fi|for|while|do|done|case|esac|function|return|exit|in|echo|export|local|source|alias|unset|shift|set|break|continue|readonly)\b)"), T::KEYWORD },
            { std::regex(R"(\$\{?[a-zA-Z_]\w*\}?)"), T::TYPE },
            { std::regex(R"(\b\d+\b)"), T::NUMBER },
        };
    } else if (lang == Lang::JSON) {
        rules = {
            { std::regex(R"("(?:\\.|[^"\\])*"\s*:)"), T::KEYWORD },
            { std::regex(R"("(?:\\.|[^"\\])*")"), T::STRING },
            { std::regex(R"(\b(true|false|null)\b)"), T::TYPE },
            { std::regex(R"(-?\d+\.?\d*([eE][+-]?\d+)?)"), T::NUMBER },
        };
    }

    return rules;
}

inline std::string highlightLine(const std::string& line, const std::vector<Rule>& rules) {
    if (rules.empty()) return ansi::color::SYN_DEFAULT + line + ansi::RESET;

    struct Span { size_t start, end; Token::Type type; };
    std::vector<Span> spans;
    std::vector<bool> covered(line.size(), false);

    for (auto& rule : rules) {
        auto begin = std::sregex_iterator(line.begin(), line.end(), rule.pattern);
        auto end   = std::sregex_iterator();
        for (auto it = begin; it != end; ++it) {
            auto& m = *it;
            size_t s = static_cast<size_t>(m.position());
            size_t e = s + static_cast<size_t>(m.length());
            bool conflict = false;
            for (size_t i = s; i < e && i < covered.size(); ++i)
                if (covered[i]) { conflict = true; break; }
            if (!conflict) {
                for (size_t i = s; i < e && i < covered.size(); ++i) covered[i] = true;
                spans.push_back({s, e, rule.type});
            }
        }
    }

    std::sort(spans.begin(), spans.end(), [](const Span& a, const Span& b){ return a.start < b.start; });

    std::string result;
    size_t pos = 0;
    for (auto& sp : spans) {
        if (sp.start > pos)
            result += ansi::color::SYN_DEFAULT + line.substr(pos, sp.start - pos) + ansi::RESET;
        result += tokenColor(sp.type) + line.substr(sp.start, sp.end - sp.start) + ansi::RESET;
        pos = sp.end;
    }
    if (pos < line.size())
        result += ansi::color::SYN_DEFAULT + line.substr(pos) + ansi::RESET;
    return result;
}

inline std::string highlightCode(const std::string& code, const std::string& langHint) {
    Lang lang = detectLang(langHint);
    auto rules = rulesFor(lang);

    std::string result;
    std::istringstream stream(code);
    std::string line;
    int lineNum = 1;
    while (std::getline(stream, line)) {
        result += ansi::color::CODE_LINE + std::to_string(lineNum++) + "  " + ansi::RESET;
        result += highlightLine(line, rules) + "\n";
    }
    return result;
}
