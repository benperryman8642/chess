#include <cassert>
#include <iostream>
#include <string>

#include "chess/fen.h"
#include "chess/makemove.h"
#include "chess/movegen.h"
#include "chess/position.h"
#include "chess/undo.h"
#include "chess/game.h"
#include "chess/rules.h"

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

static void test_threefold_repetition_draw() {
    chess::Game g;
    // Start position is already occurrence #1
    assert(g.repetition_count_current() == 1);
    assert(g.status() == chess::GameResult::Ongoing);

    // Cycle 1: Nf3 ... Nf6 Ng1 ... Ng8
    assert(g.play_uci("g1f3"));
    assert(g.play_uci("g8f6"));
    assert(g.play_uci("f3g1"));
    assert(g.play_uci("f6g8"));

    // Back to start position: occurrence #2
    assert(g.repetition_count_current() == 2);
    assert(g.status() == chess::GameResult::Ongoing);

    // Cycle 2
    assert(g.play_uci("g1f3"));
    assert(g.play_uci("g8f6"));
    assert(g.play_uci("f3g1"));
    assert(g.play_uci("f6g8"));

    // Back to start position: occurrence #3 -> automatic draw
    assert(g.repetition_count_current() == 3);
    assert(g.status() == chess::GameResult::DrawRepetition);

    // Optional: undo should reduce repetition count / remove draw
    assert(g.undo()); // undo f6g8
    assert(g.status() != chess::GameResult::DrawRepetition);
}

static void test_fifty_move_draw() {
    chess::Game g;

    // Simple legal position with halfmove clock = 100.
    // (Kings only is legal; castling '-' and ep '-' are fine.)
    const std::string fen = "8/8/8/8/8/8/8/K6k w - - 100 1";
    assert(g.set_fen(fen));

    assert(g.status() == chess::GameResult::DrawFiftyMove);

    // Optional sanity: if you undo to an earlier state (not applicable here),
    // you'd expect it to go back to ongoing. Here we just confirm it triggers.
}

int main() {
    test_fen_roundtrip();
    test_make_undo_identity_startpos_one_ply();
    test_threefold_repetition_draw();
    test_fifty_move_draw();
    std::cout << "Unit tests passed\n";
    return 0;
}