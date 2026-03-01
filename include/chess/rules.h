#pragma once

#include "chess/position.h"
#include "chess/types.h"

namespace chess {

enum class GameResult {
    Ongoing,
    Checkmate,
    Stalemate,
    DrawFiftyMove,
    DrawRepetition
};

bool in_check(const Position& pos, Color side);

// repetition_count is how many times the *current* position has appeared in the game history.
// (If you treat 3-fold as automatic draw, you’ll pass the computed count from Game.)
GameResult result(const Position& pos, int repetition_count);

} // namespace chess