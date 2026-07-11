#include "renderer.h"
#include "../engine/move.h"
#include <math.h>

// -------------------------
// internal helper
// -------------------------
static Texture2D LoadPieceTexture(const char *path){
    return LoadTexture(path);
}

// Draws a piece at exact pixel coordinates (useful for dragging)
static void DrawPieceEx(Piece p, float x, float y, int tileSize, PieceTextures *tex){
    if (p == EMPTY) return;

    Texture2D t;
    switch (p)
    {
        case W_PAWN:   t = tex->wP; break;
        case W_KNIGHT: t = tex->wN; break;
        case W_BISHOP: t = tex->wB; break;
        case W_ROOK:   t = tex->wR; break;
        case W_QUEEN:  t = tex->wQ; break;
        case W_KING:   t = tex->wK; break;

        case B_PAWN:   t = tex->bP; break;
        case B_KNIGHT: t = tex->bN; break;
        case B_BISHOP: t = tex->bB; break;
        case B_ROOK:   t = tex->bR; break;
        case B_QUEEN:  t = tex->bQ; break;
        case B_KING:   t = tex->bK; break;
        default: return;
    }

    Rectangle source = {0, 0, t.width, t.height};
    Rectangle dest = { x, y, tileSize, tileSize };
    DrawTexturePro(t, source, dest, (Vector2){0,0}, 0.0f, WHITE);
}

// Draws an arrow
static void DrawArrow(Vector2 start, Vector2 end, Color color) {
    float thick = 6.0f;
    DrawLineEx(start, end, thick, color);
    
    Vector2 dir = { end.x - start.x, end.y - start.y };
    float length = sqrtf(dir.x*dir.x + dir.y*dir.y);
    if (length == 0) return;
    
    dir.x /= length; 
    dir.y /= length;
    
    float headSize = 25.0f;
    Vector2 perp = { -dir.y, dir.x };
    
    Vector2 p1 = end;
    Vector2 p2 = { end.x - dir.x * headSize + perp.x * (headSize * 0.5f), 
                   end.y - dir.y * headSize + perp.y * (headSize * 0.5f) };
    Vector2 p3 = { end.x - dir.x * headSize - perp.x * (headSize * 0.5f), 
                   end.y - dir.y * headSize - perp.y * (headSize * 0.5f) };
                   
    DrawTriangle(p1, p2, p3, color);
}

// -------------------------
// public API
// -------------------------
void InitRenderer(Renderer *r) {
    r->tileSize = 100;
    r->tex.wP = LoadPieceTexture("assets/pieces/wP.png");
    r->tex.wN = LoadPieceTexture("assets/pieces/wN.png");
    r->tex.wB = LoadPieceTexture("assets/pieces/wB.png");
    r->tex.wR = LoadPieceTexture("assets/pieces/wR.png");
    r->tex.wQ = LoadPieceTexture("assets/pieces/wQ.png");
    r->tex.wK = LoadPieceTexture("assets/pieces/wK.png");

    r->tex.bP = LoadPieceTexture("assets/pieces/bP.png");
    r->tex.bN = LoadPieceTexture("assets/pieces/bN.png");
    r->tex.bB = LoadPieceTexture("assets/pieces/bB.png");
    r->tex.bR = LoadPieceTexture("assets/pieces/bR.png");
    r->tex.bQ = LoadPieceTexture("assets/pieces/bQ.png");
    r->tex.bK = LoadPieceTexture("assets/pieces/bK.png");
}

void UnloadRenderer(Renderer *r) {
    UnloadTexture(r->tex.wP); UnloadTexture(r->tex.wN); UnloadTexture(r->tex.wB);
    UnloadTexture(r->tex.wR); UnloadTexture(r->tex.wQ); UnloadTexture(r->tex.wK);
    UnloadTexture(r->tex.bP); UnloadTexture(r->tex.bN); UnloadTexture(r->tex.bB);
    UnloadTexture(r->tex.bR); UnloadTexture(r->tex.bQ); UnloadTexture(r->tex.bK);
}

void DrawGame(Renderer *r, Board *board, int selectedRow, int selectedCol, MoveList *selectedMoves, UIContext *ctx) {
    int ts = r->tileSize;

    // 1. Draw Board & Highlights
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Color color = ((row + col) % 2 == 0) ? BEIGE : BROWN;

            // Highlight last played move
            if (ctx->hasLastMove && 
               ((row == ctx->lastMove.fromRow && col == ctx->lastMove.fromCol) || 
                (row == ctx->lastMove.toRow && col == ctx->lastMove.toCol))) {
                color = (Color){205, 210, 106, 255}; // Yellow highlight
            }

            // Right-click highlight
            for (int i = 0; i < ctx->highlightCount; i++) {
                if (ctx->highlights[i].row == row && ctx->highlights[i].col == col) {
                    color = (Color){235, 97, 80, 255}; // Red highlight
                }
            }

            // Currently selected square
            if (row == selectedRow && col == selectedCol) {
                color = GOLD;
            }

            DrawRectangle(col * ts, row * ts, ts, ts, color);
        }
    }

    // 2. Draw Pieces (Skip dragged piece)
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (ctx->isDragging && row == ctx->dragRow && col == ctx->dragCol) {
                continue; 
            }
            Piece p = board->squares[row][col];
            DrawPieceEx(p, col * ts, row * ts, ts, &r->tex);
        }
    }

    // 3. Draw Legal Moves (On top of pieces)
    for (int i = 0; i < selectedMoves->count; i++) {
        Move move = selectedMoves->moves[i];
        float centerX = move.toCol * ts + ts / 2.0f;
        float centerY = move.toRow * ts + ts / 2.0f;

        if (board->squares[move.toRow][move.toCol] != EMPTY || (move.flags & MOVE_EN_PASSANT)) {
            // Draw a thick ring for captures
            DrawRing((Vector2){centerX, centerY}, ts * 0.40f, ts * 0.48f, 0, 360, 36, (Color){0, 0, 0, 60});
        } else {
            // Draw a solid dot for empty squares
            DrawCircle(centerX, centerY, ts * 0.15f, (Color){0, 0, 0, 60});
        }
    }

    // 4. Draw Arrows
    for (int i = 0; i < ctx->arrowCount; i++) {
        Vector2 start = { ctx->arrows[i].fromCol * ts + ts/2.0f, ctx->arrows[i].fromRow * ts + ts/2.0f };
        Vector2 end = { ctx->arrows[i].toCol * ts + ts/2.0f, ctx->arrows[i].toRow * ts + ts/2.0f };
        DrawArrow(start, end, (Color){255, 170, 0, 200});
    }

    // 5. Draw Dragged Piece (On top of everything)
    if (ctx->isDragging) {
        Piece p = board->squares[ctx->dragRow][ctx->dragCol];
        DrawPieceEx(p, ctx->mousePos.x - ts / 2, ctx->mousePos.y - ts / 2, ts, &r->tex);
    }
}