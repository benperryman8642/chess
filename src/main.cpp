#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "chess/game.h"
#include "chess/move.h"
#include "chess/movegen.h"
#include "chess/perft.h"
#include "chess/rules.h"

static void print_help() {
    std::cout
        << "Commands:\n"
        << "  help\n"
        << "  startpos\n"
        << "  board\n"
        << "  fen\n"
        << "  setfen <fen...>\n"
        << "  moves\n"
        << "  play <uci>\n"
        << "  undo\n"
        << "  perft <depth>\n"
        << "  divide <depth>\n"
        << "  quit\n";
}

static void print_status(const chess::Game& game) {
    const auto& pos = game.position();
    auto res = chess::result(pos);

    if (res == chess::GameResult::Checkmate) {
        std::cout << "Checkmate! "
                  << (pos.side_to_move() == chess::WHITE ? "White" : "Black")
                  << " is checkmated.\n";
    } else if (res == chess::GameResult::Stalemate) {
        std::cout << "Stalemate.\n";
    } else {
        if (chess::in_check(pos, pos.side_to_move())) {
            std::cout << "Check.\n";
        }
    }
}

int main() {
    chess::Game game;

    std::cout << "Chess CLI (type 'help')\n";
    std::cout << game.position().ascii_board() << "\n";
    print_status(game);

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd.empty()) continue;

        if (cmd == "help") {
            print_help();
        }
        else if (cmd == "quit" || cmd == "exit") {
            break;
        }
        else if (cmd == "startpos") {
            game.reset_startpos();
            std::cout << game.position().ascii_board() << "\n";
            print_status(game);
        }
        else if (cmd == "board") {
            std::cout << game.position().ascii_board() << "\n";
            print_status(game);
        }
        else if (cmd == "fen") {
            std::cout << game.fen() << "\n";
        }
        else if (cmd == "setfen") {
            std::string rest;
            std::getline(iss, rest);
            while (!rest.empty() && rest.front() == ' ') rest.erase(rest.begin());

            if (!game.set_fen(rest)) {
                std::cout << "Invalid FEN\n";
            } else {
                std::cout << game.position().ascii_board() << "\n";
                print_status(game);
            }
        }
        else if (cmd == "moves") {
            auto moves = game.legal_moves();
            std::cout << "Legal moves (" << moves.size() << "):\n";
            for (auto& m : moves) std::cout << chess::move_to_uci(m) << " ";
            std::cout << "\n";
        }
        else if (cmd == "play") {
            std::string uci;
            iss >> uci;
            if (uci.empty()) {
                std::cout << "Usage: play e2e4\n";
                continue;
            }
            if (!game.play_uci(uci)) {
                std::cout << "Illegal move: " << uci << "\n";
            } else {
                std::cout << game.position().ascii_board() << "\n";
                print_status(game);
            }
        }
        else if (cmd == "undo") {
            if (!game.undo()) {
                std::cout << "Nothing to undo\n";
            } else {
                std::cout << game.position().ascii_board() << "\n";
                print_status(game);
            }
        }
        else if (cmd == "perft") {
            int depth;
            iss >> depth;
            if (!iss || depth < 0) {
                std::cout << "Usage: perft <depth>\n";
                continue;
            }
            chess::Position copy = game.position();
            std::cout << "perft(" << depth << ") = " << chess::perft(copy, depth) << "\n";
        }
        else if (cmd == "divide") {
            int depth;
            iss >> depth;
            if (!iss || depth <= 0) {
                std::cout << "Usage: divide <depth>\n";
                continue;
            }
            chess::Position copy = game.position();
            chess::perft_divide(copy, depth);
        }
        else {
            std::cout << "Unknown command. Type 'help'.\n";
        }
    }

    return 0;
}