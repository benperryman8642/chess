#include "chess/game.h"

#include <algorithm>

#include "chess/fen.h"
#include "chess/makemove.h"
#include "chess/movegen.h"
#include "chess/move.h"

namespace chess {

Game::Game() {
    reset_startpos();
}

void Game::reset_startpos() {
    pos_ = Position::startpos();
    moves_.clear();
    undos_.clear();
}

bool Game::set_fen(std::string_view fen_str) {
    Position tmp;
    if (!from_fen(fen_str, tmp)) return false;
    pos_ = tmp;
    moves_.clear();
    undos_.clear();
    return true;
}

std::string Game::fen() const {
    return to_fen(pos_);
}

std::vector<Move> Game::legal_moves() const {
    // generate_legal requires a non-const Position because it makes/undos moves
    Position copy = pos_;
    std::vector<Move> out;
    generate_legal(copy, out);
    return out;
}

bool Game::same_move(const Move& a, const Move& b) {
    // For legality checking we treat moves equal if from/to/promo match.
    // Flags are derived from position and may differ depending on how parsed.
    return a.from == b.from && a.to == b.to && a.promo == b.promo;
}

bool Game::play_move(const Move& m) {
    // Validate against generated legal moves.
    Position copy = pos_;
    std::vector<Move> legals;
    generate_legal(copy, legals);

    auto it = std::find_if(legals.begin(), legals.end(),
                           [&](const Move& lm) { return same_move(lm, m); });

    if (it == legals.end()) return false;

    // Use the engine-produced move (has correct flags: capture/ep/castle/doublepush)
    Undo u;
    make_move(pos_, *it, u);

    moves_.push_back(*it);
    undos_.push_back(u);
    return true;
}

bool Game::play_uci(std::string_view uci) {
    auto pm = parse_uci_move(uci);
    if (!pm) return false;
    return play_move(*pm);
}

bool Game::undo() {
    if (moves_.empty()) return false;

    Move m = moves_.back();
    Undo u = undos_.back();

    moves_.pop_back();
    undos_.pop_back();

    undo_move(pos_, m, u);
    return true;
}

bool Game::step_ai() {
    Color stm = pos_.side_to_move();

    if (players_[stm] != PlayerType::AI) return false;
    if (!ai_[stm]) return false;

    auto m = ai_[stm](pos_);
    if (!m) return false;

    // Validate + play
    return play_move(*m);
}

} // namespace chess