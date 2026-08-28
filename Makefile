CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wpedantic
TARGET   := mdrender
SRCS     := src/main.cpp
BUILD    := build

.PHONY: all clean install

all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(SRCS) src/parser.hpp src/renderer.hpp src/highlighter.hpp src/ansi.hpp src/pager.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

$(BUILD):
	mkdir -p $(BUILD)

install: $(BUILD)/$(TARGET)
	install -m 755 $(BUILD)/$(TARGET) /usr/local/bin/$(TARGET)

clean:
	rm -rf $(BUILD)
