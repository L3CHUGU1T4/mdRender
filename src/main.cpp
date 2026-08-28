#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include "parser.hpp"
#include "renderer.hpp"
#include "pager.hpp"

static std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f) { std::cerr << "mdrender: cannot open '" << path << "'\n"; std::exit(1); }
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static void usage() {
    std::cerr << "Usage: mdrender [options] <file.md>\n"
              << "  -p    print raw (no pager)\n"
              << "  -h    show this help\n";
    std::exit(1);
}

int main(int argc, char* argv[]) {
    bool pagerMode = isatty(STDOUT_FILENO); // auto-detect: pager only when TTY
    std::string path;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-p")      pagerMode = false;
        else if (arg == "-h") usage();
        else if (arg[0] == '-') { std::cerr << "Unknown flag: " << arg << "\n"; usage(); }
        else                  path = arg;
    }

    if (path.empty()) usage();

    std::string src      = readFile(path);
    auto        blocks   = parse(src);
    std::string rendered = render(blocks);

    if (pagerMode) {
        runPager(rendered);
    } else {
        std::cout << rendered;
    }

    return 0;
}
