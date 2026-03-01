#include "chess/rules.h"

#include <vector>

#include "chess/attack.h"
#include "chess/movegen.h"
#include "chess/move.h"

namespace chess {

bool in_check(const Position& pos, Color side) {
    int ksq = pos.king_square(side);
    return is_square_attacked(pos, ksq, opposite(side));
}

GameResult result(const Position& pos, int repetition_count) {
    // Automatic draws first (your chosen simplification)
    if (repetition_count >= 3) return GameResult::DrawRepetition;
    if (pos.halfmove_clock() >= 100) return GameResult::DrawFiftyMove; // 100 plies = 50 moves

    // Mate/stalemate depends on legal moves
    Position copy = pos;
    std::vector<Move> moves;
    generate_legal(copy, moves);

    if (!moves.empty()) return GameResult::Ongoing;

    // No legal moves
    if (in_check(pos, pos.side_to_move())) return GameResult::Checkmate;
    return GameResult::Stalemate;
}

} // namespace chess