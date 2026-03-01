#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <optional>

#include "chess/types.h"

namespace chess {

// Move flags (bitmask)
enum MoveFlags : uint8_t {
    MF_NONE         = 0,
    MF_CAPTURE      = 1 << 0,
    MF_DOUBLE_PUSH  = 1 << 1,
    MF_EN_PASSANT   = 1 << 2,
    MF_CASTLE       = 1 << 3,
    MF_PROMOTION    = 1 << 4
};

// Compact, simple move representation.
struct Move {
    uint8_t from = 0;   // 0..63
    uint8_t to   = 0;   // 0..63
    uint8_t promo = 0;  // PieceType (PT_*) for promotions, else 0
    uint8_t flags = 0;  // MoveFlags bitmask

    constexpr Move() = default;
    constexpr Move(uint8_t f, uint8_t t, uint8_t fl = MF_NONE, uint8_t pr = 0)
        : from(f), to(t), promo(pr), flags(fl) {}
};

constexpr bool is_capture(const Move& m)     { return (m.flags & MF_CAPTURE) != 0; }
constexpr bool is_promotion(const Move& m)   { return (m.flags & MF_PROMOTION) != 0; }
constexpr bool is_en_passant(const Move& m)  { return (m.flags & MF_EN_PASSANT) != 0; }
constexpr bool is_castle(const Move& m)      { return (m.flags & MF_CASTLE) != 0; }
constexpr bool is_double_push(const Move& m) { return (m.flags & MF_DOUBLE_PUSH) != 0; }

// UCI format:
//  - normal: "e2e4"
//  - promotion: "e7e8q" (always lower-case in UCI)
std::string move_to_uci(const Move& m);

// Parse a UCI move string (syntax only, no legality checking).
// Returns nullopt if malformed.
// promo field will store PieceType: PT_QUEEN/PT_ROOK/PT_BISHOP/PT_KNIGHT.
std::optional<Move> parse_uci_move(std::string_view s);

} // namespace chess