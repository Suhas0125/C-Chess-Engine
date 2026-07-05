````md
# Phase 2_Notes.md

# Phase 2 – Rendering System (Complete Development Log)

## Objective

Build a full graphical rendering system for the chess engine using Raylib, while keeping the Phase 1 engine unchanged and authoritative.

The goal of this phase is to turn the terminal-based chess engine into a fully visual system where:

- The chessboard is rendered on screen
- Pieces are drawn from engine state
- Assets are loaded and managed correctly
- Engine data drives all rendering
- Screen coordinates map correctly to board coordinates
- A clean rendering architecture is formed

This phase is strictly about visualization and engine connection, not move generation.

---

# Starting Point (End of Phase 1)

At the start of Phase 2, the project already had a complete chess engine:

## Engine already implemented:

- 8×8 board representation (`Piece squares[8][8]`)
- Full `Piece` enum (white + black pieces)
- `Board` struct containing:
  - board state
  - sideToMove
  - castling rights
  - en passant square
  - halfmove clock
  - fullmove number
- `Board_Init()` sets standard starting position
- `Board_Print()` for terminal debugging
- Multi-file structure:
  - main.c
  - engine/board.h
  - engine/board.c
- Makefile working
- Git repository initialized

## What was NOT present:

- No graphics system
- No Raylib rendering pipeline
- No textures or assets
- No input handling
- No UI layer
- No board → screen mapping

---

# Phase 2 Step 1 — Raylib Window Setup

Raylib was integrated into the project and a graphical window was created.

```c
InitWindow(800, 800, "C_Chess_Engine");
SetTargetFPS(60);
````

## Design decisions:

* 800×800 window
* 8×8 chessboard
* tileSize = 100

This creates a perfect 1:1 mapping between:

engine squares ↔ screen pixels

---

# Phase 2 Step 2 — Game Loop Creation

The main rendering loop was established:

```c
while (!WindowShouldClose())
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    EndDrawing();
}
```

## Key rules established:

* All drawing happens between BeginDrawing / EndDrawing
* ClearBackground resets frame each iteration
* Rendering system is separated from engine logic

---

# Phase 2 Step 3 — Chessboard Rendering

The board was rendered using nested loops:

```c
for (int row = 0; row < 8; row++)
for (int col = 0; col < 8; col++)
```

## Square coloring logic:

```c
Color color = ((row + col) % 2 == 0) ? BEIGE : BROWN;
```

## Screen mapping:

* row → y-axis
* col → x-axis

```text
x = col * tileSize
y = row * tileSize
```

## Result:

* Full 8×8 chessboard rendered
* Correct alternating colors
* Stable coordinate system established

---

# Phase 2 Step 4 — Engine → Renderer Connection

The renderer was connected directly to engine state:

```c
board.squares[row][col]
```

## Critical design rule established:

> Renderer never stores game state
> Engine is the single source of truth

---

# Phase 2 Step 5 — Debug Piece Rendering (Letters)

Before textures were introduced, pieces were rendered as ASCII characters:

* White pieces: P N B R Q K
* Black pieces: p n b r q k

## Purpose:

* Validate board indexing
* Confirm coordinate correctness
* Ensure engine state matches visual output

---

# Phase 2 Step 6 — Texture System Introduction


## Asset Pipeline (Testing → Real Chess Pieces)

Before integrating real chess piece assets, the rendering system was validated step-by-step using progressive visual tests to ensure correctness of the engine → renderer pipeline.

---

## Step 1 — Red Square Test Texture (Initial Rendering Validation)

Before using any chess piece images, a **procedural red square texture** was generated and used as a placeholder.

### Method

A simple generated texture was created using Raylib:

```c
Image img = GenImageColor(80, 80, RED);
Texture2D tex = LoadTextureFromImage(img);
UnloadImage(img);
````

### Purpose

This was used to verify:

* Texture loading works correctly
* DrawTexture / DrawTexturePro works correctly
* Board coordinates map correctly to screen positions
* Rendering pipeline is stable before introducing external assets

---

## Step 2 — Single Piece Rendering Test (Rook Only)

After confirming the red square system worked, the next step was to test actual board mapping using a **single chess piece (rook)**.

### Test Setup

* Only **white rook** texture was used
* Rendered at its correct starting position
* Verified engine → renderer mapping

### Purpose

* Confirm piece-to-square mapping is correct
* Ensure DrawPiece logic works for real assets
* Validate coordinate alignment in a real scenario

---

## Step 3 — Pawn Rendering Tests (Progressive Expansion)

After rook validation, testing was expanded gradually.

### Test 3.1 — White Pawns Only

White pawns were rendered on:

```text
row = 6
```

This confirmed:

* Entire row rendering works
* Loop iteration over board is correct

---

### Test 3.2 — White + Black Pawns

Next test expanded to both sides:

* White pawns → row 6
* Black pawns → row 1

### Verified:

* Dual-side rendering works
* Board indexing is correct
* Piece enum mapping is correct
* Color separation is correct

---

## Step 4 — Transition to Real Chess Assets (SVG Source)

After confirming rendering stability, real chess assets were introduced.

### Asset Source

Chess pieces were downloaded from a GitHub repository (SVG format).

Example sources (typical open chess asset packs):

* [https://github.com/](https://github.com/)
* (SVG chess piece sets such as "Wikipedia chess pieces", "cburnett chess set", or similar open sets)

*(Exact repo used was not recorded during development)*

---

## Step 5 — SVG → PNG Conversion Pipeline

Since Raylib works best with raster images, all SVG files were converted into PNG format.

### Tool Used

Inkscape (command-line conversion tool)

Official download:
[https://inkscape.org/](https://inkscape.org/)

---

### Conversion Command

All 12 chess pieces were batch-converted using CMD:

```bash
inkscape input.svg --export-type=png --export-filename=output.png
```

---

### Batch Conversion Example

For all pieces:

```bash
for f in *.svg; do inkscape "$f" --export-type=png --export-filename="${f%.svg}.png"; done
```

---

## Step 6 — Final Asset Integration into Engine

After conversion, all PNG files were placed into:

```text
assets/pieces/
```

### Folder Structure

```text
C_Chess_Engine/
└── assets/
    └── pieces/
        ├── wP.png
        ├── wN.png
        ├── wB.png
        ├── wR.png
        ├── wQ.png
        ├── wK.png
        ├── bP.png
        ├── bN.png
        ├── bB.png
        ├── bR.png
        ├── bQ.png
        └── bK.png
```

---

## Step 7 — Loading Assets into Engine (main.c)

All textures were then loaded in `main.c`:

```c
Texture2D whitePawnTexture   = LoadTexture("assets/pieces/wP.png");
Texture2D whiteKnightTexture = LoadTexture("assets/pieces/wN.png");
Texture2D whiteBishopTexture = LoadTexture("assets/pieces/wB.png");
Texture2D whiteRookTexture   = LoadTexture("assets/pieces/wR.png");
Texture2D whiteQueenTexture  = LoadTexture("assets/pieces/wQ.png");
Texture2D whiteKingTexture   = LoadTexture("assets/pieces/wK.png");

Texture2D blackPawnTexture   = LoadTexture("assets/pieces/bP.png");
Texture2D blackKnightTexture = LoadTexture("assets/pieces/bN.png");
Texture2D blackBishopTexture = LoadTexture("assets/pieces/bB.png");
Texture2D blackRookTexture   = LoadTexture("assets/pieces/bR.png");
Texture2D blackQueenTexture  = LoadTexture("assets/pieces/bQ.png");
Texture2D blackKingTexture   = LoadTexture("assets/pieces/bK.png");
```

---

## Step 8 — Result of Full Asset Pipeline

After completing this pipeline:

* Procedural texture testing validated rendering system
* Single-piece rendering validated mapping system
* Pawn-based scaling validated full board iteration
* SVG assets were converted successfully
* PNG assets were integrated into Raylib
* Full 12-piece chess set rendering became possible

---

## Final Outcome

This pipeline ensured that:

✔ Rendering system was validated BEFORE real assets
✔ Engine mapping was verified incrementally
✔ Asset pipeline was reliable and reproducible
✔ Final chess visuals are fully engine-driven

---

## Key Insight

> Visual correctness was built in layers:
>
> 1. Generated texture (red square)
> 2. Single piece (rook)
> 3. Partial board (pawns)
> 4. Full board (both sides)
> 5. Real assets (SVG → PNG)
>
> This prevented debugging complexity and ensured stability at every stage.

```

Chess piece assets were introduced:

```
assets/pieces/
```

## Files:

* wP.png wN.png wB.png wR.png wQ.png wK.png
* bP.png bN.png bB.png bR.png bQ.png bK.png

---

# Phase 2 Step 7 — Texture Loading

Each texture was loaded manually:

```c
Texture2D whitePawnTexture = LoadTexture("assets/pieces/wP.png");
Texture2D blackPawnTexture = LoadTexture("assets/pieces/bP.png");
```

## Reason:

* Explicit control over assets
* Easy debugging
* Direct mapping between piece and texture

---

# Phase 2 Step 8 — PieceTextures Struct

To reduce complexity, textures were grouped:

```c
typedef struct {
    Texture2D wP, wN, wB, wR, wQ, wK;
    Texture2D bP, bN, bB, bR, bQ, bK;
} PieceTextures;
```

## Benefit:

* Avoids 12 separate variables everywhere
* Centralizes rendering assets
* Makes renderer scalable

---

# Phase 2 Step 9 — DrawPiece System

A dedicated rendering function was introduced:

```c
void DrawPiece(Piece p, int row, int col, int tileSize, PieceTextures tex)
```

## Responsibilities:

* Convert Piece → Texture
* Select correct texture
* Render piece on correct square

---

# Phase 2 Step 10 — DrawTexturePro Integration

Instead of DrawTexture, the system used:

```c
DrawTexturePro()
```

## Reason:

* Needed scaling control
* Needed proper fitting into squares
* Required consistent rendering quality

---

# Phase 2 Step 11 — Texture Centering Fix

Initial issue:

* Pieces rendered at top-left corner

Fix:

```c
int x = col * tileSize + (tileSize - texSize) / 2;
int y = row * tileSize + (tileSize - texSize) / 2;
```

## Result:

* Perfect centering inside each square
* Visual alignment fixed

---

# Phase 2 Step 12 — Full Rendering Pipeline

Final pipeline:

```
Board (engine)
→ board.squares[row][col]
→ DrawPiece()
→ PieceTextures
→ DrawTexturePro()
→ Screen output
```

---

# Phase 2 Step 13 — Renderer Refactor (UI Layer)

Rendering was moved out of main.c into a dedicated system:

```
ui/
├── renderer.h
└── renderer.c
```

## New responsibilities:

### Before:

* main.c handled rendering

### After:

* renderer handles:

  * drawing board
  * drawing pieces
  * managing rendering flow

* main.c only:

  * initializes systems
  * runs game loop

---

# Phase 2 Step 14 — SVG → PNG Pipeline

Chess assets were created using:

* Inkscape

## Workflow:

* Import SVG
* Export PNG
* Ensure square aspect ratio
* Maintain transparency

---

# Phase 2 Step 15 — Resource Management

Rule enforced:

> Every LoadTexture must have matching UnloadTexture

At shutdown:

* All textures are properly freed

---

# Phase 2 Step 16 — Mouse Coordinate Mapping

Mouse position mapped to board:

```c
Vector2 mouse = GetMousePosition();

int col = mouse.x / tileSize;
int row = mouse.y / tileSize;
```

## Purpose:

* Convert screen → board coordinates
* Enable square selection system
* Prepare for input handling

---

# Phase 2 Step 17 — Click Debug System

Mouse clicks were used for debugging:

* Print selected square
* Print piece at square

## Purpose:

* Validate input mapping
* Verify engine-render synchronization

---

# Phase 2 Step 18 — FEN System Integration

Files added:

```
engine/fen.h
engine/fen.c
```

## Function:

```c
void Board_ToFEN(const Board *b, char *out);
```

## Output example:

```
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
```

## Purpose:

* Engine state validation
* Debug snapshots
* Standard chess representation support

---

# Phase 2 Step 19 — Build System Update

Makefile updated to include:

* main.c
* engine/board.c
* engine/fen.c
* ui/renderer.c

## Result:

* Full multi-file compilation
* Stable engine + renderer integration

---

# Phase 2 Step 20 — Final System State

## Fully working systems:

✔ Raylib window system
✔ Chessboard rendering
✔ Engine-driven piece rendering
✔ Texture system (12 pieces)
✔ Proper scaling and centering
✔ Renderer module (ui/)
✔ FEN generation system
✔ Mouse coordinate mapping
✔ Click-based debugging
✔ Multi-file architecture

---

# Problems Solved During Phase 2

* Texture misalignment → fixed centering math
* Rendering clutter → moved to renderer module
* Too many variables → introduced PieceTextures struct
* Asset inconsistency → standardized PNG pipeline
* Screen-to-board mapping errors → fixed coordinate logic
* Architecture confusion → introduced ui separation

---

# Key Concepts Learned

* Engine must remain the single source of truth
* Renderer must be stateless
* Grid math defines correctness of visualization
* Texture systems must be centralized
* Debug rendering is essential during development
* Asset pipeline is part of engine design
* Architecture must evolve incrementally

---

# Phase 2 Deliverable (COMPLETE)

The system now includes:

* Fully graphical chessboard
* Engine-driven rendering system
* 12-piece texture system
* Clean rendering abstraction
* UI module (renderer.c)
* Asset pipeline (SVG → PNG)
* FEN generation system
* Mouse input mapping
* Click-based debugging system
* Stable multi-file build system

---

# Current Project State

The project is now:

✔ Fully graphical
✔ Fully engine-connected
✔ Architecturally separated
✔ Input-mapping capable
✔ Debug-ready
✔ FEN-capable

BUT:

* Move generation is NOT implemented
* Legal move rules are NOT implemented
* Game state is NOT affected by input
* No gameplay logic yet

---

# Conclusion

Phase 2 successfully transforms the chess engine from a terminal-based system into a fully graphical engine-driven rendering system.

The foundation is now complete for Phase 3:

> Move Generation System (pseudo-legal → legal moves → game rules)

```
