# C Chess Engine — Phase 5 Notes

---

# Project Status Before Phase 5

## Overall Project Structure

```
C_Chess_Engine/

assets/
│
├── pieces/
│     └── Chess piece textures.
│
engine/
│
├── board.c / board.h
├── move.c / move.h
├── makemove.c / makemove.h
├── history.c / history.h
├── movegen.c / movegen.h
├── gamestate.c / gamestate.h
├── fen.c / fen.h
│
ui/
│
├── renderer.c / renderer.h
├── main.c
│
Makefile
```

---

# Engine Modules

## board.c / board.h

Responsible for representing the chess board.

Contains:

- Board structure
- Piece enumeration
- Square indexing
- Board initialization
- Board printing
- Piece helper functions

Purpose:

Acts as the single source of truth for the entire engine.

---

## move.c / move.h

Responsible for representing moves.

Contains:

### Structures

- Move
- MoveList

### Functions

- MoveList_Add()
- MoveList_Clear()
- MoveList_Print()

Purpose:

Provides a common move representation shared by every engine module.

---

## movegen.c / movegen.h

Responsible for move generation.

Contains:

### Piece-specific generators

- GeneratePawnMoves()
- GenerateKnightMoves()
- GenerateBishopMoves()
- GenerateRookMoves()
- GenerateQueenMoves()
- GenerateKingMoves()

### General generators

- GenerateMoves()
- GenerateLegalMoves()

Purpose:

Generates every pseudo-legal and legal move for a given position.

---

## makemove.c / makemove.h

Responsible only for executing moves.

Contains:

### Function

- MakeMove()

Updates:

- Piece movement
- Captures
- Promotions
- En passant
- Castling
- Castling rights
- Halfmove clock
- Fullmove number
- Side to move

Purpose:

Changes the board state while keeping it internally consistent.

---

## history.c / history.h

Responsible for storing previous board states.

Contains:

### Structure

- History

### Functions

- History_Init()
- History_Push()
- UndoMove()
- PositionEquals()

Purpose:

Allows moves to be undone exactly as they were before execution.

Also provides board comparison for threefold repetition detection.

---

## gamestate.c / gamestate.h

Responsible for chess rule validation.

Contains:

### Functions

- IsSquareAttacked()
- IsKingInCheck()
- IsCheckmate()
- IsStalemate()
- IsDrawByFiftyMoveRule()
- IsDrawByThreefoldRepetition()

Purpose:

Implements game-ending conditions and attack detection.

---

## fen.c / fen.h

Responsible for FEN generation.

Contains:

### Function

- BoardToFEN()

Purpose:

Converts the current board position into FEN for debugging and verification.

---

# Rendering Modules

## renderer.c / renderer.h

Responsible only for graphics.

Draws:

- Chess board
- Pieces
- Textures

Never:

- Generates moves
- Changes board state
- Applies game rules

Purpose:

Pure rendering layer.

---

## main.c

Project entry point.

Responsible for:

- Window creation
- Texture loading
- Board initialization
- Renderer initialization
- Main game loop
- Temporary testing

Purpose:

Coordinates engine modules without implementing engine logic.

---

# Engine Features Completed

The engine currently supports:

- Board representation
- Board initialization
- Board printing
- Piece helper functions
- Rendering
- Mouse coordinate mapping
- FEN generation
- Pseudo-legal move generation
- Legal move generation
- Move execution
- Undo functionality
- Attack detection
- Check detection
- Checkmate detection
- Stalemate detection
- Fully legal castling
- En passant
- Pawn promotion
- Fifty-move rule
- Threefold repetition

The engine can successfully play a complete legal game of chess.

---

# Core Concepts Implemented

During Phases 0–4 the following concepts were introduced:

- Board representation
- Piece encoding
- Move representation
- Move lists
- Pseudo-legal move generation
- Legal move filtering
- Attack maps
- Check detection
- Castling legality
- En passant rules
- Promotion handling
- History stack
- Undo system
- Threefold repetition
- Fifty-move rule
- FEN generation
- Separation of engine and rendering

---

# Architecture Rules

The project follows these principles:

- Board is the single source of truth.
- Rendering never stores game state.
- Rendering never modifies the board.
- Move generation never performs rendering.
- MakeMove() only executes moves.
- UndoMove() restores previous positions.
- Rule validation belongs inside gamestate.
- Each module has one responsibility.
- Small reusable functions are preferred.
- Features are implemented incrementally and verified before continuing.

---

# Remaining Improvements

Not yet implemented:

- Insufficient material draw

Future optimizations:

- Cached king positions
- Zobrist hashing
- Faster move generation
- Search optimizations

---

# Phase 5 Progress

## Step 1 — Evaluation Framework

### Goal

Introduce the evaluation module and implement a basic material evaluation function.

### Concepts Learned

- Every chess engine needs an evaluation function to judge positions.
- The first evaluation is based only on material.
- Positive scores favor White.
- Negative scores favor Black.
- Piece values are stored in a private helper function.
- The evaluation module only reads the board and never modifies it.

### Files Added

```
engine/
├── evaluate.c
└── evaluate.h
```

### Functions Added

#### evaluate.h

- `EvaluatePosition()`

#### evaluate.c

- `GetPieceValue()` *(private helper)*
- `EvaluatePosition()`

### Result

The engine can evaluate any position by calculating the material difference between White and Black.

Verified using:

- Starting position
- Missing Black queen
- Missing White rook
- Missing White pawn


## Step 2 — Piece-Square Tables

### Part 1 — PST Module

#### Goal

Create a dedicated module to store Piece-Square Tables.

#### Concepts Learned

- Piece-Square Tables store positional bonuses for pieces.
- Tables are constant data and belong in their own module.
- `extern` allows a constant table to be shared across multiple source files.

#### Files Added

```
engine/
├── pst.c
└── pst.h
```

#### Result

Created the first Piece-Square Table module containing the Knight Piece-Square Table.

### Part 2 — Knight PST Integration

#### Goal

Incorporate the Knight Piece-Square Table into the evaluation function.

#### Concepts Learned

- Positional bonuses are added separately from material.
- Black pieces reuse the same table by mirroring the row index.
- Evaluation combines multiple independent scoring components.

#### Functions Added

- `GetPieceSquareValue()` *(private helper)*

#### Result

The evaluation now considers both material and knight placement.

## Step 3 — Negamax Search

### Part 1 — Search Module

#### Goal

Create the search module and define the public search interface.

#### Concepts Learned

- Search is separated from evaluation.
- `SearchBestMove()` manages the root of the search tree.
- `Negamax()` recursively evaluates positions.

#### Files Added

```
engine/
├── search.c
└── search.h
```

#### Functions Added

- `Negamax()`
- `SearchBestMove()`

#### Result

Created the search framework that future search algorithms will build upon.

### Part 2 — Negamax Base Case

#### Goal

Implement the stopping condition for the Negamax search.

#### Concepts Learned

- Every recursive algorithm requires a base case.
- The search stops when the specified depth reaches zero.
- Leaf positions are evaluated using `EvaluatePosition()`.

#### Functions Modified

- `Negamax()`

#### Result

The search now correctly terminates at the specified depth and returns the static evaluation of the current position.

### Part 3 — One-Ply Search

#### Goal

Teach the search to examine every legal move from the current position.

#### Concepts Learned

- Generate all legal moves.
- Execute one move at a time.
- Evaluate the resulting position.
- Restore the previous position using `UndoMove()`.
- Keep the highest evaluation found.

#### Functions Modified

- `Negamax()`

#### Result

The engine now explores every legal move one ply deep while correctly restoring the board after each move.

### Part 4 — Recursive Negamax

#### Goal

Extend the search from one ply to multiple plies using recursion.

#### Concepts Learned

- Negamax searches one move deeper by recursively calling itself.
- The score is negated because each recursive call switches to the opponent's perspective.
- The recursion terminates at the base case implemented earlier.

#### Functions Modified

- `Negamax()`

#### Result

The engine can now search positions to any specified depth.

### Part 5 — Terminal Position Handling

#### Goal

Handle positions where no legal moves are available.

#### Concepts Learned

- No legal moves do not always mean checkmate.
- Checkmate returns a large negative score.
- Stalemate returns a draw score of 0.

#### Functions Modified

- `Negamax()`

#### Result

The search now correctly distinguishes between checkmate and stalemate, producing meaningful scores for terminal positions.

### Part 6 — SearchBestMove

#### Goal

Implement the root search function that selects the best move.

#### Concepts Learned

- `SearchBestMove()` searches all legal moves from the current position.
- Each move is evaluated using `Negamax()`.
- The move with the highest score is returned.

#### Functions Modified

- `SearchBestMove()`

#### Result

The engine can now choose a move by searching ahead and comparing evaluations. `Move_Print()` gave a legal and valid first best move: b1 -> c3. And `Board_Print()` still shows the starting position.
And running multiple times gave the same move.

## Step 4 — Alpha-Beta Pruning

### Part 1 — Alpha-Beta Parameters

#### Goal

Prepare the search function for Alpha-Beta pruning by extending the Negamax interface.

#### Concepts Learned

- Alpha and Beta define the current search window.
- The search window is passed recursively through the search tree.
- The search behavior remains unchanged until pruning logic is added.

#### Functions Modified

- `Negamax()`
- `SearchBestMove()`

#### Result

The search is now prepared for Alpha-Beta pruning while producing the same results as before.

### Part 2 — Alpha Update

#### Goal

Track the best score found so far using the alpha value.

#### Concepts Learned

- Alpha represents the best score the current player can guarantee.
- Alpha is updated whenever a better score is found.
- Updating alpha alone does not change the search result.

#### Functions Modified

- `Negamax()`

#### Result

The search now maintains the alpha bound while producing the same moves as before.

### Part 3 — Alpha-Beta Cutoff

#### Goal

Avoid searching branches that cannot improve the current result.

#### Concepts Learned

- Alpha-Beta pruning stops exploring moves when `alpha >= beta`.
- Pruned branches cannot affect the final decision.
- Pruning improves search efficiency without changing the chosen move.

#### Functions Modified

- `Negamax()`

#### Result

The search now performs Alpha-Beta pruning, allowing deeper searches with fewer evaluated positions.

## Step 5 — Search Statistics

### Part 1 — Node Counter

#### Goal

Measure the number of positions visited during a search.

#### Concepts Learned

- Every recursive call to `Negamax()` represents one searched node.
- Node counts are useful for evaluating search performance.
- Search statistics help verify future optimizations such as Alpha-Beta pruning.

#### Files Modified

- `search.h`
- `search.c`

#### Result

The engine now reports how many positions were searched for each move selection.

### Part 2 — Best Evaluation

#### Goal

Record the evaluation of the best move found during the search.

#### Concepts Learned

- The search not only selects a move but also computes its evaluation.
- Reporting the evaluation helps explain the engine's decisions.
- Search statistics are useful for debugging and future optimizations.

#### Files Modified

- `search.h`
- `search.c`

#### Result

The engine now reports both the best move and its evaluation after each search.

### Part 3 — SearchResult

#### Goal

Group all search outputs into a single structure.

#### Concepts Learned

- A search produces multiple related results, not just a move.
- `SearchResult` keeps the move, evaluation, and node count together.
- Grouping related data simplifies function interfaces and improves extensibility.

#### Files Modified

- `search.h`
- `search.c`

#### Result

`SearchBestMove()` now returns a `SearchResult` structure instead of only the best move.

### Part 4 — Remove Global Search State

#### Goal

Eliminate the remaining global search statistics.

#### Concepts Learned

- Search statistics should belong to a specific search, not to the entire program.
- Passing the node counter through recursion removes the need for global state.
- The search module is now self-contained and easier to maintain.

#### Functions Modified

- `Negamax()`
- `SearchBestMove()`

#### Result

The search no longer depends on global variables. All search results are stored directly inside the `SearchResult` structure.

## Step 6 — Engine vs Human Gameplay

### Part 1 — MakeEngineMove()

#### Goal

Create a helper function that searches for and immediately executes the best move.

#### Concepts Learned

- `SearchBestMove()` decides what to play.
- `MakeEngineMove()` performs the move on the board.
- Keeping search and execution separate keeps the engine modular.

#### Functions Added

- `MakeEngineMove()`

#### Result

The engine can now search for and play its own move with a single function call.

### Part 2 — Engine Response

#### Goal

Allow the engine to automatically respond after the human plays a move.

#### Concepts Learned

- The game loop determines when the engine should move.
- `MakeEngineMove()` searches for and executes the best move.
- The board state and search statistics can be verified after every engine move.

#### Files Modified

- `main.c`

#### Result

The engine now automatically responds to a human move, updates the board, and returns the turn to the human player.

### Part 3 — Handle Positions with No Legal Moves

#### Goal

Prevent the search from attempting to execute an invalid move when no legal moves are available.

#### Concepts Learned

- Some positions (checkmate and stalemate) contain zero legal moves.
- The search should detect this case and return immediately.
- Returning a default `SearchResult` avoids executing an invalid move.

#### Files Modified

- `search.c`

#### Result

The search now safely handles terminal positions without crashing or returning an invalid move.

### Part 4 — Terminal Position Evaluation

#### Goal

Evaluate checkmate and stalemate positions correctly instead of using only the material evaluation.

#### Concepts Learned

- Terminal positions require special evaluation.
- Checkmate is evaluated as `-CHECKMATE_SCORE`.
- Stalemate is evaluated as `DRAW_SCORE`.
- Material evaluation is only used for non-terminal positions.

#### Files Modified

- `evaluate.h`
- `search.c`

#### Result

The search now correctly distinguishes between normal positions, checkmate, and stalemate.

## Step 7 — Human Move Input

### Part 1 — Selected Square

#### Goal

Store the square selected by the user.

#### Concepts Learned

- Mouse clicks are converted into board coordinates.
- The selected square is stored until another square is selected.
- This selection will be used later for move generation and piece movement.

#### Files Modified

- `main.c`

#### Result

The program now tracks the currently selected board square.

### Part 2 — Select Only the Human's Pieces

#### Goal

Restrict square selection to pieces controlled by the human player.

#### Concepts Learned

- Not every clicked square should become a valid selection.
- The UI should only allow selecting pieces belonging to the human player.
- This prevents invalid interactions before move generation.

#### Files Modified

- `main.c`

#### Result

Only White pieces can currently be selected. This will later be generalized to support playing as either White or Black.

### Part 3 — Generate Legal Moves for the Selected Piece

#### Goal

Generate and store the legal moves for the currently selected piece.

#### Concepts Learned

- The engine generates all legal moves for the current position.
- The UI filters those moves based on the selected piece.
- The filtered move list is stored for future interaction.

#### Files Modified

- `main.c`

#### Result

Selecting a piece now produces a list of all legal moves for that piece, ready for move validation and highlighting.

### Part 4 — Execute a Selected Legal Move

### Goal

Allow the player to execute one of the legal moves generated in Part 3.

At the end of this step, the interaction becomes:

```
Click White piece
        │
        ▼
Generate legal moves
        │
        ▼
Store moves in selectedMoves
        │
        ▼
Highlight legal destinations
        │
        ▼
Click destination square
        │
        ▼
Is destination in selectedMoves?
        │
     Yes ▼
Execute move
        │
        ▼
Clear selection and highlights
        │
        ▼
Engine searches for the best reply
        │
        ▼
Engine executes its move
        │
        ▼
Player's turn
```

### Implementation

- Detect a left mouse click on the board.
- Compare the clicked square against every move stored in `selectedMoves`.
- If a matching destination is found:
  - Execute the move using `MakeMove()`.
  - Clear the current piece selection.
  - Reset `selectedMoves`.
  - Call `MakeEngineMove()` to generate and execute the engine's response.
- If no legal destination is selected:
  - Treat the click as a new piece selection.
  - Generate all legal moves for the current side.
  - Filter the moves belonging to the selected piece.
  - Store them in `selectedMoves` for rendering and execution.

### Result

The player can now play a complete legal move through the GUI, after which the engine immediately searches for and executes its own move, enabling full Human vs Engine gameplay.

## Phase 5 Wrap-up & Debugging Summary

### Engine Status
- Confirmed that the core Phase 5 features are implemented:
  - Material evaluation
  - Piece-Square Tables
  - Evaluation function
  - Negamax
  - Alpha-Beta pruning
  - Best move selection
  - Human vs Engine gameplay
- Decided that Phase 5 is complete and ready for a Git commit.

### UI
- Added move highlighting to the renderer.
- Added engine move execution after the human move.
- Added checkmate/stalemate detection after each move.

### Search Improvements
- Fixed `MakeEngineMove()` so it does not execute an invalid move when there are no legal moves.
- Added temporary debugging prints in `SearchBestMove()` for:
  - Root legal moves
  - Score of each move
  - Best move chosen
  - Node count

### Legal Move Investigation
- Investigated a position where Black appeared to have only one legal move before checkmate.
- Added extensive debugging to `GenerateLegalMoves()`.
- Printed every pseudo-legal move as:
  - LEGAL
  - ILLEGAL
- Determined that the move generator and check detection appear to be functioning correctly in normal gameplay.

### Checkmate Testing
- Tested multiple real games.
- Verified that:
  - The king correctly escapes when one legal square exists.
  - Engine correctly reports checkmate when there are zero legal moves.
  - Illegal king captures are no longer occurring.

### Remaining Game Rule
- Observed that threefold repetition is not implemented.
- Confirmed that `PositionEquals()` already compares the correct fields:
  - Piece placement
  - Side to move
  - Castling rights
  - En passant square
- Only the repetition-counting logic still needs to be added later.

### Development Decision
- Decided not to continue UI work for now.
- Next priority is engine development.

### Planned Next Steps
1. Commit Phase 5.
2. Implement debugging utilities:
   - FEN parser
   - Perft
   - Debug commands
3. Begin Phase 6 search improvements:
   - Move ordering
   - Iterative Deepening
   - Quiescence Search
   - Transposition Tables
   - Zobrist Hashing
   - Principal Variation
   - Killer/History heuristics

### Bug Investigation Notes
- Earlier in testing, observed two inconsistent behaviors:
  - The king could apparently be captured.
  - The engine occasionally failed to move even when a safe king move seemed available.
- These issues could not be reproduced consistently during later testing.
- Possible causes considered:
  - UI confusion due to the lack of last-move highlighting.
  - Very fast user interaction/clicking causing misinterpretation of the current board state.
  - An edge case that has not yet been isolated.
- After extensive debugging, multiple complete games were played and the engine consistently:
  - Generated legal moves correctly.
  - Allowed the king to escape when a legal square existed.
  - Correctly detected checkmate when no legal moves remained.
- Current conclusion:
  - No reproducible bug has been found in the core move generation, legality checking, or search logic.
  - The engine appears stable under normal gameplay, but future testing (especially after adding a FEN parser and Perft) should continue to watch for any reproducible edge cases before Phase 6.
---