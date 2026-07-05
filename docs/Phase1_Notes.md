# Phase 1_Notes.md

# Phase 1 – Board Representation

## Objective

Build the core internal data model of a chess engine.

By the end of this phase, the engine should be able to:

* Represent all chess pieces on an 8×8 board
* Store full game state (not just pieces)
* Initialize a standard chess starting position
* Print the board for debugging
* Serve as the foundation for move generation and rendering

This phase focuses entirely on **data representation**, not graphics or gameplay.

---

## Starting Point

At the beginning of this phase:

* The project already had a working build system (Makefile + Raylib + GCC)
* A basic Raylib window existed from Phase 0 testing
* Git repository was clean and initialized
* No real chess logic existed yet
* The engine could not represent a real chess position

This phase began from a working environment with only infrastructure, not engine logic.

---

## Board Representation (8×8 Array)

The chessboard is represented using a 2D array:

```c
Piece squares[8][8];
```

### Coordinate System

* row 0 → rank 8 (black side)
* row 7 → rank 1 (white side)
* col 0 → file a
* col 7 → file h

This mapping ensures compatibility with standard chess notation and future FEN support.

---

## Piece Encoding

Pieces are represented using an enumeration:

```c
typedef enum
{
    EMPTY,

    W_PAWN,
    W_KNIGHT,
    W_BISHOP,
    W_ROOK,
    W_QUEEN,
    W_KING,

    B_PAWN,
    B_KNIGHT,
    B_BISHOP,
    B_ROOK,
    B_QUEEN,
    B_KING

} Piece;
```

Each piece is stored as an integer value, making the engine simple and efficient.

---

## Game State Structure

```c
typedef enum
{
    SIDE_WHITE,
    SIDE_BLACK
} Side;

typedef struct
{
    int whiteKingSide;
    int whiteQueenSide;
    int blackKingSide;
    int blackQueenSide;
} CastlingRights;

typedef struct
{
    Piece squares[8][8];

    Side sideToMove;

    CastlingRights castling;

    int enPassantSquare;

    int halfmoveClock;
    int fullmoveNumber;

} Board;
```

This structure allows the engine to represent a complete chess position, not just pieces.

---

## Side to Move

Tracks which player is to move:

* SIDE_WHITE
* SIDE_BLACK

This is required for move generation and rule validation.

---

## Castling Rights

Tracks whether each side is allowed to castle:

* White king-side
* White queen-side
* Black king-side
* Black queen-side

These values will later be updated during move execution.

---

## En Passant Square

```c
int enPassantSquare;
```

* Stores the square index where en passant capture is possible
* Value `-1` means no en passant available

---

## Move Counters

* halfmoveClock → used for 50-move rule tracking
* fullmoveNumber → counts full turns in the game

---

## Board Initialization

Implemented `Board_Init(Board *board)` which:

### 1. Clears the board

All squares are set to EMPTY.

### 2. Places standard chess starting position

r n b q k b n r  
p p p p p p p p  
. . . . . . . .  
. . . . . . . .  
. . . . . . . .  
. . . . . . . .  
P P P P P P P P  
R N B Q K B N R  

### 3. Initializes game state

* sideToMove = SIDE_WHITE
* all castling rights enabled
* enPassantSquare = -1
* halfmoveClock = 0
* fullmoveNumber = 1

---

## Debug Printing System

Implemented:

```c
void Board_Print(const Board *board);
```

### Purpose

Used to verify engine state in the terminal before rendering.

### Output Example

r n b q k b n r  
p p p p p p p p  
. . . . . . . .  
. . . . . . . .  
. . . . . . . .  
. . . . . . . .  
P P P P P P P P  
R N B Q K B N R  

---

## Raylib Naming Conflict Fix

Raylib defines:

* Color
* WHITE
* BLACK

These conflicted with engine definitions.

### Fix applied:

* Color → Side
* WHITE → SIDE_WHITE
* BLACK → SIDE_BLACK

This prevented naming collisions with Raylib headers.

---

## Build System Update

The Makefile was updated to compile multiple source files:

SRC = main.c engine/board.c

This ensured both engine and application code are linked correctly.

---

## Debugging Tools Introduced

* Terminal-based board printing
* Board initialization test inside main.c
* Multi-file compilation support

---

## Git Progress

Completed:

* Git repository already initialized from Phase 0
* First commit pushed successfully to GitHub
* Clean project structure maintained

---

## Problems Encountered

### 1. Undefined reference to Board_Init

Cause:
board.c not included in compilation

Fix:
Added engine/board.c to Makefile

---

### 2. Raylib naming conflicts

Cause:
Engine enums conflicted with Raylib macros/types

Fix:
Renamed Color → Side and adjusted usage

---

### 3. Multi-file linking issues

Cause:
Only main.c was being compiled

Fix:
Updated Makefile to include all engine source files

---

## What I Learned

* C projects must compile all .c files together
* Header files define structure, .c files define behavior
* Structs are essential for representing game state cleanly
* Enums improve readability and reduce errors
* Debug printing is critical before rendering
* External libraries can introduce naming conflicts
* Engine architecture must separate:
  * data (engine)
  * rendering (ui)
  * execution (main)

---

## Phase 1 Deliverable

The engine now supports:

* Full chess position representation
* Standard starting position initialization
* Complete game state tracking
* Terminal-based debugging output
* Multi-file compilation system

---

## Conclusion

Phase 1 successfully establishes the **core chess engine data layer**, which serves as the foundation for:

* Move generation (Phase 3)
* Rules validation (Phase 4)
* AI / search (Phase 5)
* Rendering system (Phase 2)
```