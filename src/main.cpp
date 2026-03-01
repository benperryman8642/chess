#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "chess/game.h"
#include "chess/move.h"
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
        << "  draw?\n"
        << "  draw!\n"
        << "  no\n"
        << "  resign!\n"
        << "  quit\n";
}

static const char* color_name(chess::Color c) {
    return (c == chess::WHITE) ? "White" : "Black";
}

enum class ManualOutcome {
    None,
    DrawAgreed,
    WhiteResigned,
    BlackResigned
};

struct DrawOffer {
    bool pending = false;
    chess::Color offered_by = chess::WHITE;
};

static void print_status(const chess::Game& game, ManualOutcome outcome, const DrawOffer& offer) {
    const auto& pos = game.position();

    // Manual outcomes override everything else.
    switch (outcome) {
        case ManualOutcome::DrawAgreed:
            std::cout << "Game over: Draw by agreement.\n";
            return;
        case ManualOutcome::WhiteResigned:
            std::cout << "Game over: White resigned. Black wins.\n";
            return;
        case ManualOutcome::BlackResigned:
            std::cout << "Game over: Black resigned. White wins.\n";
            return;
        case ManualOutcome::None:
            break;
    }

    // Engine outcome (mate/stalemate/automatic draws)
    const auto res = game.status();
    switch (res) {
        case chess::GameResult::Checkmate:
            std::cout << "Checkmate! " << color_name(pos.side_to_move()) << " is checkmated.\n";
            return;
        case chess::GameResult::Stalemate:
            std::cout << "Stalemate.\n";
            return;
        case chess::GameResult::DrawFiftyMove:
            std::cout << "Draw by 50-move rule.\n";
            return;
        case chess::GameResult::DrawRepetition:
            std::cout << "Draw by repetition.\n";
            return;
        case chess::GameResult::Ongoing:
            break;
    }

    // Ongoing: show check, and any pending draw offer
    if (chess::in_check(pos, pos.side_to_move())) {
        std::cout << "Check.\n";
    }

    if (offer.pending) {
        std::cout << "Draw offer pending from " << color_name(offer.offered_by)
                  << ". Type 'draw!' to accept or 'no' to decline.\n";
    }
}

static void render(const chess::Game& game, ManualOutcome outcome, const DrawOffer& offer) {
    std::cout << game.position().ascii_board() << "\n";
    print_status(game, outcome, offer);
}

static bool is_game_over(const chess::Game& game, ManualOutcome outcome) {
    if (outcome != ManualOutcome::None) return true;
    return game.status() != chess::GameResult::Ongoing;
}

int main() {
    chess::Game game;

    ManualOutcome outcome = ManualOutcome::None;
    DrawOffer offer{};

    std::cout << "Chess CLI (type 'help')\n";
    render(game, outcome, offer);

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
            outcome = ManualOutcome::None;
            offer = {};
            render(game, outcome, offer);
        }
        else if (cmd == "board") {
            render(game, outcome, offer);
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
                outcome = ManualOutcome::None;
                offer = {};
                render(game, outcome, offer);
            }
        }
        else if (cmd == "moves") {
            if (is_game_over(game, outcome)) {
                std::cout << "Game is over. Use 'startpos' or 'setfen ...' to start a new game.\n";
                continue;
            }
            auto moves = game.legal_moves();
            std::cout << "Legal moves (" << moves.size() << "):\n";
            for (auto& m : moves) std::cout << chess::move_to_uci(m) << " ";
            std::cout << "\n";
        }
        else if (cmd == "play") {
            if (is_game_over(game, outcome)) {
                std::cout << "Game is over. Use 'startpos' or 'setfen ...' to start a new game.\n";
                continue;
            }

            std::string uci;
            iss >> uci;
            if (uci.empty()) {
                std::cout << "Usage: play e2e4\n";
                continue;
            }

            // If a draw offer is pending, playing a move implicitly declines it.
            if (offer.pending) {
                offer = {};
            }

            if (!game.play_uci(uci)) {
                std::cout << "Illegal move: " << uci << "\n";
            } else {
                render(game, outcome, offer);
            }
        }
        else if (cmd == "undo") {
            if (!game.undo()) {
                std::cout << "Nothing to undo\n";
            } else {
                // Undoing should also clear manual outcomes/offers (simple + intuitive)
                outcome = ManualOutcome::None;
                offer = {};
                render(game, outcome, offer);
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
        else if (cmd == "draw?") {
            if (is_game_over(game, outcome)) {
                std::cout << "Game is already over.\n";
                continue;
            }
            if (offer.pending) {
                std::cout << "A draw offer is already pending from " << color_name(offer.offered_by)
                          << ". Type 'draw!' to accept or 'no' to decline.\n";
                continue;
            }

            offer.pending = true;
            offer.offered_by = game.position().side_to_move();

            std::cout << color_name(offer.offered_by)
                      << " offers a draw. Opponent: type 'draw!' to accept or 'no' to decline.\n";
            // Turn does NOT change (we did not make a move)
            print_status(game, outcome, offer);
        }
        else if (cmd == "draw!") {
            if (is_game_over(game, outcome)) {
                std::cout << "Game is already over.\n";
                continue;
            }
            if (!offer.pending) {
                std::cout << "No draw offer is pending. Use 'draw?' to offer a draw.\n";
                continue;
            }

            outcome = ManualOutcome::DrawAgreed;
            offer = {};
            render(game, outcome, offer);
        }
        else if (cmd == "no") {
            if (is_game_over(game, outcome)) {
                std::cout << "Game is already over.\n";
                continue;
            }
            if (!offer.pending) {
                std::cout << "No draw offer is pending.\n";
                continue;
            }

            std::cout << "Draw offer declined.\n";
            offer = {};
            // Turn does NOT change
            print_status(game, outcome, offer);
        }
        else if (cmd == "resign!") {
            if (is_game_over(game, outcome)) {
                std::cout << "Game is already over.\n";
                continue;
            }

            const auto stm = game.position().side_to_move();
            outcome = (stm == chess::WHITE) ? ManualOutcome::WhiteResigned : ManualOutcome::BlackResigned;
            offer = {};
            render(game, outcome, offer);
        }
        else {
            std::cout << "Unknown command. Type 'help'.\n";
        }
    }

    return 0;
}