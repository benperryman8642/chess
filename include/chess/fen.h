#pragma once

#include <string>
#include <string_view>

#include "chess/position.h"

namespace chess {

// Parse a full FEN string into a Position.
// Returns true on success; on failure, leaves 'out' unchanged.
bool from_fen(std::string_view fen, Position& out);

// Convert a Position to a full FEN string.
std::string to_fen(const Position& pos);

} // namespace chess