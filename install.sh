#!/usr/bin/env bash
set -euo pipefail

REPO="https://github.com/L3CHUGU1T4/mdRender"
BIN="mdrender"
PREFIX="${PREFIX:-/usr/local}"
INSTALL_DIR="$PREFIX/bin"

echo "==> Installing $BIN..."

# Check compiler
if command -v g++ &>/dev/null; then
    CXX=g++
elif command -v clang++ &>/dev/null; then
    CXX=clang++
else
    echo "Error: no C++ compiler found."
    echo "  macOS: xcode-select --install"
    echo "  Linux: sudo apt install build-essential"
    exit 1
fi

command -v make &>/dev/null || { echo "Error: make not found."; exit 1; }
command -v git  &>/dev/null || { echo "Error: git not found.";  exit 1; }

# Clone into temp dir
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

echo "==> Cloning repository..."
git clone --depth 1 "$REPO" "$TMP/$BIN"

echo "==> Building..."
make -C "$TMP/$BIN" CXX="$CXX" --silent

# Install — try without sudo first
echo "==> Installing to $INSTALL_DIR..."
if [ -w "$INSTALL_DIR" ]; then
    install -m 755 "$TMP/$BIN/build/$BIN" "$INSTALL_DIR/$BIN"
else
    sudo install -m 755 "$TMP/$BIN/build/$BIN" "$INSTALL_DIR/$BIN"
fi

echo ""
echo "  $BIN installed successfully."
echo "  Usage: $BIN <file.md>"
