#include "chess/position.h"

#include <sstream>

namespace chess {

Position::Position() {
    board_.fill(EMPTY);
    // stm_, castling_, ep_sq_, halfmove_, fullmove_ already defaulted
    king_sq_[WHITE] = 0; // a1 placeholder until set
    king_sq_[BLACK] = 0;
}

Piece Position::at(int sq) const {
    if (!is_valid_square(sq)) return EMPTY;
    return board_[static_cast<size_t>(sq)];
}

void Position::set_piece(int sq, Piece p) {
    if (!is_valid_square(sq)) return;
    board_[static_cast<size_t>(sq)] = p;

    // Maintain king cache if a king is placed/removed.
    if (p == WK) king_sq_[WHITE] = static_cast<uint8_t>(sq);
    else if (p == BK) king_sq_[BLACK] = static_cast<uint8_t>(sq);
    // If a king is removed (set to EMPTY), we don't auto-fix here;
    // higher-level code (FEN or make/undo) should ensure valid positions.
}

Position Position::startpos() {
    Position p;

    // White pieces
    p.set_piece(make_square(0,0), WR); // a1
    p.set_piece(make_square(1,0), WN); // b1
    p.set_piece(make_square(2,0), WB); // c1
    p.set_piece(make_square(3,0), WQ); // d1
    p.set_piece(make_square(4,0), WK); // e1
    p.set_piece(make_square(5,0), WB); // f1
    p.set_piece(make_square(6,0), WN); // g1
    p.set_piece(make_square(7,0), WR); // h1

    for (int f = 0; f < 8; ++f)
        p.set_piece(make_square(f,1), WP); // rank 2 pawns

    // Black pieces
    p.set_piece(make_square(0,7), BR); // a8
    p.set_piece(make_square(1,7), BN); // b8
    p.set_piece(make_square(2,7), BB); // c8
    p.set_piece(make_square(3,7), BQ); // d8
    p.set_piece(make_square(4,7), BK); // e8
    p.set_piece(make_square(5,7), BB); // f8
    p.set_piece(make_square(6,7), BN); // g8
    p.set_piece(make_square(7,7), BR); // h8

    for (int f = 0; f < 8; ++f)
        p.set_piece(make_square(f,6), BP); // rank 7 pawns

    p.set_side_to_move(WHITE);
    p.set_castling_rights(CASTLE_WK | CASTLE_WQ | CASTLE_BK | CASTLE_BQ);
    p.set_ep_square(-1);
    p.set_halfmove_clock(0);
    p.set_fullmove_number(1);

    return p;
}

std::string Position::ascii_board() const {
    std::ostringstream oss;

    for (int r = 7; r >= 0; --r) {
        oss << (r + 1) << " ";
        for (int f = 0; f < 8; ++f) {
            int sq = make_square(f, r);
            oss << piece_to_char(at(sq)) << " ";
        }
        oss << "\n";
    }
    oss << "  a b c d e f g h\n";
    return oss.str();
}

} // namespace chess