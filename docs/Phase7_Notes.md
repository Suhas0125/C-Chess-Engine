# Phase 7 Development Log

## Step 7A — The UCI Communication Loop

### Goal
Establish the Universal Chess Interface (UCI) communication loop so the engine can receive text commands from external chess GUIs (like Arena or CuteChess) and send standard responses.

### Files Modified
engine/
├── uci.h (New)
└── uci.c (New)
main.c

### Features
- Created `UCI_Loop()` to continuously read standard input (`stdin`) from the GUI.
- Implemented basic UCI handshake commands: `uci`, `isready`, and `quit`.
- Updated `main.c` to parse command-line arguments (e.g., `./game.exe uci`), allowing the engine to boot in either headless UCI mode or the default Raylib UI mode.
- Added `fflush(stdout)` after every output to ensure messages are immediately sent to the GUI without getting trapped in the output buffer.

### Result & Developer Notes
The engine successfully boots into a headless terminal mode, bypassing the graphical window when given the `uci` flag. It correctly answers the standard handshakes and terminates cleanly. Intercepting the launch sequence in `main.c` was crucial here, as standard chess GUIs expect a purely text-based background process and will fail to connect if the engine attempts to open its own Raylib graphical window.

## Step 7B — UCI Command Integration (Position and Go)

### Goal
Bridge the text-based Universal Chess Interface (UCI) with the engine's internal board representation and search algorithms via the `position` and `go` commands.

### Files Modified
engine/
└── uci.c

### Features
- Implemented `ParsePosition()` to decode starting setups (`startpos` or FEN strings) and apply any subsequent move history to the internal board state.
- Implemented `ParseAndMakeMove()` to translate UCI coordinate strings (e.g., `e2e4` or `e7e8q`) into internal `Move` structs by validating against the engine's pseudo-legal move generator and explicitly checking the `promotion` piece.
- Implemented `ParseGo()` to extract search constraints (`depth`), invoke `SearchBestMove()`, and format the engine's best move back into standard algebraic notation (`bestmove b1c3`).

### Result & Developer Notes
The engine successfully translates back and forth between human-readable UCI strings and its own internal memory structures. It parses position strings silently and triggers iterative deepening upon receiving a `go` command, properly printing real-time `info` stats and concluding with the `bestmove` trigger expected by all modern chess GUIs.

## Step 7C — Time Management

### Goal
Implement a cross-platform time management system so the engine scales its search depth dynamically based on the remaining game clock, preventing it from losing on time.

### Files Modified / Created
engine/
├── time_utils.h (New)
├── time_utils.c (New)
├── search.h
├── search.c
└── uci.c

### Features
- Created `GetTimeMs()` in `time_utils.c` using OS-specific high-resolution timers (`GetTickCount64` for Windows, `gettimeofday` for POSIX) for cross-platform compatibility.
- Added a `CheckTime()` hook inside `Negamax()` that probes the system clock every 2048 nodes to avoid slowing down the search with constant OS timer calls.
- Updated `SearchBestMove()` to catch the `searchStopped` flag, safely discarding incomplete depth evaluations and returning the best move from the highest fully completed depth.
- Updated `ParseGo()` in `uci.c` to parse `wtime`, `btime`, `winc`, and `binc` UCI parameters, calculating an allocated time window (Time / 30 + Increment / 2) with a safety buffer.

### Result & Developer Notes
The engine successfully acts as an autonomous player. In blitz time controls, it aborts its search quickly to play instantly, while in longer time controls, it utilizes the available time to calculate significantly deeper (e.g., reaching depth 7+ on startpos). The Transposition Table proved extremely effective during back-to-back searches of the same position, resulting in near-instant evaluations for previously reached depths.

## Step 7D — UI Polish

### Goal
Upgrade the custom Raylib graphical interface to provide a modern, tactile playing experience, and finalize the engine for external testing.

### Files Modified
ui/
├── renderer.h
└── renderer.c
main.c

### Features
- **Drag-and-Drop & Click-to-Move:** Implemented seamless piece dragging, rendering the dragged piece at exact mouse coordinates while hiding it from its origin square.
- **Visual Move Indicators:** Added logic to draw thick rings on capture squares and small solid dots on empty legal squares.
- **Last Move Highlighting:** Tracks and renders a yellow tint over the origin and destination squares of the most recently played move (both human and engine).
- **Tactical Drawing Tools:** Added right-click square highlighting (red tint) and right-click-and-drag arrow drawing (orange arrows) for calculating lines visually.
- **Bug Fix:** Fixed an OS-level integer division-by-zero crash by ensuring the Transposition Table (`TT_Init` / `TT_Free`) spans the entire lifecycle of the program, rather than just a testing block.

### Result & Developer Notes
The engine now boasts a fully featured, visually polished custom GUI that rivals standard chess software. The user can seamlessly play against the engine locally without needing to boot into UCI mode, making debugging and casual play incredibly smooth.

## Step 7E — Engine Validation & Competitive Testing

### Goal
Subject the engine to real-world competitive testing by pitting it against a world-class UCI engine (Stockfish) via an automated chess GUI (CuteChess) to uncover edge-case bugs and validate stability.

### 1. The Testing Environment Setup
To facilitate engine-vs-engine matches, a "referee" GUI and a benchmarking opponent were required:
*   **The GUI:** Downloaded and installed **CuteChess**, the industry standard for automated engine tournaments.
*   **The Opponent:** Downloaded **Stockfish**, the world's highest-rated open-source chess engine (~4000 Elo), serving as the ultimate stress-tester for move legality and engine robustness.

### 2. GUI Integration & Troubleshooting
Integrating a custom engine with a dual-mode (UI/Headless) architecture into CuteChess presented initial launch issues:
*   **Issue:** CuteChess failed to initialize the engine because it booted into the Raylib graphics mode instead of headless UCI mode.
*   **Diagnosis:** CuteChess was executing the `.exe` without passing the required `uci` command-line argument.
*   **Failed Fix:** Placing `uci` in the "Init Strings" configuration failed, as this sends text *after* the executable launches, which is too late to prevent the Raylib window from initializing and locking the main thread.
*   **Successful Fix (The Batch File Workaround):** Created a `run_engine.bat` script containing the exact command `game.exe uci`. CuteChess was reconfigured to execute the `.bat` file instead of the `.exe`, guaranteeing the command-line argument was successfully passed to `main.c`.

### 3. The First Match
A 1-minute + 1-second increment match was initiated between the C Chess Engine (White) and Stockfish (Black).
*   **Performance:** The engine successfully communicated via the UCI protocol, parsed time controls, and managed its clock. 
*   **Stability:** It survived 37 deep-calculation moves against Stockfish without crashing or generating illegal pseudo-moves, proving the core algorithmic stability of the legal move generator.

### 4. The Root TT Cutoff Bug (`a8a8`)
On move 37, the engine abruptly attempted to play `a8a8` (an illegal move) despite having ample time on the clock.
*   **The Symptom:** In C, a `Move` struct initialized to `{0}` evaluates to `fromRow=0, fromCol=0, toRow=0, toCol=0`, translating to the algebraic notation `a8a8`.
*   **The Cause:** The engine experienced a **Root Transposition Table (TT) Cutoff**. During the opponent's turn, the engine had pondered and stored the exact resulting board state in its TT at a high depth. When it became the engine's turn to search (`ply == 0`), the `Negamax()` function probed the TT, found an exact match, and instantly returned the score. 
*   **The Consequence:** Because the root node returned instantly, it entirely bypassed legal move generation and the alpha-beta evaluation loop. Consequently, no move was saved to the Principal Variation (PV) array. When `SearchBestMove()` attempted to extract the best move from the empty PV array, it defaulted to `{0}`.

### 5. The Fix
The logic in `engine/search.c` was modified to explicitly forbid TT cutoffs at the root node.

**Modified File:** `engine/search.c`
```c
    // --- 1. PROBE TRANSPOSITION TABLE ---
    Move ttMove = {0};
    int ttScore;
    
    if (TT_Probe(board->hashKey, depth, alpha, beta, &ttScore, &ttMove)) {
        // FIX: Prevent TT cutoffs at the root node (ply == 0) to guarantee move generation.
        if (ply > 0) {
            return ttScore; 
        }
    }
    // ------------------------------------
```

### 6. Result
The TT is still probed at the root to extract the ttMove (which is immediately used by the Move Ordering logic to search the absolute best move first), but the root node is now forced to complete its evaluation loop, ensuring a valid, legal move is always returned to the GUI.

### 7. Stability Verification & Stress Testing
Following the fix of the Root TT Cutoff bug, the engine underwent rigorous competitive testing against Stockfish in multiple full-game matches. 
* **Reliability:** The engine demonstrated complete stability, successfully executing 50-move sequences against a grandmaster-level opponent without crashing or outputting illegal moves.
* **Evaluation Accuracy:** The engine correctly identified and tracked declining positions, reliably triggering terminal evaluation scores (e.g., -999.96) when facing forced checkmate sequences.
* **Protocol Compliance:** The engine maintained seamless communication through the UCI protocol, properly managing time budgets and responding to complex game-state queries under high-pressure conditions.