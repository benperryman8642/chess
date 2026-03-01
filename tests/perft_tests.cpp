#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "chess/fen.h"
#include "chess/perft.h"
#include "chess/position.h"

static inline std::string trim(std::string s) {
    auto is_space = [](unsigned char c) { return std::isspace(c); };
    while (!s.empty() && is_space(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    while (!s.empty() && is_space(static_cast<unsigned char>(s.back()))) s.pop_back();
    return s;
}

static bool split3(const std::string& line, std::string& a, std::string& b, std::string& c) {
    // split by ';' into exactly 3 parts
    size_t p1 = line.find(';');
    if (p1 == std::string::npos) return false;
    size_t p2 = line.find(';', p1 + 1);
    if (p2 == std::string::npos) return false;

    a = trim(line.substr(0, p1));
    b = trim(line.substr(p1 + 1, p2 - (p1 + 1)));
    c = trim(line.substr(p2 + 1));
    return !(a.empty() || b.empty() || c.empty());
}

int main() {
    const std::string path = "data/perft_suite.txt";
    std::ifstream in(path);
    if (!in) {
        std::cerr << "Failed to open " << path << "\n";
        return 2;
    }

    int total = 0;
    int passed = 0;

    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::string fen, depth_s, nodes_s;
        if (!split3(line, fen, depth_s, nodes_s)) {
            std::cerr << "Bad line (expected: FEN ; depth ; nodes): " << line << "\n";
            return 2;
        }

        int depth = 0;
        std::uint64_t expected = 0;
        try {
            depth = std::stoi(depth_s);
            expected = static_cast<std::uint64_t>(std::stoull(nodes_s));
        } catch (...) {
            std::cerr << "Bad numbers in line: " << line << "\n";
            return 2;
        }

        chess::Position pos;
        if (!chess::from_fen(fen, pos)) {
            std::cerr << "FEN parse failed: " << fen << "\n";
            return 2;
        }

        ++total;
        chess::Position copy = pos;
        auto got = chess::perft(copy, depth);

        if (got != expected) {
            std::cerr << "FAIL perft depth " << depth << "\n";
            std::cerr << "  FEN:      " << fen << "\n";
            std::cerr << "  expected: " << expected << "\n";
            std::cerr << "  got:      " << got << "\n";
            return 1;
        } else {
            ++passed;
        }
    }

    std::cout << "Perft tests passed: " << passed << "/" << total << "\n";
    return 0;
}