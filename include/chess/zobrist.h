#pragma once

#include <cstdint>
#include "chess/position.h"

namespace chess {

// Compute a 64-bit Zobrist key for the position.
// Includes: pieces, side to move, castling rights, en-passant (file), etc.
std::uint64_t zobrist_key(const Position& pos);

} // namespace chess