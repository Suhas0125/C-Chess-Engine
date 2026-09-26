#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "../engine/board.h"
#include "../engine/move.h"

// -------------------------
// UI State Structs
// -------------------------
typedef struct {
    int row;
    int col;
} UISquare;

typedef struct {
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
} UIArrow;

// Holds all visual state for the current frame
typedef struct {
    bool isDragging;
    int dragRow;
    int dragCol;
    Vector2 mousePos;

    bool hasLastMove;
    Move lastMove;

    UISquare highlights[64];
    int highlightCount;

    UIArrow arrows[64];
    int arrowCount;
} UIContext;

// -------------------------
// Renderer Struct
// -------------------------
typedef struct {
    Texture2D wP, wN, wB, wR, wQ, wK;
    Texture2D bP, bN, bB, bR, bQ, bK;
} PieceTextures;

typedef struct {
    PieceTextures tex;
    int tileSize;
} Renderer;

// -------------------------
// Lifecycle & Drawing
// -------------------------
void InitRenderer(Renderer *r);
void UnloadRenderer(Renderer *r);

// flipped: false draws White's home rank at the bottom (screen row 7),
// true draws Black's home rank at the bottom (screen row 7). All board
// coordinates passed in (selectedRow/Col, moves, ctx->highlights/arrows,
// ctx->dragRow/Col) are always given in true board coordinates
// (row 0 = rank 8); the renderer alone handles the visual flip.
void DrawGame(
    Renderer *r,
    Board *board,
    int selectedRow,
    int selectedCol,
    MoveList *selectedMoves,
    UIContext *ctx,
    bool flipped
);

// Draws both players' remaining time. Each clock is placed on the
// screen edge matching that color's home rank, so it stays correct
// whether or not the board is flipped. The side to move's clock is
// highlighted.
void DrawClocks(long long whiteTimeMs, long long blackTimeMs, Side sideToMove, bool flipped);

// What the player clicked on the game-over screen, if anything.
typedef enum {
    GAMEOVER_NONE,
    GAMEOVER_PLAY_AGAIN,
    GAMEOVER_QUIT
} GameOverAction;

// Draws a game-over banner with the given result message plus
// "Play Again" / "Quit" buttons. Returns which one (if any) was
// clicked this frame.
GameOverAction DrawGameOver(const char *message);

#endif