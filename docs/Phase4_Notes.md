## Previous Phases Summary

### Phase 0 — Project Setup

Implemented:

- Raylib setup
- VS Code configuration
- Makefile
- Git repository
- GitHub repository
- Modular project structure

---

### Phase 1 — Board Representation

Implemented:

- Piece enum
- Side enum
- CastlingRights structure
- Board structure
- Board initialization
- Board printing
- Piece helper functions

Board stores:

- Piece placement
- Side to move
- Castling rights
- En passant square
- Halfmove clock
- Fullmove number

---

### Phase 2 — Rendering System

Implemented:

- Chessboard rendering
- Piece rendering
- Texture loading
- Mouse coordinate mapping
- Click debugging
- FEN generation

Architecture:

- Renderer is completely separated from the engine.
- The board is always the single source of truth.
- Renderer never modifies engine state.

---

### Phase 3 — Pseudo-Legal Move Generation

Implemented:

#### Move System

- Move structure
- Move flags
- MoveList
- MoveList_Add()

#### Move Generation

- Pawn moves
- Knight moves
- Bishop moves
- Rook moves
- Queen moves
- King moves
- Promotions
- En passant
- Castling

#### Move Execution

Implemented `MakeMove()` supporting:

- Normal moves
- Captures
- Promotions
- En passant
- Double pawn push
- Castling
- Castling rights updates
- En passant updates
- Halfmove clock
- Fullmove number
- Side switching

At the end of Phase 3, the engine generated and executed every pseudo-legal move.

---

## Current Folder Structure

```
C_Chess_Engine/

assets/
    pieces/

engine/
    board.c
    board.h

    move.c
    move.h

    makemove.c
    makemove.h

    history.c
    history.h

    movegen.c
    movegen.h

    gamestate.c
    gamestate.h

    fen.c
    fen.h

ui/
    renderer.c
    renderer.h

main.c
Makefile
```

---

## File Responsibilities

### board.*

- Board representation
- Board initialization
- Piece helper functions

---

### move.*

- Move structure
- Move flags
- MoveList
- MoveList_Add()

---

### makemove.*

Responsible for executing moves.

Includes:

- MakeMove()

---

### history.*

Responsible for:

- Board history
- History_Init()
- History_Push()
- UndoMove()

---

### movegen.*

Responsible for:

- Pseudo-legal move generation
- Legal move generation

Includes all piece move generators.

---

### gamestate.*

Responsible for:

- Attack detection
- Check detection
- Checkmate
- Stalemate
- Draw rules
- Castling legality

---

### fen.*

Converts the current board into FEN.

Used mainly for debugging.

---

### renderer.*

Responsible only for graphics.

Never modifies engine state.

---

### main.c

Responsible only for:

- Window creation
- Engine initialization
- Renderer initialization
- Temporary testing

Business logic should not accumulate here.

---

## Important Design Rules

- The board is always the single source of truth.
- Renderer never stores game state.
- Renderer never modifies the board.
- Move generation never draws anything.
- Each module has exactly one responsibility.
- Small reusable functions are preferred over very large functions.
- Every feature is implemented incrementally.
- Every feature is verified before moving to the next step.

## Phase 4 Notes Log

## Step 1 — Board_Copy()

### Added

- `Board_Copy(Board *dest, const Board *src)`

### Why

Provides a simple way to duplicate the complete board state without modifying the original.

### Result

The engine can create an exact copy of a board, which is useful for testing and future move validation.

## Step 2.1 — History Module

### Added

- `history.h`
- `history.c`
- `History` structure
- `History_Init()`
- `History_Push()`
- `History_Pop()`

### Why

Created a dedicated history stack to store complete board states before moves are made.

### Result

The engine can save and restore previous board positions, providing the foundation for `UndoMove()`.

## Step 2.2 — Save History Before Every Move

### Added

- `History` parameter to `MakeMove()`
- `History_Push()` at the beginning of `MakeMove()`

### Why

Every move now stores the previous board position before modifying it.

### Result

The engine keeps a complete history of positions, making undo operations possible.

## Step 2.3 — UndoMove()

### Added

- `UndoMove(Board *board, History *history)`

### Why

Restores the previous board position by popping the most recent board state from the history stack.

### Result

The engine now supports complete make/undo cycles. Every part of the board state—including pieces, castling rights, en passant target, clocks, and side to move—is restored exactly.

## Step 2.4 — Improved Board_Print()

### Added

- Side to move
- Castling rights
- En passant target square (in chess notation)
- Halfmove clock
- Fullmove number

### Why

Expanded `Board_Print()` into a complete debugging tool for verifying the full game state.

### Result

Every board print now displays all information needed to validate move execution and undo operations.

## Phase 4B – Step 1: Attack Module

### Added

- attack.h
- attack.c

### Why

Created a dedicated module for attack detection to keep it separate from move generation.

### Result

The project is now ready to implement attack detection and check detection in a clean, modular way.

## Phase 4B – Step 2: Pawn Attack Detection

### Added

- White pawn attack detection
- Black pawn attack detection

### Why

Pawns attack differently from how they move, so they are implemented separately.

### Result

`IsSquareAttacked()` can now correctly detect attacks from pawns.

## Phase 4B – Step 3: Knight Attack Detection

### Added

- Knight attack detection using the shared `knightMoves` table from `directions.h`.

### Why

Knights attack in fixed "L" shapes and are unaffected by blocking pieces.

### Result

`IsSquareAttacked()` now correctly detects attacks from pawns and knights.

## Phase 4B – Step 4: Bishop Attack Detection

### Added

- Bishop attack detection using diagonal ray tracing.

### Why

Bishops are sliding pieces and can attack any distance along diagonals until blocked.

### Result

`IsSquareAttacked()` now correctly detects attacks from pawns, knights, and bishops.

## Phase 4B – Step 5: Rook Attack Detection

### Added

- Rook attack detection using horizontal and vertical ray tracing.

### Why

Rooks are sliding pieces that attack along ranks and files until blocked.

### Result

`IsSquareAttacked()` now correctly detects attacks from pawns, knights, bishops, and rooks.

## Phase 4B – Step 6: Queen Attack Detection

### Added

- Queen attack detection using eight-direction ray tracing.

### Why

Queens combine the movement patterns of bishops and rooks.

### Result

`IsSquareAttacked()` now correctly detects attacks from pawns, knights, bishops, rooks, and queens.

## Phase 4B – Step 7: King Attack Detection

### Added

- King attack detection using the shared `kingDirections` table.

### Why

Kings attack the eight surrounding squares regardless of occupancy.

### Result

`IsSquareAttacked()` is now complete and supports attacks from every piece type.

## Phase 4B – Step 8: IsKingInCheck()

### Added

- `IsKingInCheck()`

### Why

Finds the king for the given side and determines whether its square is attacked by the opponent.

### Result

The engine can now correctly determine if either king is in check.

### Future Optimization

Currently, `IsKingInCheck()` searches the entire board to find the king. This is simple and perfectly acceptable for now because the board has only 64 squares.

Later, we can optimize this by storing the positions of both kings (row and column) in the `Board` struct and updating them inside `MakeMove()` and `UndoMove()`. This will eliminate the board scan and make `IsKingInCheck()` an O(1) operation for locating the king.

## Phase 4C – Step 1: Legal Move Module

### Added

- legalmove.h
- legalmove.c

### Why

Created a dedicated module responsible for converting pseudo-legal moves into legal moves.

### Result

The project is ready to implement legal move filtering while keeping responsibilities separated.

## Phase 4C – Step 2: Legal Move Filtering Loop

### Added

- Generated pseudo-legal moves.
- Tested each move by making it on the board.
- Checked whether the moving side's king remained safe.
- Undid the move.
- Kept only legal moves.

### Why

Legal chess moves are simply pseudo-legal moves that do not leave the moving side's king in check.

### Result

The engine now has the complete legal move filtering algorithm. The next step is to verify it with test positions.

## How Legal Move Generation Works

`GenerateLegalMoves()` is called once at the start of each player's turn.

For the current side:

1. Generate all pseudo-legal moves.
2. Temporarily make one move.
3. Check if that same side's king is in check.
4. If the king is safe, keep the move.
5. Undo the move.
6. Repeat for every pseudo-legal move.

After all moves are tested, the function returns only the legal moves. The player (or engine) then chooses one of these moves, and `MakeMove()` is called again to permanently update the game.

### Why save `movingSide`?

`MakeMove()` switches `board->sideToMove` to the opponent. However, while testing a move, we need to check whether the player who **made the move** left **their own king** in check, not the opponent's.

Example:

```
White to move
↓
GenerateLegalMoves()
↓
Test e2 → e4 (temporary)
↓
MakeMove() → board->sideToMove becomes Black
↓
Check if White's king is in check
↓
UndoMove()
↓
Try the next White move
```

This process repeats for every candidate move until only legal moves remain.

## Phase 4C – Step 3: Move List Printer

### Added

- `MoveList_Print()` for debugging move lists.

### Why

Provides a reusable way to inspect generated moves during testing.

### Result

The engine can now display all generated moves, making it easier to verify legal move filtering and future game-state logic.

## Phase 4C – Step 4: Verify Legal Move Filtering

### Added

- Tested `GenerateLegalMoves()` using a pinned rook position.

### Why

A pinned piece provides a simple way to verify that moves exposing the king are correctly filtered out.

### Result

Illegal rook moves that expose the king are removed from the legal move list, confirming that legal move filtering is working correctly.

## Phase 4C – Step 5: Verify Moves While in Check

### Added

- Tested legal move generation with a king already in check.

### Why

When a king is in check, only moves that remove the check are legal.

### Result

The engine correctly filters out moves that leave the king in check, returning only valid escape moves.

## Phase 4D – Step 1: Checkmate Detection

### Added

- `gamestate.h`
- `gamestate.c`
- `IsCheckmate()`

### Why

A side is checkmated if its king is in check and it has no legal moves.

### Result

The engine can now determine whether a player is checkmated using the existing attack detection and legal move generation systems.

## Phase 4D – Step 2: Verify Checkmate Detection

### Test Position

White:
- King: g6
- Queen: g7

Black:
- King: h8

Black to move.

### Expected Result

Black in check: 1

Black checkmated: 1

### Result

The engine correctly identified the checkmate.

## Phase 4D – Step 3: Stalemate Detection

### Added

- `IsStalemate()`.

### Why

A stalemate occurs when the side to move is not in check but has no legal moves.

### Result

The engine can now distinguish between checkmate and stalemate using legal move generation and check detection.

## Phase 4E – Step 1: Castling While in Check

### Added

- Prevent castling if the king is currently in check.

### Why

Chess rules forbid castling while the king is in check.

### Result

Castling moves are no longer generated when the king begins the turn in check.

## Phase 4E – Step 2: Castling Through Check

### Added

- Prevent castling while in check.
- Prevent castling through an attacked square.
- Prevent castling into an attacked square.

### Why

According to the official chess rules, castling is only legal if:
- The king is not currently in check.
- The squares the king passes through are not attacked.
- The destination square is not attacked.

### Result

The engine now generates only fully legal castling moves.

## Phase 4F – Step 1: Fifty-Move Rule

### Added

- `IsDrawByFiftyMoveRule()`.

### Why

The fifty-move rule allows a draw to be claimed if 100 consecutive half-moves occur without a pawn move or capture.

### Result

The engine can now detect when the fifty-move rule has been reached using the existing `halfmoveClock`.

## Phase 4G – Step 1: Position Comparison

### Added

- `PositionEquals()`.

### Why

Threefold repetition depends on whether the same board position has occurred multiple times. A position includes:
- Piece placement
- Side to move
- Castling rights
- En passant square

It does not include the halfmove clock or fullmove number.

### Result

The engine can now accurately compare two board positions according to the official rules of chess.

## Phase 4G – Step 2: Threefold Repetition

### Added

- `IsDrawByThreefoldRepetition()`.

### Why

A draw by repetition occurs when the same position appears three or more times. Positions are compared using:
- Piece placement
- Side to move
- Castling rights
- En passant square

### Result

The engine can now detect threefold repetition by scanning the stored board history.

## Current Draw Rules

### Implemented

- Checkmate detection
- Stalemate detection
- Fifty-move rule
- Threefold repetition

### Not Yet Implemented

- Insufficient material draw

Examples of positions not currently detected as draws include:
- King vs King
- King and Bishop vs King
- King and Knight vs King
- King and Bishop vs King and Bishop (same-colored bishops)

This feature is intentionally deferred and may be added in a future version of the engine.

## Future Optimizations

The current implementation searches the board to locate the king when checking for check.

This is correct but not optimal.

A future optimization is to store the white and black king positions in the `Board` structure and update them in `MakeMove()`, allowing constant-time king lookup.

---

# Phase 5 Handoff

## Project Status

At the end of Phase 4, the engine can play a complete legal game of chess.

Implemented:

- Complete board representation
- Rendering system
- Pseudo-legal move generation
- Move execution
- Undo functionality
- Attack detection
- Check detection
- Legal move generation
- Checkmate detection
- Stalemate detection
- Fully legal castling
- Fifty-move rule
- Threefold repetition

Not implemented:

- Insufficient material draw (intentionally postponed)

---