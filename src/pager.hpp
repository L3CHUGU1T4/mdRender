#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Split rendered output into screen lines
inline std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::string cur;
    for (char c : text) {
        if (c == '\n') {
            lines.push_back(cur);
            cur.clear();
        } else {
            cur += c;
        }
    }
    if (!cur.empty()) lines.push_back(cur);
    return lines;
}

inline int termRows() {
    struct winsize w{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row > 5 ? w.ws_row : 24;
}

inline int termCols() {
    struct winsize w{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col > 20 ? w.ws_col : 80;
}

inline char readKey() {
    char c = 0;
    read(STDIN_FILENO, &c, 1);
    if (c == '\033') {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return c;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return c;
        if (seq[0] == '[') {
            if (seq[1] == 'A') return 'k'; // up
            if (seq[1] == 'B') return 'j'; // down
            if (seq[1] == 'H') return 'g'; // home
            if (seq[1] == 'F') return 'G'; // end
            if (seq[1] == '5') { read(STDIN_FILENO, &seq[2], 1); return 'u'; } // page up
            if (seq[1] == '6') { read(STDIN_FILENO, &seq[2], 1); return 'd'; } // page down
        }
    }
    return c;
}

inline void runPager(const std::string& rendered) {
    auto lines = splitLines(rendered);
    int total  = static_cast<int>(lines.size());
    int top    = 0;

    // Raw mode
    struct termios orig{}, raw{};
    tcgetattr(STDIN_FILENO, &orig);
    raw = orig;
    raw.c_lflag &= ~static_cast<unsigned>(ICANON | ECHO);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    // Alternate screen
    std::cout << "\033[?1049h" << std::flush;
    // Hide cursor
    std::cout << "\033[?25l" << std::flush;

    auto draw = [&]() {
        int rows = termRows() - 1; // reserve 1 for status bar
        int cols = termCols();

        std::cout << "\033[H"; // move to top-left
        for (int i = 0; i < rows; ++i) {
            int lineIdx = top + i;
            std::cout << "\033[K"; // clear line
            if (lineIdx < total) std::cout << lines[lineIdx];
            std::cout << "\n";
        }

        // Status bar
        int pct = total > 0 ? (top + rows) * 100 / total : 100;
        if (pct > 100) pct = 100;
        std::string status = " \033[7m mdrender  \033[27m  "
            + std::to_string(top + 1) + "-" + std::to_string(std::min(top + rows, total))
            + "/" + std::to_string(total) + " lines  " + std::to_string(pct) + "%"
            + "  [j/k ↑↓] [d/u PgDn/PgUp] [g/G] [q]uit ";
        // Pad to width
        while ((int)status.size() < cols + 14) status += ' '; // rough pad (ANSI codes inflate size)
        std::cout << "\033[7m" << status << "\033[27m" << std::flush;
    };

    draw();

    while (true) {
        char key = readKey();
        int rows = termRows() - 1;

        if (key == 'q' || key == 'Q') break;
        else if (key == 'j' || key == '\r') top = std::min(top + 1,  std::max(0, total - rows));
        else if (key == 'k')                top = std::max(top - 1,  0);
        else if (key == 'd')                top = std::min(top + rows / 2, std::max(0, total - rows));
        else if (key == 'u')                top = std::max(top - rows / 2, 0);
        else if (key == 'g')                top = 0;
        else if (key == 'G')                top = std::max(0, total - rows);

        draw();
    }

    // Restore
    std::cout << "\033[?25h";   // show cursor
    std::cout << "\033[?1049l"; // main screen
    std::cout << std::flush;
    tcsetattr(STDIN_FILENO, TCSANOW, &orig);
}
