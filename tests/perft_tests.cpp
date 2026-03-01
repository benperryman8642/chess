#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <chrono>

#include "chess/fen.h"
#include "chess/perft.h"
#include "chess/position.h"

struct TestCase {
    std::string fen;
    int depth = 0;
    std::uint64_t expected = 0;
};

static inline std::string trim(std::string s) {
    auto is_space = [](unsigned char c) { return std::isspace(c); };
    while (!s.empty() && is_space(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    while (!s.empty() && is_space(static_cast<unsigned char>(s.back()))) s.pop_back();
    return s;
}

static bool split3(const std::string& line, std::string& a, std::string& b, std::string& c) {
    size_t p1 = line.find(';');
    if (p1 == std::string::npos) return false;
    size_t p2 = line.find(';', p1 + 1);
    if (p2 == std::string::npos) return false;

    a = trim(line.substr(0, p1));
    b = trim(line.substr(p1 + 1, p2 - (p1 + 1)));
    c = trim(line.substr(p2 + 1));
    return !(a.empty() || b.empty() || c.empty());
}

static bool load_suite(const std::string& path, std::vector<TestCase>& out) {
    std::ifstream in(path);
    if (!in) return false;

    out.clear();

    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::string fen, depth_s, nodes_s;
        if (!split3(line, fen, depth_s, nodes_s)) {
            std::cerr << "Bad line (expected: FEN ; depth ; nodes): " << line << "\n";
            return false;
        }

        TestCase tc;
        tc.fen = fen;
        try {
            tc.depth = std::stoi(depth_s);
            tc.expected = static_cast<std::uint64_t>(std::stoull(nodes_s));
        } catch (...) {
            std::cerr << "Bad numbers in line: " << line << "\n";
            return false;
        }

        out.push_back(std::move(tc));
    }

    return true;
}

static void print_progress(size_t done, size_t total) {
    // prints on one line using carriage return
    int pct = (total == 0) ? 100 : static_cast<int>((done * 100) / total);
    std::cout << "\r[" << done << "/" << total << "] " << pct << "% complete" << std::flush;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: perft_tests <path_to_suite>\n";
        return 2;
    }

    const std::string path = argv[1];

    std::vector<TestCase> tests;
    if (!load_suite(path, tests)) {
        std::cerr << "Failed to load suite: " << path << "\n";
        return 2;
    }

    std::cout << "Loaded " << tests.size() << " perft tests from " << path << "\n";

    const size_t total = tests.size();
    size_t passed = 0;

    // Print progress every N tests (tune as you like)
    const size_t progress_every = 5;

    auto t0 = std::chrono::steady_clock::now();
    print_progress(0, total);

    for (size_t i = 0; i < total; ++i) {
        const auto& tc = tests[i];

        chess::Position pos;
        if (!chess::from_fen(tc.fen, pos)) {
            std::cout << "\n";
            std::cerr << "FEN parse failed at test " << (i + 1) << ":\n" << tc.fen << "\n";
            return 2;
        }

        chess::Position copy = pos;
        std::uint64_t got = chess::perft(copy, tc.depth);

        if (got != tc.expected) {
            std::cout << "\n";
            std::cerr << "FAIL test " << (i + 1) << "/" << total << "\n";
            std::cerr << "  depth:    " << tc.depth << "\n";
            std::cerr << "  FEN:      " << tc.fen << "\n";
            std::cerr << "  expected: " << tc.expected << "\n";
            std::cerr << "  got:      " << got << "\n";
            return 1;
        }

        ++passed;

        // Progress output
        if ((i + 1) % progress_every == 0 || (i + 1) == total) {
            print_progress(i + 1, total);
        }
    }

    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    std::cout << "\nPerft tests passed: " << passed << "/" << total
              << " in " << ms << " ms\n";
    return 0;
}