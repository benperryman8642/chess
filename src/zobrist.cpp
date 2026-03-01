#include "chess/zobrist.h"

#include <array>
#include <cstdint>

#include "chess/types.h"

namespace chess {

// SplitMix64 PRNG (deterministic with fixed seed)
static std::uint64_t splitmix64(std::uint64_t& x) {
    std::uint64_t z = (x += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}

struct ZKeys {
    // piece_keys[piece][square]
    std::array<std::array<std::uint64_t, 64>, 13> piece_keys{};
    std::array<std::uint64_t, 16> castling_keys{}; // castling rights bitmask 0..15
    std::array<std::uint64_t, 9> ep_file_keys{};   // 0..7 for file, 8 = none
    std::uint64_t side_key = 0;                    // toggled when black to move
};

static const ZKeys& keys() {
    static ZKeys k = []{
        ZKeys z{};
        std::uint64_t seed = 0xC0FFEE123456789Full; // fixed seed => stable hashes

        for (int p = 0; p < 13; ++p) {
            for (int sq = 0; sq < 64; ++sq) {
                z.piece_keys[p][sq] = splitmix64(seed);
            }
        }
        for (int i = 0; i < 16; ++i) z.castling_keys[i] = splitmix64(seed);
        for (int i = 0; i < 9; ++i)  z.ep_file_keys[i] = splitmix64(seed);

        z.side_key = splitmix64(seed);
        return z;
    }();
    return k;
}

std::uint64_t zobrist_key(const Position& pos) {
    const auto& k = keys();

    std::uint64_t h = 0;

    // Pieces
    for (int sq = 0; sq < 64; ++sq) {
        Piece p = pos.at(sq);
        if (p != EMPTY) h ^= k.piece_keys[p][sq];
    }

    // Side to move
    if (pos.side_to_move() == BLACK) h ^= k.side_key;

    // Castling rights (0..15)
    h ^= k.castling_keys[pos.castling_rights() & 0x0F];

    // En-passant: hash file only (standard approach)
    // If none: index 8
    int ep_idx = 8;
    if (pos.ep_square() != -1) ep_idx = file_of(pos.ep_square()); // 0..7
    h ^= k.ep_file_keys[ep_idx];

    return h;
}

} // namespace chess