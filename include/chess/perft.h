#pragma once

#include <cstdint>

#include "chess/position.h"

namespace chess {

// Counts leaf nodes to `depth` using legal move generation.
uint64_t perft(Position& pos, int depth);

// Like perft, but prints each root move with its node count (useful for debugging).
uint64_t perft_divide(Position& pos, int depth);

} // namespace chess