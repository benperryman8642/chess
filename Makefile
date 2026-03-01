# ============================================================
# Chess Engine Makefile (C++20)
# ============================================================

CXX := g++

SRC_DIR := src
TEST_DIR := tests
INC_DIR := include
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

CLI_TARGET   := chess_cli
PERFT_TARGET := perft_tests
UNIT_TARGET  := unit_tests

# Suites
SMOKE_SUITE := data/perft_suite.txt
FULL_SUITE  := data/perftsuite_extended.txt

CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -I$(INC_DIR)
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O3

SRC_SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
TEST_SOURCES := $(shell find $(TEST_DIR) -name '*.cpp')

SRC_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/src/%.o,$(SRC_SOURCES))

ENGINE_SOURCES := $(filter-out $(SRC_DIR)/main.cpp,$(SRC_SOURCES))
ENGINE_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/src/%.o,$(ENGINE_SOURCES))

TEST_OBJECTS := $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/tests/%.o,$(TEST_SOURCES))

all: debug

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(BIN_DIR)/$(CLI_TARGET)

release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(BIN_DIR)/$(CLI_TARGET)

# ------------------------------------------------------------
# Linking
# ------------------------------------------------------------
$(BIN_DIR)/$(CLI_TARGET): $(SRC_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/$(PERFT_TARGET): $(ENGINE_OBJECTS) $(OBJ_DIR)/tests/perft_tests.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/$(UNIT_TARGET): $(ENGINE_OBJECTS) $(OBJ_DIR)/tests/unit_tests.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ------------------------------------------------------------
# Compile
# ------------------------------------------------------------
$(OBJ_DIR)/src/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ------------------------------------------------------------
# Test runners
# ------------------------------------------------------------
test: debug $(BIN_DIR)/$(PERFT_TARGET) $(BIN_DIR)/$(UNIT_TARGET)
	./$(BIN_DIR)/$(PERFT_TARGET) $(SMOKE_SUITE)
	./$(BIN_DIR)/$(UNIT_TARGET)

test-smoke: debug $(BIN_DIR)/$(PERFT_TARGET)
	./$(BIN_DIR)/$(PERFT_TARGET) $(SMOKE_SUITE)

test-full: release $(BIN_DIR)/$(PERFT_TARGET)
	./$(BIN_DIR)/$(PERFT_TARGET) $(FULL_SUITE)

# ------------------------------------------------------------
# Clean
# ------------------------------------------------------------
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all debug release test test-smoke test-full clean