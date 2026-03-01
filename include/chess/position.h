#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "chess/types.h"

namespace chess {

class Position {
public:
    Position();

    // Construct the standard initial position
    static Position startpos();

    // Accessors
    Piece at(int sq) const;
    void  set_piece(int sq, Piece p);

    Color side_to_move() const { return stm_; }
    void  set_side_to_move(Color c) { stm_ = c; }

    uint8_t castling_rights() const { return castling_; }
    void    set_castling_rights(uint8_t rights) { castling_ = rights; }

    Square ep_square() const { return ep_sq_; }
    void   set_ep_square(Square sq) { ep_sq_ = sq; }

    uint16_t halfmove_clock() const { return halfmove_; }
    void     set_halfmove_clock(uint16_t hm) { halfmove_ = hm; }

    uint16_t fullmove_number() const { return fullmove_; }
    void     set_fullmove_number(uint16_t fm) { fullmove_ = fm; }

    // King square helpers (cached)
    int king_square(Color c) const { return king_sq_[c]; }
    void set_king_square(Color c, int sq) { king_sq_[c] = sq; }

    // Basic debug printing (optional convenience, not "UI")
    // Returns an ASCII board with ranks 8..1.
    std::string ascii_board() const;

private:
    std::array<Piece, 64> board_{};

    Color   stm_      = WHITE;
    uint8_t castling_ = CASTLE_NONE;
    Square  ep_sq_    = -1;
    uint16_t halfmove_ = 0;
    uint16_t fullmove_ = 1;

    // Cache king squares for fast check detection later.
    // Always keep updated when setting pieces / making moves.
    std::array<uint8_t, 2> king_sq_{0, 0};
};

} // namespace chess