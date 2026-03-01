#include "chess/movegen.h"

#include "chess/attack.h"
#include "chess/makemove.h"
#include "chess/undo.h"

namespace chess {

static inline bool is_enemy_piece(Piece p, Color us) {
    return p != EMPTY && piece_color(p) != us;
}

static inline void add_move(std::vector<Move>& out, int from, int to, uint8_t flags = MF_NONE, uint8_t promo = 0) {
    out.emplace_back(static_cast<uint8_t>(from), static_cast<uint8_t>(to), flags, promo);
}

static void gen_knights(const Position& pos, Color us, std::vector<Move>& out) {
    static constexpr int kdf[8] = {  1,  2,  2,  1, -1, -2, -2, -1 };
    static constexpr int kdr[8] = {  2,  1, -1, -2, -2, -1,  1,  2 };

    for (int from = 0; from < 64; ++from) {
        Piece pc = pos.at(from);
        if (pc == EMPTY) continue;

        if (piece_type(pc) != PT_KNIGHT || piece_color(pc) != us) continue;

        int f = file_of(from);
        int r = rank_of(from);

        for (int i = 0; i < 8; ++i) {
            int nf = f + kdf[i];
            int nr = r + kdr[i];
            if (nf < 0 || nf > 7 || nr < 0 || nr > 7) continue;

            int to = make_square(nf, nr);
            Piece dst = pos.at(to);

            if (dst == EMPTY) add_move(out, from, to);
            else if (is_enemy_piece(dst, us)) add_move(out, from, to, MF_CAPTURE);
        }
    }
}

static void gen_king_and_castle(const Position& pos, Color us, std::vector<Move>& out) {
    int from = pos.king_square(us);
    int f = file_of(from);
    int r = rank_of(from);

    // King steps
    for (int df = -1; df <= 1; ++df) {
        for (int dr = -1; dr <= 1; ++dr) {
            if (df == 0 && dr == 0) continue;
            int nf = f + df;
            int nr = r + dr;
            if (nf < 0 || nf > 7 || nr < 0 || nr > 7) continue;

            int to = make_square(nf, nr);
            Piece dst = pos.at(to);

            if (dst == EMPTY) add_move(out, from, to);
            else if (is_enemy_piece(dst, us)) add_move(out, from, to, MF_CAPTURE);
        }
    }

    // Castling (pseudo-legal: also checks empty squares + not currently in check + transit squares not attacked)
    const Color them = opposite(us);

    if (us == WHITE) {
        if (pos.castling_rights() & CASTLE_WK) {
            // e1 -> g1: squares f1, g1 empty; e1,f1,g1 not attacked
            if (pos.at(make_square(5,0)) == EMPTY && pos.at(make_square(6,0)) == EMPTY) {
                int e1 = make_square(4,0);
                int f1 = make_square(5,0);
                int g1 = make_square(6,0);
                if (!is_square_attacked(pos, e1, them) &&
                    !is_square_attacked(pos, f1, them) &&
                    !is_square_attacked(pos, g1, them)) {
                    add_move(out, e1, g1, MF_CASTLE);
                }
            }
        }
        if (pos.castling_rights() & CASTLE_WQ) {
            // e1 -> c1: squares d1,c1,b1 empty; e1,d1,c1 not attacked
            if (pos.at(make_square(3,0)) == EMPTY && pos.at(make_square(2,0)) == EMPTY && pos.at(make_square(1,0)) == EMPTY) {
                int e1 = make_square(4,0);
                int d1 = make_square(3,0);
                int c1 = make_square(2,0);
                if (!is_square_attacked(pos, e1, them) &&
                    !is_square_attacked(pos, d1, them) &&
                    !is_square_attacked(pos, c1, them)) {
                    add_move(out, e1, c1, MF_CASTLE);
                }
            }
        }
    } else {
        if (pos.castling_rights() & CASTLE_BK) {
            // e8 -> g8
            if (pos.at(make_square(5,7)) == EMPTY && pos.at(make_square(6,7)) == EMPTY) {
                int e8 = make_square(4,7);
                int f8 = make_square(5,7);
                int g8 = make_square(6,7);
                if (!is_square_attacked(pos, e8, them) &&
                    !is_square_attacked(pos, f8, them) &&
                    !is_square_attacked(pos, g8, them)) {
                    add_move(out, e8, g8, MF_CASTLE);
                }
            }
        }
        if (pos.castling_rights() & CASTLE_BQ) {
            // e8 -> c8
            if (pos.at(make_square(3,7)) == EMPTY && pos.at(make_square(2,7)) == EMPTY && pos.at(make_square(1,7)) == EMPTY) {
                int e8 = make_square(4,7);
                int d8 = make_square(3,7);
                int c8 = make_square(2,7);
                if (!is_square_attacked(pos, e8, them) &&
                    !is_square_attacked(pos, d8, them) &&
                    !is_square_attacked(pos, c8, them)) {
                    add_move(out, e8, c8, MF_CASTLE);
                }
            }
        }
    }
}

static void gen_sliders(const Position& pos, Color us, std::vector<Move>& out) {
    // Directions: rook (4) + bishop (4)
    static constexpr int rdf[4] = {  1, -1,  0,  0 };
    static constexpr int rdr[4] = {  0,  0,  1, -1 };
    static constexpr int bdf[4] = {  1,  1, -1, -1 };
    static constexpr int bdr[4] = {  1, -1,  1, -1 };

    for (int from = 0; from < 64; ++from) {
        Piece pc = pos.at(from);
        if (pc == EMPTY || piece_color(pc) != us) continue;

        PieceType pt = piece_type(pc);
        bool rook_like = (pt == PT_ROOK || pt == PT_QUEEN);
        bool bishop_like = (pt == PT_BISHOP || pt == PT_QUEEN);
        if (!rook_like && !bishop_like) continue;

        int f0 = file_of(from);
        int r0 = rank_of(from);

        if (rook_like) {
            for (int d = 0; d < 4; ++d) {
                int f = f0 + rdf[d];
                int r = r0 + rdr[d];
                while (f >= 0 && f < 8 && r >= 0 && r < 8) {
                    int to = make_square(f, r);
                    Piece dst = pos.at(to);
                    if (dst == EMPTY) {
                        add_move(out, from, to);
                    } else {
                        if (is_enemy_piece(dst, us)) add_move(out, from, to, MF_CAPTURE);
                        break;
                    }
                    f += rdf[d];
                    r += rdr[d];
                }
            }
        }

        if (bishop_like) {
            for (int d = 0; d < 4; ++d) {
                int f = f0 + bdf[d];
                int r = r0 + bdr[d];
                while (f >= 0 && f < 8 && r >= 0 && r < 8) {
                    int to = make_square(f, r);
                    Piece dst = pos.at(to);
                    if (dst == EMPTY) {
                        add_move(out, from, to);
                    } else {
                        if (is_enemy_piece(dst, us)) add_move(out, from, to, MF_CAPTURE);
                        break;
                    }
                    f += bdf[d];
                    r += bdr[d];
                }
            }
        }
    }
}

static void gen_pawns(const Position& pos, Color us, std::vector<Move>& out) {
    const int dir = (us == WHITE) ? 1 : -1;          // rank direction
    const int start_rank = (us == WHITE) ? 1 : 6;
    const int promo_rank = (us == WHITE) ? 6 : 1;    // pawn on this rank can move to last rank and promote
    const int last_rank  = (us == WHITE) ? 7 : 0;

    const Piece pawn = (us == WHITE) ? WP : BP;

    for (int from = 0; from < 64; ++from) {
        if (pos.at(from) != pawn) continue;

        int f = file_of(from);
        int r = rank_of(from);

        // Single push
        int r1 = r + dir;
        if (r1 >= 0 && r1 <= 7) {
            int to = make_square(f, r1);
            if (pos.at(to) == EMPTY) {
                if (r == promo_rank) {
                    // promotions (quiet)
                    add_move(out, from, to, MF_PROMOTION, PT_QUEEN);
                    add_move(out, from, to, MF_PROMOTION, PT_ROOK);
                    add_move(out, from, to, MF_PROMOTION, PT_BISHOP);
                    add_move(out, from, to, MF_PROMOTION, PT_KNIGHT);
                } else {
                    add_move(out, from, to);

                    // Double push
                    if (r == start_rank) {
                        int r2 = r + 2 * dir;
                        int to2 = make_square(f, r2);
                        if (pos.at(to2) == EMPTY) {
                            add_move(out, from, to2, MF_DOUBLE_PUSH);
                        }
                    }
                }
            }
        }

        // Captures (including promotion captures)
        for (int df : {-1, 1}) {
            int nf = f + df;
            int nr = r + dir;
            if (nf < 0 || nf > 7 || nr < 0 || nr > 7) continue;

            int to = make_square(nf, nr);
            Piece dst = pos.at(to);

            if (dst != EMPTY && is_enemy_piece(dst, us)) {
                if (r == promo_rank) {
                    add_move(out, from, to, MF_CAPTURE | MF_PROMOTION, PT_QUEEN);
                    add_move(out, from, to, MF_CAPTURE | MF_PROMOTION, PT_ROOK);
                    add_move(out, from, to, MF_CAPTURE | MF_PROMOTION, PT_BISHOP);
                    add_move(out, from, to, MF_CAPTURE | MF_PROMOTION, PT_KNIGHT);
                } else {
                    add_move(out, from, to, MF_CAPTURE);
                }
            }
        }

        // En passant captures (pseudo-legal)
        if (pos.ep_square() != -1) {
            int ep = pos.ep_square();
            int ep_f = file_of(ep);
            int ep_r = rank_of(ep);

            // EP target square must be one step diagonally forward
            if (ep_r == r + dir && (ep_f == f - 1 || ep_f == f + 1)) {
                add_move(out, from, ep, MF_EN_PASSANT | MF_CAPTURE);
            }
        }

        (void)last_rank; // quiet unused var guard (kept for readability)
    }
}

void generate_pseudo_legal(const Position& pos, std::vector<Move>& out) {
    out.clear();
    Color us = pos.side_to_move();

    gen_pawns(pos, us, out);
    gen_knights(pos, us, out);
    gen_sliders(pos, us, out);
    gen_king_and_castle(pos, us, out);
}

void generate_legal(Position& pos, std::vector<Move>& out) {
    out.clear();

    std::vector<Move> pseudo;
    generate_pseudo_legal(pos, pseudo);

    Color us = pos.side_to_move();
    Color them = opposite(us);

    int king_from = pos.king_square(us);

    for (const Move& m : pseudo) {
        Undo u;
        make_move(pos, m, u);

        // After making the move, it's now opponent's turn.
        // We need to check if OUR king is attacked in the resulting position.
        int king_sq = king_from;

        // If we moved our king, update to destination (make_move also caches, but we keep it explicit)
        Piece moved_piece = pos.at(m.to);
        if (piece_type(moved_piece) == PT_KING && piece_color(moved_piece) == us) {
            king_sq = m.to;
        } else {
            // safer: trust cache
            king_sq = pos.king_square(us);
        }

        bool illegal = is_square_attacked(pos, king_sq, them);

        undo_move(pos, m, u);

        if (!illegal) out.push_back(m);
    }
}

} // namespace chess