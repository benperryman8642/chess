#pragma once

#include "chess/position.h"
#include "chess/move.h"
#include "chess/undo.h"

namespace chess {

// Applies move to position.
// Fills Undo with all data needed to restore.
// Returns true (we do not check legality here).
bool make_move(Position& pos, const Move& move, Undo& undo);

// Restores position to state before make_move.
void undo_move(Position& pos, const Move& move, const Undo& undo);

} // namespace chess