#include "chess/game.h"

#include <algorithm>

#include "chess/fen.h"
#include "chess/makemove.h"
#include "chess/move.h"
#include "chess/movegen.h"
#include "chess/zobrist.h"

namespace chess {

Game::Game() {
    reset_startpos();
}

void Game::reset_startpos() {
    pos_ = Position::startpos();
    moves_.clear();
    undos_.clear();

    keys_.clear();
    keys_.push_back(zobrist_key(pos_));
}

bool Game::set_fen(std::string_view fen_str) {
    Position tmp;
    if (!from_fen(fen_str, tmp)) return false;

    pos_ = tmp;
    moves_.clear();
    undos_.clear();

    keys_.clear();
    keys_.push_back(zobrist_key(pos_));
    return true;
}

std::string Game::fen() const {
    return to_fen(pos_);
}

std::vector<Move> Game::legal_moves() const {
    // generate_legal requires non-const because it makes/undos internally
    Position copy = pos_;
    std::vector<Move> out;
    generate_legal(copy, out);
    return out;
}

bool Game::same_move(const Move& a, const Move& b) {
    // Compare by from/to/promo. Flags are position-derived.
    return a.from == b.from && a.to == b.to && a.promo == b.promo;
}

bool Game::play_move(const Move& m) {
    // Validate against generated legal moves
    Position copy = pos_;
    std::vector<Move> legals;
    generate_legal(copy, legals);

    auto it = std::find_if(legals.begin(), legals.end(),
                           [&](const Move& lm) { return same_move(lm, m); });

    if (it == legals.end()) return false;

    // Use the engine-produced move (correct flags)
    Undo u;
    make_move(pos_, *it, u);

    moves_.push_back(*it);
    undos_.push_back(u);
    keys_.push_back(zobrist_key(pos_));
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

    // keys_ has one entry per position, including current
    if (!keys_.empty()) keys_.pop_back();
    if (keys_.empty()) keys_.push_back(zobrist_key(pos_)); // safety

    return true;
}

int Game::repetition_count_current() const {
    if (keys_.empty()) return 1;
    const auto cur = keys_.back();

    int count = 0;
    for (auto k : keys_) {
        if (k == cur) ++count;
    }
    return count;
}

GameResult Game::status() const {
    return chess::result(pos_, repetition_count_current());
}

bool Game::step_ai() {
    Color stm = pos_.side_to_move();
    const size_t idx = static_cast<size_t>(stm);

    if (players_[idx] != PlayerType::AI) return false;
    if (!ai_[idx]) return false;

    auto m = ai_[idx](pos_);
    if (!m) return false;

    return play_move(*m);
}

} // namespace chess