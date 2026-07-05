#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "../engine/board.h"

typedef struct {
    Texture2D wP, wN, wB, wR, wQ, wK;
    Texture2D bP, bN, bB, bR, bQ, bK;
} PieceTextures;

// FULL renderer state (now owns textures)
typedef struct {
    PieceTextures tex;
    int tileSize;
} Renderer;

// lifecycle
void InitRenderer(Renderer *r);
void UnloadRenderer(Renderer *r);

// draw
void DrawGame(Renderer *r, Board *board);

#endif