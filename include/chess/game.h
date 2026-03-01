#pragma once

#include <array>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "chess/position.h"
#include "chess/move.h"
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
    bool play_move(const Move& m);              // validates legality
    bool play_uci(std::string_view uci);        // parses + validates

    bool undo();

    // History
    size_t ply() const { return moves_.size(); }
    const std::vector<Move>& moves() const { return moves_; }

    // Player mode wiring
    void set_player(Color side, PlayerType type) { players_.at(side) = type; }
    PlayerType player(Color side) const { return players_.at(side); }

    void set_ai(Color side, AiMoveFn fn) { ai_.at(side) = std::move(fn); }

    // If side-to-move is AI and callback exists, plays one AI move.
    bool step_ai();

private:
    Position pos_;

    std::vector<Move> moves_;
    std::vector<Undo> undos_;

    std::array<PlayerType, 2> players_{ PlayerType::Human, PlayerType::Human };
    std::array<AiMoveFn, 2> ai_{}; // default-constructed std::function is empty

    static bool same_move(const Move& a, const Move& b);
};

} // namespace chess