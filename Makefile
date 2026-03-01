# ============================================================
# Chess Engine Makefile (C++20)
# ============================================================

# ------------------------------------------------------------
# Compiler
# ------------------------------------------------------------
CXX := g++

# ------------------------------------------------------------
# Directories
# ------------------------------------------------------------
SRC_DIR := src
TEST_DIR := tests
INC_DIR := include
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

# ------------------------------------------------------------
# Targets
# ------------------------------------------------------------
CLI_TARGET   := chess_cli
PERFT_TARGET := perft_tests
UNIT_TARGET  := unit_tests

# ------------------------------------------------------------
# Compiler flags
# ------------------------------------------------------------
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -I$(INC_DIR)
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O3

# ------------------------------------------------------------
# Source discovery
# ------------------------------------------------------------

# Engine + main sources
SRC_SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')

# Test sources
TEST_SOURCES := $(shell find $(TEST_DIR) -name '*.cpp')

# Engine objects (includes main.cpp)
SRC_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/src/%.o,$(SRC_SOURCES))

# Engine objects WITHOUT main.cpp (used for tests)
ENGINE_SOURCES := $(filter-out $(SRC_DIR)/main.cpp,$(SRC_SOURCES))
ENGINE_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/src/%.o,$(ENGINE_SOURCES))

# Test objects
TEST_OBJECTS := $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/tests/%.o,$(TEST_SOURCES))

# ------------------------------------------------------------
# Default build
# ------------------------------------------------------------
all: debug

# ------------------------------------------------------------
# Debug build
# ------------------------------------------------------------
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(BIN_DIR)/$(CLI_TARGET)

# ------------------------------------------------------------
# Release build
# ------------------------------------------------------------
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(BIN_DIR)/$(CLI_TARGET)

# ------------------------------------------------------------
# CLI linking
# ------------------------------------------------------------
$(BIN_DIR)/$(CLI_TARGET): $(SRC_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ------------------------------------------------------------
# Test linking
# ------------------------------------------------------------
$(BIN_DIR)/$(PERFT_TARGET): $(ENGINE_OBJECTS) $(OBJ_DIR)/tests/perft_tests.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/$(UNIT_TARGET): $(ENGINE_OBJECTS) $(OBJ_DIR)/tests/unit_tests.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ------------------------------------------------------------
# Compile rules
# ------------------------------------------------------------

# Engine source files
$(OBJ_DIR)/src/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test source files
$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ------------------------------------------------------------
# Test runner
# ------------------------------------------------------------
test: debug $(BIN_DIR)/$(PERFT_TARGET) $(BIN_DIR)/$(UNIT_TARGET)
	./$(BIN_DIR)/$(PERFT_TARGET)
	./$(BIN_DIR)/$(UNIT_TARGET)

# ------------------------------------------------------------
# Clean
# ------------------------------------------------------------
clean:
	rm -rf $(BUILD_DIR)

# ------------------------------------------------------------
# Phony
# ------------------------------------------------------------
.PHONY: all debug release test clean