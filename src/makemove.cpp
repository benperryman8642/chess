#include "chess/makemove.h"

namespace chess {

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

static void remove_castling_rights_on_rook_move(Position& pos, int from) {
    switch (from) {
        case 0:  pos.set_castling_rights(pos.castling_rights() & ~CASTLE_WQ); break; // a1
        case 7:  pos.set_castling_rights(pos.castling_rights() & ~CASTLE_WK); break; // h1
        case 56: pos.set_castling_rights(pos.castling_rights() & ~CASTLE_BQ); break; // a8
        case 63: pos.set_castling_rights(pos.castling_rights() & ~CASTLE_BK); break; // h8
    }
}

static void remove_castling_rights_on_rook_capture(Position& pos, int to) {
    switch (to) {
        case 0:  pos.set_castling_rights(pos.castling_rights() & ~CASTLE_WQ); break;
        case 7:  pos.set_castling_rights(pos.castling_rights() & ~CASTLE_WK); break;
        case 56: pos.set_castling_rights(pos.castling_rights() & ~CASTLE_BQ); break;
        case 63: pos.set_castling_rights(pos.castling_rights() & ~CASTLE_BK); break;
    }
}

// ------------------------------------------------------------

bool make_move(Position& pos, const Move& m, Undo& u) {

    u.captured = EMPTY;
    u.castling_rights = pos.castling_rights();
    u.ep_square = pos.ep_square();
    u.halfmove_clock = pos.halfmove_clock();
    u.fullmove_number = pos.fullmove_number();

    Piece moving = pos.at(m.from);
    Piece target = pos.at(m.to);

    Color us = pos.side_to_move();
    Color them = opposite(us);

    // --- halfmove clock ---
    if (piece_type(moving) == PT_PAWN || target != EMPTY)
        pos.set_halfmove_clock(0);
    else
        pos.set_halfmove_clock(pos.halfmove_clock() + 1);

    // --- en passant reset (cleared unless double push sets it again) ---
    pos.set_ep_square(-1);

    // --- capture handling ---
    if (is_en_passant(m)) {
        int cap_sq = (us == WHITE) ? m.to - 8 : m.to + 8;
        u.captured = pos.at(cap_sq);
        pos.set_piece(cap_sq, EMPTY);
    } else {
        u.captured = target;
    }

    if (u.captured != EMPTY)
        remove_castling_rights_on_rook_capture(pos, m.to);

    // --- move piece ---
    pos.set_piece(m.to, moving);
    pos.set_piece(m.from, EMPTY);

    // --- king move updates ---
    if (moving == WK) {
        pos.set_king_square(WHITE, m.to);
        pos.set_castling_rights(pos.castling_rights() & ~(CASTLE_WK | CASTLE_WQ));
    }
    else if (moving == BK) {
        pos.set_king_square(BLACK, m.to);
        pos.set_castling_rights(pos.castling_rights() & ~(CASTLE_BK | CASTLE_BQ));
    }

    // --- rook move updates ---
    if (piece_type(moving) == PT_ROOK)
        remove_castling_rights_on_rook_move(pos, m.from);

    // --- pawn double push sets ep square ---
    if (is_double_push(m)) {
        int ep = (us == WHITE) ? m.to - 8 : m.to + 8;
        pos.set_ep_square(ep);
    }

    // --- promotion ---
    if (is_promotion(m)) {
        PieceType pt = static_cast<PieceType>(m.promo);
        Piece promoted =
            (us == WHITE)
            ? static_cast<Piece>(pt)
            : static_cast<Piece>(pt + 6); // black pieces are offset by +6

        pos.set_piece(m.to, promoted);
    }

    // --- castling ---
    if (is_castle(m)) {
        if (m.to == make_square(6,0)) {         // white king side
            pos.set_piece(make_square(5,0), WR);
            pos.set_piece(make_square(7,0), EMPTY);
        }
        else if (m.to == make_square(2,0)) {    // white queen side
            pos.set_piece(make_square(3,0), WR);
            pos.set_piece(make_square(0,0), EMPTY);
        }
        else if (m.to == make_square(6,7)) {    // black king side
            pos.set_piece(make_square(5,7), BR);
            pos.set_piece(make_square(7,7), EMPTY);
        }
        else if (m.to == make_square(2,7)) {    // black queen side
            pos.set_piece(make_square(3,7), BR);
            pos.set_piece(make_square(0,7), EMPTY);
        }
    }

    // --- fullmove increment ---
    if (us == BLACK)
        pos.set_fullmove_number(pos.fullmove_number() + 1);

    // --- side to move ---
    pos.set_side_to_move(them);

    return true;
}

void undo_move(Position& pos, const Move& m, const Undo& u) {

    Color them = pos.side_to_move();
    Color us = opposite(them);

    pos.set_side_to_move(us);

    pos.set_castling_rights(u.castling_rights);
    pos.set_ep_square(u.ep_square);
    pos.set_halfmove_clock(u.halfmove_clock);
    pos.set_fullmove_number(u.fullmove_number);

    Piece moving = pos.at(m.to);

    // undo castling rook movement
    if (is_castle(m)) {
        if (m.to == make_square(6,0)) {
            pos.set_piece(make_square(7,0), WR);
            pos.set_piece(make_square(5,0), EMPTY);
        }
        else if (m.to == make_square(2,0)) {
            pos.set_piece(make_square(0,0), WR);
            pos.set_piece(make_square(3,0), EMPTY);
        }
        else if (m.to == make_square(6,7)) {
            pos.set_piece(make_square(7,7), BR);
            pos.set_piece(make_square(5,7), EMPTY);
        }
        else if (m.to == make_square(2,7)) {
            pos.set_piece(make_square(0,7), BR);
            pos.set_piece(make_square(3,7), EMPTY);
        }
    }

    // undo promotion
    if (is_promotion(m)) {
        moving = (us == WHITE) ? WP : BP;
    }

    pos.set_piece(m.from, moving);

    if (is_en_passant(m)) {
        int cap_sq = (us == WHITE) ? m.to - 8 : m.to + 8;
        pos.set_piece(cap_sq, u.captured);
        pos.set_piece(m.to, EMPTY);
    } else {
        pos.set_piece(m.to, u.captured);
    }

    if (moving == WK)
        pos.set_king_square(WHITE, m.from);
    else if (moving == BK)
        pos.set_king_square(BLACK, m.from);
}

} // namespace chess