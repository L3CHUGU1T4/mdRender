# mdrender — Terminal Markdown Renderer

A **blazing-fast** markdown renderer that runs entirely in your terminal.
No Electron, no browser — just pure C++17 and ANSI escape codes.

## Features

- Custom markdown parser (zero dependencies)
- True-color ANSI rendering
- Syntax highlighting inside code blocks
- Built-in pager with vim keybindings

## Installation

```bash
git clone https://github.com/L3CHUGU1T4/mdRender.git
cd mdrender
make && make install
```

## Usage

```bash
mdrender README.md       # open with pager
mdrender -p README.md    # pipe / print raw
```

## Supported Elements

### Inline formatting

You can use **bold**, _italic_, ~~strikethrough~~, and `inline code`.
Links look like [GitHub](https://github.com) in the output.

### Code blocks with syntax highlighting

```cpp
#include <iostream>

int main(int argc, char* argv[]) {
    // greet the world
    std::string name = argv[1] ? argv[1] : "world";
    std::cout << "Hello, " << name << "!\n";
    return 0;
}
```

```python
def fibonacci(n: int) -> list[int]:
    """Return the first n Fibonacci numbers."""
    a, b = 0, 1
    result = []
    for _ in range(n):
        result.append(a)
        a, b = b, a + b
    return result
```

### Tables

| Language | Paradigm   | Typed   |
| -------- | ---------- | ------- |
| C++      | Multi      | Static  |
| Python   | Multi      | Dynamic |
| Rust     | Systems    | Static  |
| Go       | Concurrent | Static  |

### Blockquotes

> The best programs are the ones written when the programmer is supposed to be doing something else.
> — Melinda Varian

### Lists

Ordered:

1. Parse the markdown source
2. Build an AST of blocks
3. Render each block with ANSI codes
4. Page the output

Nested unordered:

- Rendering
  - Headers (H1–H6)
  - Paragraphs
  - Code blocks
- Navigation
  - j / k — scroll line by line
  - d / u — half page
  - g / G — top / bottom
  - q — quit

---

## Architecture

### Parser

The parser is a **single-pass** state machine that reads line by line.
It outputs a flat list of `Block` structs — no heap-allocated tree required.

### Renderer

Each block type maps to an ANSI rendering function.
Inline elements (bold, italic, links) are handled by a small recursive-descent pass.

### Pager

The pager uses `termios` raw mode and an **alternate screen buffer** so it never
corrupts your scrollback history. Resize-safe via `TIOCGWINSZ`.

---

_Built with C++17. No dependencies. MIT License._
