#pragma once

#include <cstdint>
#include "chess/types.h"

namespace chess {

// Stores the minimal information needed to fully restore a Position
// after a move is made. Filled by make_move(), consumed by undo_move().
struct Undo {
    Piece captured = EMPTY;      // what was on the destination (or pawn in EP)
    uint8_t castling_rights = 0; // previous castling rights bitmask
    Square ep_square = -1;       // previous en-passant square
    uint16_t halfmove_clock = 0; // previous halfmove clock
    uint16_t fullmove_number = 1;// previous fullmove number (optional but safe)
};

} // namespace chess