# ============================================================
# Basic Chess Engine Makefile (C++20)
# ============================================================

# Compiler
CXX := g++

# Directories
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

# Target executable name
TARGET := chess_cli

# Compiler flags
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -I$(INC_DIR)
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O3

# Find all .cpp files
SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Default build (debug)
all: debug

# ------------------------------------------------------------
# Debug build
# ------------------------------------------------------------
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(BIN_DIR)/$(TARGET)

# ------------------------------------------------------------
# Release build
# ------------------------------------------------------------
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(BIN_DIR)/$(TARGET)

# ------------------------------------------------------------
# Link
# ------------------------------------------------------------
$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ------------------------------------------------------------
# Compile source files
# ------------------------------------------------------------
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ------------------------------------------------------------
# Clean
# ------------------------------------------------------------
clean:
	rm -rf $(BUILD_DIR)

# ------------------------------------------------------------
# Phony targets
# ------------------------------------------------------------
.PHONY: all debug release clean