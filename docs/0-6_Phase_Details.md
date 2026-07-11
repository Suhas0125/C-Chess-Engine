# C Chess Engine Progress Summary

---

# Project Structure

```text
C_Chess_Engine/
│
├── assets/
│   └── pieces/
│       ├── White/Black piece PNGs
│
├── engine/
│   ├── attack.c / attack.h       (Attack & check detection)
│   ├── board.c / board.h         (Board state & representation)
│   ├── debug.c / debug.h         (Centralized debugging tools)
│   ├── directions.h              (Pre-calculated movement offsets)
│   ├── evaluate.c / evaluate.h   (Static position evaluation)
│   ├── fen.c / fen.h             (FEN import/export parsing)
│   ├── gamestate.c / gamestate.h (Mate & draw detection)
│   ├── history.c / history.h     (Game state history stack)
│   ├── legalmove.c / legalmove.h (Legal move filtering)
│   ├── makemove.c / makemove.h   (Move execution logic)
│   ├── move.c / move.h           (Move definitions & lists)
│   ├── movegen.c / movegen.h     (Pseudo-legal move generation)
│   ├── perft.c / perft.h         (Recursive move tree testing)
│   ├── pst.c / pst.h             (Piece-Square Tables)
│   ├── search.c / search.h       (Negamax & Alpha-Beta pruning)
│   └── tests.c / tests.h         (Automated Perft verification)
│
├── ui/
│   ├── renderer.c / renderer.h   (Raylib graphics & UI)
│
├── main.c
│
└── Makefile

```

---

# Phase 0 — Project Setup ✅

### Completed

* Raylib & VS Code configured
* Git & GitHub initialized
* Makefile & folder structure established

### Deliverable

A clean, compiling C project.

---

# Phase 1 — Board Representation ✅

### Completed (engine/board.h)

* Structs & Enums: `Piece`, `Side`, `CastlingRights`, `Board`
* State Tracking: Halfmove clock, fullmove number, en passant square
* Functions: `Board_Init()`, `Board_Print()`, `IsWhitePiece()`, `IsBlackPiece()`, `Board_Copy()`

### Deliverable

Complete internal chess position representation.

---

# Phase 2 — Rendering ✅

### Completed (ui/renderer.h)

* Structs: `PieceTextures`, `Renderer`
* Functions: `InitRenderer()`, `UnloadRenderer()`, `DrawGame()`
* Features: Chessboard drawing, piece rendering, selected square & legal move highlighting

### Deliverable

Fully playable graphical chessboard.

---

# Phase 3 — Move Generation ✅

### Completed (engine/move.h, movegen.h, directions.h)

* Move Structs: `MoveFlags`, `Move`, `MoveList`
* Move List Utils: `MoveList_Init()`, `MoveList_Add()`, `MoveList_Print()`, `Move_Print()`
* Offsets: `knightMoves`, `bishopDirections`, `rookDirections`, `queenDirections`, `kingDirections`
* Generators: `GenerateMoves()`, `GeneratePawnMoves()`, `GenerateEnPassantMoves()`, `GenerateSlidingMoves()`, `GenerateKnightMoves()`, `GenerateBishopMoves()`, `GenerateRookMoves()`, `GenerateQueenMoves()`, `GenerateKingMoves()`

### Deliverable

Complete pseudo-legal move generator.

---

# Phase 4 — Legal Move Generation ✅

### Completed (engine/attack.h, history.h, makemove.h, legalmove.h)

* Attack Detection: `IsSquareAttacked()`, `IsKingInCheck()`
* History Stack: `History`, `History_Init()`, `History_Push()`, `History_Pop()`, `PositionEquals()`
* Move Execution: `MakeMove()`, `UndoMove()`
* Legal Filtering: `GenerateLegalMoves()`

### Deliverable

Fully legal move generation handling self-checks, pins, and castling rules.

---

# Phase 5 — Search & Evaluation ✅

### Completed (engine/evaluate.h, pst.h, search.h, gamestate.h)

* Evaluation: `EvaluatePosition()`, `PawnPST` through `KingPST` arrays
* Search: `SearchResult`, `Negamax()`, `SearchBestMove()`, `MakeEngineMove()`
* Game State: `IsCheckmate()`, `IsStalemate()`, `IsDrawByFiftyMoveRule()`, `IsDrawByThreefoldRepetition()`

### Deliverable

The engine evaluates positions, searches several plies, and plays complete games.

---

# Phase 5.5 — Verification & Debugging Infrastructure ✅

### Completed (engine/fen.h, perft.h, tests.h, debug.h)

* FEN Parsing: `Board_ToFEN()`, `Board_FromFEN()`, `PrintFEN()`
* Debug Utilities: Centralized board and move inspection.
* Perft Testing: `Perft()`, `PerftDivide()`
* Automated Test Suite: `RunAutomatedPerftSuite()` verifying Start, Kiwipete, Positions 3, 4, and 5.

### Deliverable

A mathematically verified legal move generator. Successfully calculated over 6.8 million leaf nodes. Move generation is officially 100% bug-free.

---

## Phase 6 - Search Optimization & Heuristics ✅

### 6A: Quiescence Search
* **Files:** `engine/search.c`
* **`QuiescenceSearch()`:** Evaluates only tactical capture moves at depth 0 to resolve the Horizon Effect and prevent blind blunders.
* **`Negamax()`:** Updated to transition into Q-Search instead of static evaluation at depth 0.
* **`SearchBestMove()`:** Fixed root Alpha-Beta boundaries (`INFINITY_SCORE`) to prevent integer negation overflow bugs.

### 6B: Basic Move Ordering (MVV-LVA)
* **Files:** `engine/evaluate.h`, `engine/evaluate.c`, `engine/search.c`
* **`GetPieceValue()`:** Un-statted and exposed globally from `evaluate.c` to share piece values.
* **`SortMoves()`:** Sorts legal moves so Alpha-Beta evaluates good captures (e.g., Pawn takes Queen) first, maximizing pruning efficiency and preventing Q-Search explosions.

### 6C: Iterative Deepening & Principal Variation (PV)
* **Files:** `engine/search.h`, `engine/search.c`
* **`Negamax()`:** Added a `ply` parameter and logic to record the "best line" (PV) in a triangular array (`pvArray`).
* **`SearchBestMove()`:** Loops depths incrementally (1 to max), feeding the previous depth's PV into the next iteration to search the absolute best move first. Outputs UCI-style `info depth... pv...`.

### 6D: Zobrist Hashing
* **Files:** `engine/zobrist.h`, `engine/zobrist.c`, `engine/board.h`, `engine/makemove.c`
* **`Zobrist_Init()`:** Generates 64-bit pseudo-random numbers for all piece-square combinations and board rights.
* **`Zobrist_GenerateKey()`:** XORs board features to create a mathematically unique 64-bit integer ID for any board position.
* **`Board` / `MakeMove()`:** Added `hashKey` to the board struct; updated `MakeMove()` to recalculate the hash automatically.

### 6E: Transposition Tables (TT)
* **Files:** `engine/tt.h`, `engine/tt.c`, `engine/search.c`
* **`TT_Init()`, `TT_Store()`, `TT_Probe()`:** Allocates a dynamic hash table to save and retrieve evaluations (EXACT, ALPHA, BETA bounds) and best moves for previously calculated positions.
* **`Negamax()`:** Updated to probe the TT early for instant Alpha-Beta cutoffs, and to store the correct fail-soft bound at the end of the node evaluation.

### 6F: Advanced Move Ordering (Killers & History)
* **Files:** `engine/search.c`
* **`MovesEqual()`, `IsKillerMove()`:** Helper functions to match move structures and detect previous cutoffs.
* **`Negamax()`:** Updated to record successful quiet moves (Killers) and globally successful quiet moves (History) upon triggering a beta cutoff.
* **`SortMoves()`:** Overhauled to score and sort in strict priority: PV/TT Move -> Captures (MVV-LVA) -> Promotions -> Killer Moves -> History Heuristic.
---

# Phase 7 — UCI & Engine Polish

Final phase includes:

* UCI protocol (Arena, CuteChess compatibility)
* Time management
* Engine options
* Profiling & Documentation

### Goal

A complete, standalone, publicly distributable chess engine.

---

# Current Status

**Completed**
✅ Phase 0
✅ Phase 1
✅ Phase 2
✅ Phase 3
✅ Phase 4
✅ Phase 5
✅ Phase 5.5
✅ Phase 6
