#include <cassert>
#include <iostream>
#include <string>

#include "chess/fen.h"
#include "chess/makemove.h"
#include "chess/movegen.h"
#include "chess/position.h"
#include "chess/undo.h"

static void test_fen_roundtrip() {
    const std::string start =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    chess::Position p;
    assert(chess::from_fen(start, p));
    const auto out = chess::to_fen(p);
    assert(out == start);
}

static void test_make_undo_identity_startpos_one_ply() {
    chess::Position p = chess::Position::startpos();
    const auto original_fen = chess::to_fen(p);

    std::vector<chess::Move> moves;
    chess::generate_legal(p, moves);

    for (const auto& m : moves) {
        chess::Undo u;
        chess::make_move(p, m, u);
        chess::undo_move(p, m, u);

        const auto after = chess::to_fen(p);
        assert(after == original_fen);
    }
}

int main() {
    test_fen_roundtrip();
    test_make_undo_identity_startpos_one_ply();
    std::cout << "Unit tests passed\n";
    return 0;
}