#include "chess/types.h"

#include <cctype>   // std::tolower
#include <optional>

namespace chess {

// ------------------------------------------------------------
// Square conversions
// ------------------------------------------------------------

std::string square_to_string(int sq) {
    if (!is_valid_square(sq)) return "";

    char file = 'a' + file_of(sq);
    char rank = '1' + rank_of(sq);

    return std::string{file, rank};
}

std::optional<int> string_to_square(std::string_view str) {
    if (str.size() != 2) return std::nullopt;

    char file = str[0];
    char rank = str[1];

    if (file < 'a' || file > 'h') return std::nullopt;
    if (rank < '1' || rank > '8') return std::nullopt;

    int f = file - 'a';
    int r = rank - '1';

    return make_square(f, r);
}

// ------------------------------------------------------------
// Piece conversions
// ------------------------------------------------------------

char piece_to_char(Piece p) {
    switch (p) {
        case WP: return 'P';
        case WN: return 'N';
        case WB: return 'B';
        case WR: return 'R';
        case WQ: return 'Q';
        case WK: return 'K';

        case BP: return 'p';
        case BN: return 'n';
        case BB: return 'b';
        case BR: return 'r';
        case BQ: return 'q';
        case BK: return 'k';

        default: return '.';
    }
}

Piece char_to_piece(char c) {
    switch (c) {
        case 'P': return WP;
        case 'N': return WN;
        case 'B': return WB;
        case 'R': return WR;
        case 'Q': return WQ;
        case 'K': return WK;

        case 'p': return BP;
        case 'n': return BN;
        case 'b': return BB;
        case 'r': return BR;
        case 'q': return BQ;
        case 'k': return BK;

        default:  return EMPTY;
    }
}

} // namespace chess