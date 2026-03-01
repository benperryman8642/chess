#pragma once

#include <vector>

#include "chess/position.h"
#include "chess/move.h"

namespace chess {

// Generate pseudo-legal moves for side to move (may leave king in check).
void generate_pseudo_legal(const Position& pos, std::vector<Move>& out);

// Generate legal moves for side to move (filters out moves that leave king in check).
void generate_legal(Position& pos, std::vector<Move>& out);

} // namespace chess