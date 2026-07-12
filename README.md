# C Chess Engine

## Overview

A custom, fully autonomous, and UCI-compliant chess engine built entirely from scratch in C. This project features a strict architectural separation between the internal chess data model (the "engine"), the Raylib-based graphical user interface (the "renderer"), and the artificial intelligence (the "search/evaluation").

## Technical Stack

* **Language:** C
* **Graphics Framework:** Raylib
* **Compiler/Toolchain:** GCC via w64DevKit
* **Build System:** Makefile

## How to Build and Run

The project uses a Makefile to automate compilation.
To build the executable, run:

```bash
make

```

To build and immediately launch the graphical engine, run:

```bash
make run

```

*(Note: The engine can also be run in headless UCI mode for competitive play against other engines by launching the executable with the `uci` argument).*

---

## The Engineering Journey & Core Architecture

### Phase 0: Project Setup & Toolchain

* **Concept:** Established a clean, reproducible development environment. Development tools (Raylib, GCC) are kept strictly outside the project repository to maintain a lightweight footprint.
* **Implementation:** Created a `Makefile` to automate the multi-file build process and configured VS Code IntelliSense to recognize Raylib headers. Initialized version control using Git with a strict `.gitignore`.

### Phase 1: Core Board Representation

* **Concept:** Designed the foundational internal data model using an 8x8 2D array mapped to standard chess coordinates.
* **Key Files:** `engine/board.c`, `engine/board.h`
* **Important Functions:**
* `Board_Init()`: Resets the board and populates it with the standard chess starting position, initializing all game state trackers (halfmove clock, castling rights, etc.).
* `Board_Copy()`: Safely duplicates the exact state of a board. This is vital for temporary testing without corrupting the main game state.
* `Board_Print()`: A terminal-based visualization tool used for early headless debugging.



### Phase 2: Graphical UI & Rendering

* **Concept:** Integrated Raylib to create a perfect 1:1 mapping between internal engine coordinates and screen pixels, strictly enforcing that the UI remains stateless and never modifies engine data.
* **Key Files:** `ui/renderer.c`, `ui/renderer.h`
* **Important Functions:**
* `DrawPiece()`: Maps abstract internal `Piece` enums to their corresponding loaded PNG textures and centers them mathematically inside the dynamically scaled 8x8 screen grid.
* *Texture Management:* A centralized `PieceTextures` struct is used to load, cache, and safely unload the converted SVG-to-PNG assets at the end of the program lifecycle.



### Phase 3: Pseudo-Legal Move Generation & Execution

* **Concept:** Built the core physical rules of the board. The engine calculates every piece's theoretical movement (including sliding rays for Rooks/Bishops/Queens) without yet caring about King safety.
* **Key Files:** `engine/movegen.c`, `engine/makemove.c`
* **Important Functions:**
* `GenerateMoves(Board *board, MoveList *list)`: The centralized entry point that delegates generation to piece-specific logic (e.g., `GeneratePawnMoves`) and populates a 256-slot move array.
* `MakeMove(Board *board, const Move *move)`: The critical physical state-updater. It removes pieces from their origin, places them at the destination, handles complex side-effects (like moving the Rook during castling or removing the captured pawn during en passant), and toggles the side-to-move.



### Phase 4: Legal Move Filtering & Game State Rules

* **Concept:** Implemented the logic required to enforce King safety and detect game-ending scenarios (Checkmate, Stalemate, 50-Move Rule, Repetition).
* **Key Files:** `engine/gamestate.c`, `engine/history.c`, `engine/legalmove.c`
* **Important Functions:**
* `IsSquareAttacked()`: Uses reverse ray-tracing to scan outward from a target square to detect if any enemy pieces threaten it.
* `UndoMove(Board *board, History *history)`: Pops the last fully saved board state off the custom History stack, flawlessly reverting complex board mutations.
* `GenerateLegalMoves()`: The ultimate filter. It loops through `GenerateMoves()`, temporarily executes each one via `MakeMove()`, uses `IsKingInCheck()` to verify safety, and calls `UndoMove()` before storing the truly legal moves.



### Phase 5: Search, Evaluation & Gameplay Loop

* **Concept:** Gave the engine its "brain" by writing a static evaluation function to judge positions and a recursive search algorithm to look into the future.
* **Key Files:** `engine/evaluate.c`, `engine/search.c`
* **Important Functions:**
* `EvaluatePosition()`: Scans the board to calculate material advantages and applies Piece-Square Tables (PST) to determine positional strength (e.g., rewarding Knights in the center).
* `Negamax()`: The recursive core of the AI. It explores future board states and uses Alpha-Beta pruning to instantly cut off branches of the search tree that are mathematically worse than previously found lines.
* `SearchBestMove()`: The root caller that orchestrates the search, manages the alpha-beta bounds, and returns a clean `SearchResult` struct containing the best move, evaluation score, and total nodes calculated.



### Phase 5.5: Verification & Debugging Infrastructure

* **Concept:** Built the tools necessary to mathematically prove the engine's move generator is entirely bug-free.
* **Key Files:** `engine/perft.c`, `engine/fen.c`, `engine/debug.c`
* **Important Functions:**
* `Perft()` and `PerftDivide()`: Recursively walks the move tree to depth *N* to blindly count leaf nodes. Testing against the famous "Kiwipete" position (calculating millions of nodes) proved the move generation is 100% correct.
* `Board_FromFEN()` and `Board_ToFEN()`: Parses standard Forsyth-Edwards Notation (FEN) strings to instantly load or export complex debugging positions.



### Phase 6: Advanced Search & Optimization

* **Concept:** Vastly improved the speed and tactical depth of the AI using advanced heuristics, allowing it to search much deeper in less time.
* **Key Files:** `engine/tt.c`, `engine/zobrist.c`, `engine/search.c`
* **Important Functions:**
* `Zobrist_GenerateKey()`: XORs pre-generated 64-bit random strings based on piece placement to create a unique hash for any board position.
* `TT_Probe()` and `TT_Store()`: Interfaces with the Transposition Table to cache and instantly retrieve evaluations for previously seen positions.
* `QuiescenceSearch()`: A specialized, capture-only search called at the end of the main `Negamax` depth to resolve forcing tactical sequences and prevent the engine from blundering due to the Horizon Effect.
* `SortMoves()`: Orders the move list before searching. Prioritizes Principal Variation (PV) moves, uses MVV-LVA (Most Valuable Victim - Least Valuable Attacker) for captures, and applies Killer/History heuristics for quiet moves to maximize Alpha-Beta cutoffs.



### Phase 7: UCI Protocol, Time Management & Polish

* **Concept:** Prepared the engine for the outside world by making it compliant with standard chess GUI software (CuteChess, Arena) and giving it an internal clock.
* **Key Files:** `engine/uci.c`, `engine/time_utils.c`
* **Important Functions:**
* `UCI_Loop()`: A continuous `stdin` reading loop that intercepts standard GUI text commands (`isready`, `position`, `go`) and routes them to the engine.
* `ParseAndMakeMove()`: Translates standard algebraic string notation (e.g., `e2e4`) from external GUIs into internal `Move` structs.
* `GetTimeMs()`: Wraps OS-specific high-resolution clocks (`GetTickCount64` for Windows, `gettimeofday` for POSIX) to dynamically monitor elapsed time.
* `CheckTime()`: A hook inside `Negamax` that polls the clock every 2048 nodes. If the allocated time runs out, it aborts the search and safely returns the best move found in the previous Iterative Deepening depth.