#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "chess/move.h"
#include "chess/position.h"
#include "chess/rules.h"
#include "chess/undo.h"

namespace chess {

class Game {
public:
    enum class PlayerType { Human, AI };
    using AiMoveFn = std::function<std::optional<Move>(const Position&)>;

    Game();

    // Position management
    void reset_startpos();
    bool set_fen(std::string_view fen);
    std::string fen() const;

    const Position& position() const { return pos_; }
    Position& position_mut() { return pos_; } // use sparingly

    // Move lists
    std::vector<Move> legal_moves() const;

    // Play/undo
    bool play_move(const Move& m);       // validates legality
    bool play_uci(std::string_view uci); // parses + validates
    bool undo();

    // History
    size_t ply() const { return moves_.size(); }
    const std::vector<Move>& moves() const { return moves_; }

    // Draw detection helpers (automatic 3-fold + 50-move)
    int repetition_count_current() const;
    GameResult status() const;

    // Player mode wiring
    void set_player(Color side, PlayerType type) { players_[static_cast<size_t>(side)] = type; }
    PlayerType player(Color side) const { return players_[static_cast<size_t>(side)]; }

    void set_ai(Color side, AiMoveFn fn) { ai_[static_cast<size_t>(side)] = std::move(fn); }

    // If side-to-move is AI and callback exists, plays one AI move.
    bool step_ai();

private:
    Position pos_;

    std::vector<Move> moves_;
    std::vector<Undo> undos_;

    // Zobrist history: includes the key for the *current* position as keys_.back()
    std::vector<std::uint64_t> keys_;

    std::array<PlayerType, 2> players_{ PlayerType::Human, PlayerType::Human };
    std::array<AiMoveFn, 2> ai_{}; // empty std::function by default

    static bool same_move(const Move& a, const Move& b);
};

} // namespace chess