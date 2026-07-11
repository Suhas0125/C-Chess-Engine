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

void DrawGame(
    Renderer *r,
    Board *board,
    int selectedRow,
    int selectedCol,
    MoveList *selectedMoves,
    UIContext *ctx
);

#endif