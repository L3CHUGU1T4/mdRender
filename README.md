# mdrender

A terminal Markdown renderer built from scratch in C++17. No dependencies — just ANSI true-color, Unicode box-drawing, syntax highlighting for 10 languages, and a built-in vim-key pager.

![demo](assets/demo.gif)

---

## Features

- **From-scratch parser** — custom single-pass state machine, no third-party libs
- **True-color rendering** — headers, lists, blockquotes, tables, inline formatting
- **Syntax highlighting** — C/C++, Python, JS/TS, Rust, Go, Java, Bash, JSON
- **Built-in pager** — alternate screen buffer, vim keybindings, never pollutes scrollback
- **Zero dependencies** — only the C++17 standard library

---

## Demo

| Element | Rendered |
|---|---|
| Headers H1–H6 | Hierarchical purple palette with decorators |
| Code blocks | Language-aware syntax highlighting with line numbers |
| Tables | Aligned columns with full box borders |
| Blockquotes | Purple bar with muted italic text |
| Bold / Italic / Strike | Full inline formatting |
| Links | Underlined with dimmed URL |

---

## Installation

### Homebrew (macOS)

```bash
brew tap L3CHUGU1T4/tap
brew install mdrender
```

### curl install (macOS / Linux)

```bash
curl -fsSL https://raw.githubusercontent.com/L3CHUGU1T4/mdRender/main/install.sh | bash
```

### Build from source

```bash
git clone https://github.com/L3CHUGU1T4/mdRender
cd mdRender
make
sudo make install   # installs to /usr/local/bin
```

**Requirements:** C++17 compiler (g++ or clang++), make

---

## Usage

```bash
mdrender file.md          # open with pager
mdrender -p file.md       # print raw (pipe-friendly)
mdrender README.md | less # works with any pager
```

### Pager keybindings

| Key | Action |
|---|---|
| `j` / `↓` | Scroll down one line |
| `k` / `↑` | Scroll up one line |
| `d` / `PgDn` | Half page down |
| `u` / `PgUp` | Half page up |
| `g` | Go to top |
| `G` | Go to bottom |
| `q` | Quit |

---

## Terminal compatibility

Requires true-color (24-bit) and UTF-8 support. Works with:

- iTerm2, Alacritty, Kitty, WezTerm
- macOS Terminal.app (macOS 10.14+)
- GNOME Terminal, Windows Terminal
- tmux with `set -g default-terminal "xterm-256color"`

---

## Architecture

The project is split into four self-contained headers:

| File | Role |
|---|---|
| `src/parser.hpp` | Single-pass line-by-line block parser |
| `src/renderer.hpp` | ANSI renderer for every block and inline element |
| `src/highlighter.hpp` | Regex-based tokenizer for 10 languages |
| `src/pager.hpp` | `termios` raw-mode pager with alternate screen buffer |

---

## License

MIT
