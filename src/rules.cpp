#include "chess/rules.h"

#include "chess/attack.h"
#include "chess/movegen.h"

namespace chess {

bool in_check(const Position& pos, Color side) {
    int ksq = pos.king_square(side);
    return is_square_attacked(pos, ksq, opposite(side));
}

GameResult result(const Position& pos) {
    // If side to move has any legal move, game is ongoing.
    Position copy = pos;
    std::vector<Move> moves;
    generate_legal(copy, moves);

    if (!moves.empty()) return GameResult::Ongoing;

    // No legal moves: checkmate or stalemate depending on check status.
    if (in_check(pos, pos.side_to_move())) return GameResult::Checkmate;
    return GameResult::Stalemate;
}

} // namespace chess