# Phase 6 - Development Log (Notes)

## Step 6A — Quiescence Search

### Goal
Fix the Horizon Effect by extending the search beyond depth 0 for tactical capture sequences, ensuring the engine does not blunder pieces right after the search depth ends.

### Files Modified
engine/
└── search.c

### Features
- Implemented `QuiescenceSearch()` to evaluate "stand-pat" static scores.
- Restricts move generation at depth 0 to evaluate *only* `MOVE_CAPTURE` flags.
- Replaced the static `EvaluatePosition()` call in `Negamax()` with Q-Search.
- Fixed root `alpha`/`beta` bounds, replacing `INT_MIN` with safe bounds (`INFINITY_SCORE`) to prevent two's-complement negation overflow breaking the Alpha-Beta window.

### Result & Developer Notes
The engine dynamically calculates forcing tactical lines until the board is quiet. **However, initially implementing this caused the Raylib window to freeze.** Because Alpha-Beta searches moves in the order they are generated, Q-Search was exploring absurd captures (like Queen taking a defended pawn) first. This caused a massive node explosion (millions of calculations), locking up the main thread and freezing the OS window.

---

## Step 6B — Basic Move Ordering (MVV-LVA)

### Goal
Prevent search tree explosions by ensuring Alpha-Beta pruning evaluates the most promising captures first, directly fixing the Q-Search freeze.

### Files Modified
engine/
├── evaluate.h
├── evaluate.c
└── search.c

### Features
- Refactored `GetPieceValue()` to be globally accessible, removing code duplication.
- Implemented `SortMoves()` using the MVV-LVA (Most Valuable Victim - Least Valuable Attacker) heuristic.
- Injected sorting logic immediately after move generation in `Negamax`, `QuiescenceSearch`, and `SearchBestMove`.

### Result & Developer Notes
The engine now prioritizes high-value captures (e.g., Pawn takes Queen) and delays bad captures. This drastically increases the number of Alpha-Beta cutoffs. **This completely fixed the Raylib crash.** By searching good moves first, the engine prunes the bad branches instantly, bringing the node count on complex positions (like Kiwipete Depth 3) down to a stable ~8,759 nodes (compared to a raw baseline of 2,421 nodes without Q-Search). Calculations finish in milliseconds, and the UI remains perfectly responsive.

## Step 6C — Iterative Deepening & Principal Variation (PV)

### Goal
Dramatically increase Alpha-Beta pruning efficiency by extracting the "best line" (PV) and searching it first at deeper depths, while providing UCI-style engine thought output.

### Files Modified
engine/
├── search.h
└── search.c

### Features
- Implemented a triangular PV Table (`pvArray`, `pvLength`) to dynamically record the best sequence of moves during `Negamax`.
- Rewrote `SearchBestMove()` to use Iterative Deepening (looping from depth 1 to max), carrying the PV line over between iterations.
- Updated `SortMoves()` to identify the `pvMove` and boost its score to absolute maximum priority (2,000,000).
- Added UCI-formatted `info depth... pv...` logging to visualize the engine's thought process.

### Result
By guaranteeing the absolute best move from Depth N-1 is searched first at Depth N, Alpha-Beta pruning achieves near-perfect cutoffs at the root and main branches. The engine now seamlessly outputs its predicted lines, operating much faster and deeper.

## Step 6D — Zobrist Hashing

### Goal
Assign a unique 64-bit identifier to every possible board state as a prerequisite for Transposition Tables.

### Files Modified / Created
engine/
├── zobrist.h (New)
├── zobrist.c (New)
├── board.h
└── makemove.c

### Features
- Implemented a 64-bit Pseudo-Random Number Generator (PRNG) in `zobrist.c`.
- Generated random bitstrings for all piece-square combinations, side-to-move, castling rights, and en passant squares (`Zobrist_Init`).
- Implemented `Zobrist_GenerateKey()` to XOR these features into a unique 64-bit integer.
- Updated the `Board` struct to track `hashKey`, and updated `MakeMove()` to calculate the new key after a move is made.

### Result
The engine can now instantly identify board positions. Restoring state via `UndoMove()` flawlessly restores the correct `hashKey`. This provides the foundational architecture required to cache search evaluations in Phase 6E.

## Step 6E — Transposition Tables (TT)

### Goal
Cache evaluated positions to instantly retrieve scores for transposed positions and vastly accelerate move ordering.

### Files Modified / Created
engine/
├── tt.h (New)
├── tt.c (New)
└── search.c

### Features
- Implemented a fixed-size, dynamically allocated hash table (`TTEntry` array) mapped to Megabytes.
- Created `TT_Store()` to save `hashKey`, `depth`, `score`, `flag` (EXACT, ALPHA, BETA), and the `bestMove`.
- Created `TT_Probe()` to retrieve cached scores for instant Alpha-Beta cutoffs.
- Corrected fail-soft bounds in `Negamax` to ensure precise mathematical pruning without search corruption.

### Result
The engine no longer researches identical board states reached via different move orders. Node counts at Depth 5 dropped by ~30% (~30k to ~21k), massively accelerating deep calculations.

## Step 6F — Advanced Move Ordering (Killer & History Heuristics)

### Goal
Order "quiet" (non-capture) moves dynamically to maximize Alpha-Beta cutoffs when no tactical captures are available.

### Files Modified
engine/
└── search.c

### Features
- **Killer Heuristic:** Added a `killerMoves[MAX_PLY][2]` array to store the two most recent quiet moves that caused a beta cutoff at a specific ply.
- **History Heuristic:** Added a `historyTable[side][fromSq][toSq]` array to reward quiet moves that successfully cause cutoffs globally, weighted by depth squared.
- **SortMoves Overhaul:** Completely rewrote the scoring logic to prioritize moves in this exact order: 
  1) PV / TT Move (Absolute priority)
  2) Captures via MVV-LVA
  3) Promotions
  4) Killer Moves
  5) History Heuristic (dynamic scoring for remaining quiet moves).

### Result
The engine now searches optimal positional moves early, reducing the node count on quiet branches. Node counts consistently dropped across all depths (e.g., Depth 2 dropped by 20%). This final layer of ordering pushes the search efficiency to its mathematical limit, allowing the engine to comfortably reach Depth 6+ searching only ~67k nodes. **Phase 6 Search Improvements is officially complete.**

