# chess
chess — a small C++20 chess project for development and testing

Overview
--------

This repository contains a compact C++ implementation and tools for experimenting with chess move generation, perft testing, and related utilities. It is intended as a development playground rather than a finished chess engine.

Key points
----------
- Language: Modern C++ (C++20 required)
- Build system: Makefile-based; simple, portable build to produce command-line binaries
- Primary goals: correct move generation, perft test harnesses, unit tests, and simple CLI

Repository structure
-------------------

Top-level layout:

- `include/` — public headers used by the project (APIs and type definitions)
	- `chess/*.h` — core headers (position, movegen, makemove, perft, zobrist, etc.)
- `src/` — implementation source files (.cpp)
- `tests/` — C++ test sources (unit tests and perft tests)
- `data/` — test data and perft suites
- `build/` — build artifacts (created by the Makefile)
	- `build/bin/` — compiled binaries (e.g. `chess_cli`, `perft_tests`, `unit_tests`)
- `scripts/` — helper scripts (e.g. `run_perft.sh`), currently unused
- `tools/` — small utilities (e.g. perft suite converters)
- `README.md`, `Makefile`, `LICENSE` — project metadata

Important files
---------------
- `src/main.cpp` — small CLI entry point
- `src/perft.cpp`, `tests/perft_tests.cpp` — perft implementation and tests
- `include/chess/` — headers describing public interfaces

CLI commands
------------
The repository includes a simple interactive CLI (`chess_cli`) implemented in `src/main.cpp`. The CLI supports the following commands:

- `help` — show the available commands.
- `startpos` — reset the game to the standard chess starting position.
- `board` — print the board (ASCII) and current game status.
- `fen` — print the current position as a FEN string.
- `setfen <fen...>` — set the current position using a FEN string.
- `moves` — list legal moves from the current position (in UCI format).
- `play <uci>` — play a move in UCI format (e.g. `e2e4`).
- `undo` — undo the last move.
- `perft <depth>` — run a perft node count from the current position to the given depth and print the total node count.
- `divide <depth>` — perft divide: list each move from the current position with its perft count at the specified depth.
- `draw?` — offer a draw to the opponent (pending until accepted/rejected).
- `draw!` — accept a pending draw offer (results in a draw by agreement).
- `no` — decline a pending draw offer.
- `resign!` — resign the game immediately (current side loses).
- `quit` / `exit` — exit the CLI.

These commands are intentionally simple and intended for interactive testing, perft exploration, and manual playtesting.

Requirements
------------
- A C++20-compatible compiler (GCC 10+, Clang 12+, or MSVC with C++20 support)
- `make` and a POSIX-like shell (macOS / Linux). On macOS, the default `zsh` works fine.
- Recommended: build tools and standard library that support <filesystem> and modern C++ features.

Build and quick start
---------------------

This project provides a Makefile with several convenient targets. From the repository root you can run:

```zsh
# Build (default: debug build)
make

# Explicit debug or release builds
make debug    # debug build (adds -g -O0)
make release  # optimized release build (-O3)

# Build and run the test runners (perft + unit tests)
make test
make test-smoke  # quick perft smoke tests (uses data/perft_suite.txt)
make test-full   # thorough perft run (uses data/perftsuite_extended.txt) - builds release

# Clean build artifacts
make clean
```

The build products are written to `build/bin/`. Example binaries produced by the Makefile include `chess_cli`, `perft_tests`, and `unit_tests`.

Run the CLI and tests (after building):

```zsh
# Run the CLI (if present)
./build/bin/chess_cli

# Run test binaries directly
./build/bin/unit_tests
./build/bin/perft_tests data/perft_suite.txt

# or use the helper script to run a perft suite
scripts/run_perft.sh data/perft_suite.txt
```

Alternative (manual compilation)
--------------------------------

If you prefer not to use the Makefile, you can compile with a C++20 compiler directly. This is useful for quick experiments, but the Makefile handles object separation and test targets for you.

```zsh
g++ -std=c++20 -O2 -Iinclude src/*.cpp -o build/bin/chess_cli
```

Limitations and notes
---------------------
- This project is primarily a development/testing repository. It focuses on move generation and correctness (perft), not on a full-featured, optimized engine.
- No GUI or network play is provided.
- The code may be single-threaded and not performance-optimized.
- Rule edge-cases, promotions, and specialized chess variants may not be fully supported — consult the headers in `include/chess/` for current behavior.
- Tests and data are provided for regression/perft validation; coverage may be incomplete.

Contributing
------------
Contributions, bug reports and pull requests are welcome. Please:

- Open an issue describing the bug or feature.
- Provide small, focused pull requests with tests where appropriate.

License
-------
See the `LICENSE` file in the repository root for license terms.

Contact / Support
-----------------
For questions about the codebase, open an issue or contact the repository owner.

Acknowledgements
----------------
This project exists as a personal development repository to experiment with chess programming techniques.

