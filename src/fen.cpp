#include "chess/fen.h"

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "chess/types.h"

namespace chess {

static bool parse_castling(std::string_view token, uint8_t& out_rights) {
    out_rights = CASTLE_NONE;
    if (token == "-") return true;

    for (char c : token) {
        switch (c) {
            case 'K': out_rights |= CASTLE_WK; break;
            case 'Q': out_rights |= CASTLE_WQ; break;
            case 'k': out_rights |= CASTLE_BK; break;
            case 'q': out_rights |= CASTLE_BQ; break;
            default: return false;
        }
    }
    return true;
}

static std::string castling_to_string(uint8_t rights) {
    std::string s;
    if (rights & CASTLE_WK) s += 'K';
    if (rights & CASTLE_WQ) s += 'Q';
    if (rights & CASTLE_BK) s += 'k';
    if (rights & CASTLE_BQ) s += 'q';
    if (s.empty()) s = "-";
    return s;
}

static bool split_fen_fields(std::string_view fen, std::vector<std::string>& fields) {
    fields.clear();
    std::string cur;
    cur.reserve(fen.size());

    for (char ch : fen) {
        if (ch == ' ') {
            if (!cur.empty()) {
                fields.push_back(cur);
                cur.clear();
            }
        } else {
            cur.push_back(ch);
        }
    }
    if (!cur.empty()) fields.push_back(cur);

    // Standard FEN has 6 fields
    return fields.size() == 6;
}

bool from_fen(std::string_view fen, Position& out) {
    std::vector<std::string> fields;
    if (!split_fen_fields(fen, fields)) return false;

    const std::string& placement = fields[0];
    const std::string& stm       = fields[1];
    const std::string& castling  = fields[2];
    const std::string& ep        = fields[3];
    const std::string& halfmove  = fields[4];
    const std::string& fullmove  = fields[5];

    Position p; // build into temp; only assign to out on success
    // clear board
    for (int sq = 0; sq < 64; ++sq) p.set_piece(sq, EMPTY);

    // --- piece placement ---
    int rank = 7;
    int file = 0;

    for (size_t i = 0; i < placement.size(); ++i) {
        char c = placement[i];

        if (c == '/') {
            if (file != 8) return false;
            --rank;
            file = 0;
            if (rank < 0) return false;
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c))) {
            int empty = c - '0';
            if (empty < 1 || empty > 8) return false;
            file += empty;
            if (file > 8) return false;
            continue;
        }

        Piece pc = char_to_piece(c);
        if (pc == EMPTY) return false;

        if (file >= 8 || rank < 0) return false;
        int sq = make_square(file, rank);
        p.set_piece(sq, pc);
        ++file;
    }

    if (!(rank == 0 && file == 8)) {
        // Must finish exactly at end of rank 1
        return false;
    }

    // --- side to move ---
    if (stm == "w") p.set_side_to_move(WHITE);
    else if (stm == "b") p.set_side_to_move(BLACK);
    else return false;

    // --- castling rights ---
    uint8_t rights = 0;
    if (!parse_castling(castling, rights)) return false;
    p.set_castling_rights(rights);

    // --- en passant target ---
    if (ep == "-") {
        p.set_ep_square(-1);
    } else {
        auto sq = string_to_square(ep);
        if (!sq) return false;
        p.set_ep_square(static_cast<Square>(*sq));
    }

    // --- halfmove / fullmove ---
    try {
        int hm = std::stoi(halfmove);
        int fm = std::stoi(fullmove);
        if (hm < 0 || fm < 1) return false;

        p.set_halfmove_clock(static_cast<uint16_t>(hm));
        p.set_fullmove_number(static_cast<uint16_t>(fm));
    } catch (...) {
        return false;
    }

    // Basic sanity: make sure kings exist (optional but helps)
    // If you prefer to allow illegal FENs for testing, remove this.
    bool hasWK = false, hasBK = false;
    for (int sq = 0; sq < 64; ++sq) {
        Piece pc = p.at(sq);
        if (pc == WK) hasWK = true;
        if (pc == BK) hasBK = true;
    }
    if (!hasWK || !hasBK) return false;

    out = p;
    return true;
}

std::string to_fen(const Position& pos) {
    std::ostringstream oss;

    // --- placement ---
    for (int r = 7; r >= 0; --r) {
        int empty = 0;
        for (int f = 0; f < 8; ++f) {
            int sq = make_square(f, r);
            Piece pc = pos.at(sq);
            if (pc == EMPTY) {
                ++empty;
            } else {
                if (empty > 0) {
                    oss << empty;
                    empty = 0;
                }
                oss << piece_to_char(pc);
            }
        }
        if (empty > 0) oss << empty;
        if (r != 0) oss << '/';
    }

    // --- side to move ---
    oss << ' ' << (pos.side_to_move() == WHITE ? 'w' : 'b');

    // --- castling ---
    oss << ' ' << castling_to_string(pos.castling_rights());

    // --- en passant ---
    oss << ' ';
    if (pos.ep_square() == -1) oss << '-';
    else oss << square_to_string(pos.ep_square());

    // --- halfmove / fullmove ---
    oss << ' ' << pos.halfmove_clock();
    oss << ' ' << pos.fullmove_number();

    return oss.str();
}

} // namespace chess