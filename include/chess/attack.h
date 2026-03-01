#pragma once

#include "chess/position.h"
#include "chess/types.h"

namespace chess {

// Returns true if `square` is attacked by side `by` in the given position.
bool is_square_attacked(const Position& pos, int square, Color by);

} // namespace chess