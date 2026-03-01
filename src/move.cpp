#include "chess/move.h"

#include <cctype> // std::tolower

namespace chess {

static std::optional<PieceType> promo_char_to_pt(char c) {
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    switch (c) {
        case 'q': return PT_QUEEN;
        case 'r': return PT_ROOK;
        case 'b': return PT_BISHOP;
        case 'n': return PT_KNIGHT;
        default:  return std::nullopt;
    }
}

static char pt_to_promo_char(PieceType pt) {
    // UCI uses lowercase letters
    switch (pt) {
        case PT_QUEEN:  return 'q';
        case PT_ROOK:   return 'r';
        case PT_BISHOP: return 'b';
        case PT_KNIGHT: return 'n';
        default:        return '\0';
    }
}

std::string move_to_uci(const Move& m) {
    std::string out;
    out.reserve(5);

    out += square_to_string(m.from);
    out += square_to_string(m.to);

    if (is_promotion(m) && m.promo != 0) {
        char pc = pt_to_promo_char(static_cast<PieceType>(m.promo));
        if (pc != '\0') out += pc;
    }
    return out;
}

std::optional<Move> parse_uci_move(std::string_view s) {
    // "e2e4" or "e7e8q"
    if (s.size() != 4 && s.size() != 5) return std::nullopt;

    auto fromSq = string_to_square(s.substr(0, 2));
    auto toSq   = string_to_square(s.substr(2, 2));
    if (!fromSq || !toSq) return std::nullopt;

    Move m(static_cast<uint8_t>(*fromSq), static_cast<uint8_t>(*toSq));

    if (s.size() == 5) {
        auto pt = promo_char_to_pt(s[4]);
        if (!pt) return std::nullopt;
        m.flags |= MF_PROMOTION;
        m.promo = static_cast<uint8_t>(*pt);
    }

    // Note: we do NOT set capture/ep/castle/double-push here;
    // those are determined by the position when validating/applying.
    return m;
}

} // namespace chess