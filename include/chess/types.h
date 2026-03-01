#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <optional>

namespace chess {

// ------------------------------------------------------------
// Core conventions
// Squares are indexed 0..63 with:
//   0 = a1, 1 = b1, ..., 7 = h1,
//   8 = a2, ..., 63 = h8
// So: file = sq % 8, rank = sq / 8
// ------------------------------------------------------------

using Square = int8_t;   // -1 can mean "no square"
using Piece  = uint8_t;  // encoded piece id
using Color  = uint8_t;  // 0/1
using Depth  = int;

// Colors
enum : Color { WHITE = 0, BLACK = 1 };

// Pieces (12 + EMPTY). Keep values stable across the project.
enum : Piece {
    EMPTY = 0,

    WP = 1, WN = 2, WB = 3, WR = 4, WQ = 5, WK = 6,
    BP = 7, BN = 8, BB = 9, BR = 10, BQ = 11, BK = 12
};

// Piece type (ignores color). Helpful for evaluation later.
enum PieceType : uint8_t {
    PT_NONE = 0,
    PT_PAWN = 1,
    PT_KNIGHT = 2,
    PT_BISHOP = 3,
    PT_ROOK = 4,
    PT_QUEEN = 5,
    PT_KING = 6
};

// Castling rights bitmask
enum Castling : uint8_t {
    CASTLE_NONE = 0,
    CASTLE_WK   = 1 << 0,
    CASTLE_WQ   = 1 << 1,
    CASTLE_BK   = 1 << 2,
    CASTLE_BQ   = 1 << 3
};

// ------------------------------------------------------------
// Square helpers
// ------------------------------------------------------------

constexpr bool is_valid_square(int sq) {
    return sq >= 0 && sq < 64;
}

constexpr int file_of(int sq) { return sq & 7; }     // 0..7 (a..h)
constexpr int rank_of(int sq) { return sq >> 3; }    // 0..7 (1..8)

constexpr int make_square(int file, int rank) {
    return (rank << 3) | file;
}

constexpr int flip_rank(int sq) {
    // a1 <-> a8, etc.
    return sq ^ 56;
}

constexpr Color opposite(Color c) { return c ^ 1; }

// ------------------------------------------------------------
// Piece helpers
// ------------------------------------------------------------

constexpr bool is_white(Piece p) { return p >= WP && p <= WK; }
constexpr bool is_black(Piece p) { return p >= BP && p <= BK; }

constexpr Color piece_color(Piece p) {
    return is_black(p) ? BLACK : WHITE; // only valid for non-EMPTY
}

constexpr PieceType piece_type(Piece p) {
    switch (p) {
        case WP: case BP: return PT_PAWN;
        case WN: case BN: return PT_KNIGHT;
        case WB: case BB: return PT_BISHOP;
        case WR: case BR: return PT_ROOK;
        case WQ: case BQ: return PT_QUEEN;
        case WK: case BK: return PT_KING;
        default:          return PT_NONE;
    }
}

constexpr bool is_slider(Piece p) {
    const auto t = piece_type(p);
    return t == PT_BISHOP || t == PT_ROOK || t == PT_QUEEN;
}

// ------------------------------------------------------------
// Conversions (declarations here; definitions can live in a .cpp)
// ------------------------------------------------------------

std::string square_to_string(int sq);                 // 0..63 -> "e4"
std::optional<int> string_to_square(std::string_view); // "e4" -> 0..63

char piece_to_char(Piece p);                          // WP -> 'P', BK -> 'k'
Piece char_to_piece(char c);                          // 'q' -> BQ, etc.

} // namespace chess