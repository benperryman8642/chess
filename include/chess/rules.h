#pragma once

#include <vector>
#include "chess/position.h"

namespace chess {

enum class GameResult {
    Ongoing,
    Checkmate,
    Stalemate
};

bool in_check(const Position& pos, Color side);

GameResult result(const Position& pos);

} // namespace chess