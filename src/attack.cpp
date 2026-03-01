#include "chess/attack.h"

namespace chess {

// Helper: check bounds and compare piece
static inline bool has_piece(const Position& pos, int sq, Piece p) {
    return is_valid_square(sq) && pos.at(sq) == p;
}

// Helper: ray scan in a direction (df, dr) for rook/bishop/queen style attacks
static bool ray_attacked_by(const Position& pos, int target_sq, Color by, int df, int dr) {
    int tf = file_of(target_sq);
    int tr = rank_of(target_sq);

    int f = tf + df;
    int r = tr + dr;

    while (f >= 0 && f < 8 && r >= 0 && r < 8) {
        int sq = make_square(f, r);
        Piece pc = pos.at(sq);

        if (pc != EMPTY) {
            if (piece_color(pc) != by) return false; // blocked by enemy piece

            PieceType pt = piece_type(pc);
            // Orthogonal rays: rook or queen
            if ((df == 0 || dr == 0) && (pt == PT_ROOK || pt == PT_QUEEN)) return true;
            // Diagonal rays: bishop or queen
            if ((df != 0 && dr != 0) && (pt == PT_BISHOP || pt == PT_QUEEN)) return true;

            return false; // blocked by friendly piece that isn't an attacker type
        }

        f += df;
        r += dr;
    }

    return false;
}

bool is_square_attacked(const Position& pos, int square, Color by) {
    if (!is_valid_square(square)) return false;

    const int f = file_of(square);
    const int r = rank_of(square);

    // ------------------------------------------------------------
    // Pawn attacks
    // White pawns attack (f-1,r-1) and (f+1,r-1) relative to target
    // Black pawns attack (f-1,r+1) and (f+1,r+1) relative to target
    // ------------------------------------------------------------
    if (by == WHITE) {
        if (r > 0) {
            if (f > 0 && has_piece(pos, make_square(f - 1, r - 1), WP)) return true;
            if (f < 7 && has_piece(pos, make_square(f + 1, r - 1), WP)) return true;
        }
    } else { // by == BLACK
        if (r < 7) {
            if (f > 0 && has_piece(pos, make_square(f - 1, r + 1), BP)) return true;
            if (f < 7 && has_piece(pos, make_square(f + 1, r + 1), BP)) return true;
        }
    }

    // ------------------------------------------------------------
    // Knight attacks
    // ------------------------------------------------------------
    static constexpr int kdf[8] = {  1,  2,  2,  1, -1, -2, -2, -1 };
    static constexpr int kdr[8] = {  2,  1, -1, -2, -2, -1,  1,  2 };

    const Piece knight = (by == WHITE) ? WN : BN;
    for (int i = 0; i < 8; ++i) {
        int nf = f + kdf[i];
        int nr = r + kdr[i];
        if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8) {
            if (has_piece(pos, make_square(nf, nr), knight)) return true;
        }
    }

    // ------------------------------------------------------------
    // King attacks (adjacent squares)
    // ------------------------------------------------------------
    const Piece king = (by == WHITE) ? WK : BK;
    for (int df = -1; df <= 1; ++df) {
        for (int dr = -1; dr <= 1; ++dr) {
            if (df == 0 && dr == 0) continue;
            int nf = f + df;
            int nr = r + dr;
            if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8) {
                if (has_piece(pos, make_square(nf, nr), king)) return true;
            }
        }
    }

    // ------------------------------------------------------------
    // Sliding attacks: bishops/rooks/queens
    // Use rays in 8 directions
    // ------------------------------------------------------------

    // Rook/queen directions
    if (ray_attacked_by(pos, square, by,  1,  0)) return true;
    if (ray_attacked_by(pos, square, by, -1,  0)) return true;
    if (ray_attacked_by(pos, square, by,  0,  1)) return true;
    if (ray_attacked_by(pos, square, by,  0, -1)) return true;

    // Bishop/queen directions
    if (ray_attacked_by(pos, square, by,  1,  1)) return true;
    if (ray_attacked_by(pos, square, by,  1, -1)) return true;
    if (ray_attacked_by(pos, square, by, -1,  1)) return true;
    if (ray_attacked_by(pos, square, by, -1, -1)) return true;

    return false;
}

} // namespace chess