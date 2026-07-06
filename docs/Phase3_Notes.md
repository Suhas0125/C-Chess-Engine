# Phase 3 – Pseudo-Legal Move Generation (Development Log)

## Objective

Implement a complete pseudo-legal move generation system for the chess engine.

Pseudo-legal moves obey the movement rules of each chess piece but do **not** yet verify whether the move leaves the king in check. Legal move validation will be implemented in Phase 4.

This phase also introduces the engine's move representation and move execution system, allowing every generated move to be applied to the board correctly.

---

# Starting Point (End of Phase 2)

At the beginning of Phase 3, the project already contained:

## Engine

- Complete board representation
- Piece enum
- Board struct
- Castling rights
- En passant square
- Halfmove/fullmove counters
- FEN generation

## Renderer

- Raylib window
- Chessboard rendering
- Piece texture rendering
- Mouse coordinate mapping
- Renderer module (`ui/renderer.c`)

## Limitations

The engine had **no gameplay logic**.

- No move generation
- No move representation
- No move execution
- No chess rules

---

# Step 1 — Move Representation

## What was added

Created the move system.

```
engine/
├── move.h
└── move.c
```

Introduced a dedicated `Move` structure.

```c
typedef struct{
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;

    Piece promotion;
    int flags;
} Move;
```

## Why

Every move in the engine should be represented using a single standardized structure instead of passing multiple variables.

## Result

Every move now carries:

- starting square
- destination square
- promotion piece
- special move information

---

# Step 2 — Move Flags

## What was added

Created a bit-flag system describing special move types.

```c
typedef enum{
    MOVE_NONE        = 0,
    MOVE_CAPTURE     = 1 << 0,
    MOVE_DOUBLE_PAWN = 1 << 1,
    MOVE_EN_PASSANT  = 1 << 2,
    MOVE_CASTLING    = 1 << 3,
    MOVE_PROMOTION   = 1 << 4
} MoveFlags;
```

## Why

Many moves can have special behaviour.

For example:

- capture
- promotion
- castling
- en passant

Bit flags allow multiple properties to be stored efficiently.

## Result

Moves can now describe both their destination and their behaviour.

---

# Step 3 — Move List

## What was added

Created a container to store generated moves.

```c
typedef struct{
    Move moves[256];
    int count;
} MoveList;
```

Added:

```c
MoveList_Add(...)
```

to append moves safely.

## Why

Move generators need a common destination where every generated move is stored.

## Result

All piece generators now write into the same move list.

---

# Step 4 — Move Generation Module

## What was added

Created:

```
engine/
├── movegen.h
└── movegen.c
```

Added the engine entry point:

```c
GenerateMoves(board, &list);
```

## Why

Move generation should remain independent from rendering and input handling.

## Result

A single function is now responsible for generating every pseudo-legal move.

---

# Step 5 — Pawn Single Push

## What was added

Implemented forward pawn movement.

Example:

```c
newRow = row + direction;
```

The destination square must be empty before the move is added.

## Why

Pawn movement differs from every other piece and forms the basis for later pawn rules.

## Result

Pawns can now advance one square.

---

# Step 6 — Pawn Double Push

## What was added

Implemented the initial two-square pawn advance.

Conditions checked:

- pawn is on its starting rank
- both squares are empty

Generated moves receive:

```c
MOVE_DOUBLE_PAWN
```

## Why

The engine later uses this flag to create an en passant target square.

## Result

Initial pawn double moves are generated correctly.

---

# Step 7 — Pawn Captures

## What was added

Implemented diagonal pawn captures.

Both diagonals are checked.

Enemy pieces generate:

```c
MOVE_CAPTURE
```

Friendly pieces are ignored.

## Why

Pawn captures are directional and cannot reuse the movement logic used for forward pushes.

## Result

Pawns correctly generate all pseudo-legal capture moves.

---

# Step 8 — Pawn Promotion

## What was added

Implemented pawn promotions.

When a pawn reaches the last rank, promotion moves are generated instead of normal pawn moves.

Each generated move stores:

```c
move.promotion
```

along with:

```c
MOVE_PROMOTION
```

## Why

Promotion changes the moving piece into a completely different piece after the move is executed.

## Result

Promotion moves are fully represented inside the move list.

---

# Step 9 — En Passant Generation

## What was added

Implemented en passant move generation.

The engine checks:

- current en passant target square
- left capture
- right capture

Matching moves receive:

```c
MOVE_EN_PASSANT
```

## Why

En passant is the only chess capture where the captured piece is **not** on the destination square.

Generating it separately keeps the pawn logic clear.

## Result

Valid en passant captures are now included in the pseudo-legal move list.

---

# Step 10 — Knight Move Generation

## What was added

Implemented knight movement using a fixed direction table.

```c
const int knightMoves[8][2]
```

Each destination is checked for:

- board boundaries
- empty squares
- enemy captures

## Why

Knights always have exactly eight possible destinations and ignore blocking pieces.

A lookup table keeps the implementation simple and efficient.

## Result

Knights now generate all pseudo-legal moves and captures.

# Step 11 — Bishop Move Generation

## What was added

Implemented bishop movement using four diagonal directions.

```c
const int bishopDirections[4][2]
```

Each direction is followed until:

- the board edge is reached
- a friendly piece blocks the path
- an enemy piece is captured

## Why

Bishops are sliding pieces and can move any distance along diagonals.

## Result

Bishops now generate all diagonal pseudo-legal moves and captures.

---

# Step 12 — Rook Move Generation

## What was added

Implemented rook movement using four orthogonal directions.

```c
const int rookDirections[4][2]
```

Each direction continues until blocked.

## Why

Rooks move horizontally and vertically over any number of empty squares.

## Result

Rooks now generate every horizontal and vertical pseudo-legal move.

---

# Step 13 — Queen Move Generation

## What was added

Implemented queen movement by combining rook and bishop movement.

The queen searches:

- horizontal
- vertical
- diagonal

directions.

## Why

The queen is simply the combination of rook and bishop movement, allowing code reuse and keeping the implementation simple.

## Result

Queens now generate all pseudo-legal moves and captures.

---

# Step 14 — King Move Generation

## What was added

Implemented king movement using the eight adjacent squares.

```c
const int kingDirections[8][2]
```

Each destination is checked for:

- board boundaries
- empty squares
- enemy captures

## Why

Unlike sliding pieces, kings move only one square in any direction.

King safety is intentionally ignored during this phase.

## Result

Kings now generate all adjacent pseudo-legal moves.

---

# Step 15 — Castling Generation

## What was added

Implemented pseudo-legal castling generation.

The generator verifies:

- castling rights exist
- intermediate squares are empty

Generated moves receive:

```c
MOVE_CASTLING
```

## Why

Castling is a unique king move and cannot be generated with normal king movement.

Checking whether the king passes through check is deferred to Phase 4.

## Result

Kingside and queenside castling moves are now generated whenever their basic conditions are satisfied.

---

# Step 16 — Move Generation Pipeline

## What was added

Completed the engine's move generation pipeline.

```c
GenerateMoves(board, list)
```

calls:

```
Pawn
Knight
Bishop
Rook
Queen
King
```

in sequence.

## Why

The engine should expose one function that generates every possible pseudo-legal move.

## Result

Move generation is now centralized into a single entry point.

---

# Step 17 — Side-to-Move Filtering

## What was added

Each move generator now ignores pieces belonging to the opponent.

Example:

```c
if (board->sideToMove == SIDE_WHITE && piece != W_ROOK)
    continue;
```

## Why

A chess engine should only generate moves for the player whose turn it is.

Without this filter, both White and Black moves would appear in the same move list.

## Result

Generated move lists now contain moves for only one side.

---

# Step 18 — Move Execution System

## What was added

Created:

```c
MakeMove(Board *board, const Move *move);
```

Basic move execution now:

- removes the piece from the source square
- places it on the destination square

## Why

Generated moves must be executable so the engine can advance the game state.

## Result

The engine can now apply ordinary chess moves.

---

# Step 19 — Game State Updates

## What was added

Integrated game state updates into `MakeMove()`.

Updated:

- side to move
- halfmove clock
- fullmove number
- en passant target square

Example:

```c
board->sideToMove =
    (board->sideToMove == SIDE_WHITE)
    ? SIDE_BLACK
    : SIDE_WHITE;
```

## Why

Executing a move affects more than just piece locations.

The engine must also maintain auxiliary game information.

## Result

Board state remains synchronized after every move.

---

# Step 20 — Verification

## What was added

Tested every implemented generator individually using temporary board positions.

Verified:

- pawn movement
- knight movement
- bishop movement
- rook movement
- queen movement
- king movement
- promotions
- captures
- castling generation
- en passant generation

## Why

Testing each piece independently simplifies debugging and isolates implementation errors.

## Result

All pseudo-legal move generators were verified before extending `MakeMove()`.

# Step 21 — En Passant Execution

## What was added

Extended `MakeMove()` to correctly execute an en passant capture.

When a move contains:

```c
MOVE_EN_PASSANT
```

the captured pawn is removed from its original square rather than the destination square.

## Why

En passant is the only capture where the captured piece does not occupy the target square.

## Result

En passant moves now update the board correctly.

---

# Step 22 — Promotion Execution

## What was added

Extended `MakeMove()` to execute pawn promotions.

Instead of placing the pawn on the destination square, the promoted piece stored in the move is placed.

Example:

```c
if (move->flags & MOVE_PROMOTION)
    board->squares[move->toRow][move->toCol] = move->promotion;
```

## Why

Promotion permanently changes the moving piece.

## Result

Promotion moves now execute correctly.

---

# Step 23 — Castling Execution

## What was added

Extended `MakeMove()` to automatically move the rook whenever a castling move is executed.

Handled:

- White kingside
- White queenside
- Black kingside
- Black queenside

## Why

Castling moves two pieces simultaneously.

Moving only the king would leave the board in an invalid state.

## Result

Castling now updates both the king and rook positions correctly.

---

# Step 24 — Castling Rights Updates

## What was added

Implemented automatic updates to castling rights.

Castling rights are removed when:

- a king moves
- a rook moves
- a rook is captured

Example:

```c
board->castling.whiteKingSide = 0;
```

## Why

Once a king or rook moves, the corresponding castling option is permanently lost.

## Result

Castling rights now remain synchronized with the game state.

---

# Step 25 — Complete MakeMove()

## What was added

Integrated all move execution features into a single function.

`MakeMove()` now supports:

- Normal moves
- Captures
- Double pawn pushes
- En passant target creation
- En passant execution
- Promotions
- Castling
- Castling rights updates
- Halfmove clock
- Fullmove number
- Side-to-move switching

## Why

A single, centralized move execution function keeps the engine consistent and easier to maintain.

## Result

Every generated pseudo-legal move can now be executed correctly.

---

# Step 26 — Full Engine Verification

## What was added

Verified every implemented move type using isolated test positions.

Tested:

- pawn movement
- knight movement
- bishop movement
- rook movement
- queen movement
- king movement
- promotions
- captures
- en passant
- castling
- move execution

## Why

Incremental testing reduces debugging complexity and validates each subsystem independently.

## Result

All implemented functionality behaved as expected.

---

# Step 27 — Final Move Generation Pipeline

The complete engine pipeline is now:

```
Board
   │
   ▼
GenerateMoves()
   │
   ▼
MoveList
   │
   ▼
MakeMove()
   │
   ▼
Updated Board
```

The board remains the engine's single source of truth throughout move generation and execution.

---

# Step 28 — Project Architecture

Current project structure:

```
C_Chess_Engine/
│
├── assets/
│   └── pieces/
│
├── engine/
│   ├── board.c
│   ├── board.h
│   ├── fen.c
│   ├── fen.h
│   ├── move.c
│   ├── move.h
│   ├── movegen.c
│   └── movegen.h
│
├── ui/
│   ├── renderer.c
│   └── renderer.h
│
├── main.c
└── Makefile
```

The project now cleanly separates:

- Board representation
- Move generation
- Move execution
- Rendering
- Application entry point

---

# Step 29 — Phase Summary

## Problems Solved

- Introduced a standardized move representation.
- Centralized move generation into a dedicated module.
- Added support for every pseudo-legal chess move.
- Implemented complete move execution.
- Kept board state synchronized after every move.
- Maintained clean separation between engine and renderer.

---

# Key Concepts Learned

- Difference between pseudo-legal and legal moves.
- Using bit flags to describe move properties.
- Sliding-piece move generation.
- Direction-table based movement.
- Separating move generation from move execution.
- Maintaining board state consistently.
- Updating auxiliary game information after every move.

---

# Phase 3 Deliverable (COMPLETE)

The engine now supports:

✔ Pawn move generation

✔ Knight move generation

✔ Bishop move generation

✔ Rook move generation

✔ Queen move generation

✔ King move generation

✔ Captures

✔ Double pawn pushes

✔ Promotions

✔ En passant generation

✔ Castling generation

✔ Complete move execution

✔ Side-to-move filtering

✔ Game state updates

✔ Castling rights management

✔ Pseudo-legal move generation

---

# Current Project State

The chess engine now contains:

✔ Complete board representation

✔ Graphical renderer

✔ FEN generation

✔ Complete pseudo-legal move generator

✔ Complete move execution system

✔ Engine-driven architecture

The engine is capable of generating and executing every pseudo-legal chess move.

However, it does **not** yet enforce king safety.

Not yet implemented:

- Legal move generation
- Attack detection
- Check detection
- Checkmate detection
- Stalemate detection
- Draw detection

---

# Conclusion

Phase 3 transforms the project from a graphical board renderer into a functioning chess engine capable of generating and executing every pseudo-legal move.

With move generation and execution complete, the engine is now ready for Phase 4, where move legality will be enforced by introducing attack detection, check detection, and legal move filtering.

---

# Next Phase

## Phase 4 — Legal Move Generation

Planned implementation order:

1. Board copying (`Board_Copy`)
2. Attack detection (`IsSquareAttacked`)
3. Check detection (`IsKingInCheck`)
4. Legal move filtering
5. Checkmate detection
6. Stalemate detection

The engine will then transition from generating **pseudo-legal** moves to generating **fully legal** chess moves.