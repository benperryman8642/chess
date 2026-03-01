#include "chess/perft.h"

#include <iostream>
#include <vector>

#include "chess/movegen.h"
#include "chess/makemove.h"
#include "chess/undo.h"
#include "chess/move.h"

namespace chess {

uint64_t perft(Position& pos, int depth) {
    if (depth <= 0) return 1;

    std::vector<Move> moves;
    generate_legal(pos, moves);

    if (depth == 1) {
        return static_cast<uint64_t>(moves.size());
    }

    uint64_t nodes = 0;
    for (const auto& m : moves) {
        Undo u;
        make_move(pos, m, u);
        nodes += perft(pos, depth - 1);
        undo_move(pos, m, u);
    }
    return nodes;
}

uint64_t perft_divide(Position& pos, int depth) {
    std::vector<Move> moves;
    generate_legal(pos, moves);

    uint64_t total = 0;

    for (const auto& m : moves) {
        Undo u;
        make_move(pos, m, u);
        uint64_t n = perft(pos, depth - 1);
        undo_move(pos, m, u);

        std::cout << move_to_uci(m) << ": " << n << "\n";
        total += n;
    }

    std::cout << "Total: " << total << "\n";
    return total;
}

} // namespace chess