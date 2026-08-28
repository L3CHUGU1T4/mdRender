CXX      ?= g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wpedantic
TARGET   := mdrender
SRCS     := src/main.cpp
BUILD    := build
PREFIX   ?= /usr/local

.PHONY: all clean install uninstall

all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(SRCS) src/parser.hpp src/renderer.hpp src/highlighter.hpp src/ansi.hpp src/pager.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

$(BUILD):
	mkdir -p $(BUILD)

install: $(BUILD)/$(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(BUILD)/$(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)

clean:
	rm -rf $(BUILD)
